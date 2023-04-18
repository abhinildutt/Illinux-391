#include "filesys.h"
#include "../interrupt_handlers/syscalls_def.h"

funcptrs directory_fops = {
    .open = dir_open,
    .close = dir_close,
    .read = dir_read,
    .write = dir_write
};

funcptrs regular_fops = {
    .open = file_open,
    .close = file_close,
    .read = file_read,
    .write = file_write
};

/*
 * fs_init
 *   DESCRIPTION: Initializes the file system
 *   INPUTS: fs_start_addr: the address of the start of the file system
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets up the file system
 */
void fs_init(uint32_t * fs_start_addr) {
    if (fs_start_addr == NULL) {
        return;
    }
    boot_block_ptr = (boot_block_t*)((uint8_t *)fs_start_addr);
    inode_ptr = (inode_t*)(((uint8_t *)fs_start_addr) + BLOCK_SIZE);
    data_block_ptr = (data_block_t*)(((uint8_t *)fs_start_addr) + BLOCK_SIZE + BLOCK_SIZE*(boot_block_ptr->num_inodes));
}


/*
 * read_dentry_by_name
 *   DESCRIPTION: Reads the dentry by given name
 *   INPUTS: fname: the name of the file
 *           dentry: the dentry to be filled
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 if not successful
 *   SIDE EFFECTS: fills the dentry
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
    if (fname == NULL || dentry == NULL) {
        return -1;
    }

    int i;
    for (i = 0; i < boot_block_ptr->num_dentries; i++) {
        // Compare if the file name is the same as the given file name. If so, read the dentry
        if (strncmp((int8_t*)fname, (int8_t*)boot_block_ptr->dentries[i].filename, FILE_NAME_LEN) == 0) {
            return read_dentry_by_index(i, dentry);
        }
    }
    return -1;
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: Reads the dentry by given index
 *   INPUTS: index: the index of the file
 *           dentry: the dentry to be filled
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 if not successful
 *   SIDE EFFECTS: fills the dentry
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    if (index >= boot_block_ptr->num_dentries || dentry == NULL) {
        return -1;
    }
    memcpy((uint8_t*)dentry->filename, (uint8_t*)boot_block_ptr->dentries[index].filename, FILE_NAME_LEN);
    dentry->filetype = boot_block_ptr->dentries[index].filetype;
    dentry->inode_num = boot_block_ptr->dentries[index].inode_num;
    return 0;
}

/*
 * read_data
 *   DESCRIPTION: Reads data of a file
 *   INPUTS: inode: inode number of the file
 *           offset: the offset of the file's address
 *           buf: the buffer to be read into
 *           length: the length of the bytes to be read
 *   OUTPUTS: none
 *   RETURN VALUE: num bytes read if successful, -1 if not successful
 *   SIDE EFFECTS: fills the buffer
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    if (inode >= boot_block_ptr->num_inodes || buf == NULL) return -1;
    inode_t* curr_inode_ptr = (inode_t*)(&(inode_ptr[inode]));
    // offset beyond size of file, reached end of file
    if (offset >= curr_inode_ptr->length) return 0;

    // Get the data block number
    uint32_t data_block_idx = offset / BLOCK_SIZE;
    uint32_t data_block_offset = offset % BLOCK_SIZE;
    uint32_t curr_data_block_num = curr_inode_ptr->data_block_num[data_block_idx];
    // printf("offset: %d data_block_idx: %d\n", offset, data_block_idx);
    
    uint32_t bytes_read = 0;

    // Calculate the address of the data block
    uint8_t * curr_addr = (uint8_t *)(data_block_ptr) + curr_data_block_num * BLOCK_SIZE + data_block_offset;

    // Read the data block until the length is reached
    while (bytes_read < length) {
        if (data_block_offset == BLOCK_SIZE) {
            data_block_idx++;
            data_block_offset = 0;
            // curr_addr = (uint32_t*)(&(data_block_ptr+ data_block_num * 4096 + data_block_offset));
            if (data_block_idx >= 1023) return -1;

            curr_data_block_num = curr_inode_ptr->data_block_num[data_block_idx];
            if (curr_data_block_num > boot_block_ptr->num_data_blocks) {
                return -1;
            }
            curr_addr = (uint8_t *)data_block_ptr + curr_data_block_num * BLOCK_SIZE;
        }
        memcpy((void*)(buf + bytes_read), curr_addr, 1);

        curr_addr++;
        bytes_read++;
        data_block_offset++;
    }
    // printf("read: %d\n", bytes_read);
    return bytes_read;
}

/*
 * file_open
 *   DESCRIPTION: Opens a file
 *   INPUTS: filename: the name of the file
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 if not successful
 *   SIDE EFFECTS: none
 */
int32_t file_open(fd_array_member_t* f, const uint8_t* filename){
    if (filename == NULL) {
        return -1;
    }
    // Check if filename > 32 bytes + potentially 1 null byte
    if (strlen((int8_t*)filename) > FILE_NAME_LEN + 1) {
        return -1;
    }

    return read_dentry_by_name(filename, &curr_dentry);
}

/*
 * file_read
 *   DESCRIPTION: Reads a file
 *   INPUTS: fd: the file descriptor
 *           buf: the buffer to be read into
 *           nbytes: the number of bytes to be read
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes read if successful, -1 if not successful
 *   SIDE EFFECTS: fills the buffer
 */
int32_t file_read(fd_array_member_t* f, void* buf, int32_t nbytes){
    int32_t fl = read_data(f->inode, f->file_pos, buf, nbytes);
    if (fl == -1) return -1;
    f->file_pos += fl;
    return fl;
}

/*
 * file_write
 *   DESCRIPTION: Writes to a file
 *   INPUTS: buf: the buffer to be written into
 *           nbytes: the number of bytes to be written
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */
int32_t file_write(fd_array_member_t* f, const void* buf, int32_t nbytes){    
    return -1;
}

/*
 * file_close
 *   DESCRIPTION: Closes a file
 *   INPUTS: fd: the file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t file_close(fd_array_member_t* f) {
    return 0;
}


/*
 * dir_read
 *   DESCRIPTION: Reads a directory
 *   INPUTS: fd: the file descriptor
 *           buf: the buffer to be read into
 *           nbytes: the number of bytes to be read
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes read if successful, -1 if not successful
 *   SIDE EFFECTS: fills the buffer
 */
int32_t dir_read(fd_array_member_t* f, void* buf, int32_t nbytes){
    if (buf == NULL) {
        return -1;
    }

    dentry_t dentry;
    if (read_dentry_by_index(f->file_pos, &dentry) == -1) {
        return 0;
    }
    // Store the filename, filetype, and file size into the buffer
    memcpy(buf, &(dentry.filename), FILE_NAME_LEN);
    // memcpy(buf + FILE_NAME_LEN, &(dentry.filetype), FILE_TYPE_SIZE);
    // uint32_t file_size = inode_ptr[dentry.inode_num].length;
    // memcpy(buf + FILE_NAME_LEN + FILE_TYPE_SIZE, &(file_size), FILE_SIZE_SIZE);

    // Increment file position by one every time you read directory
    f->file_pos++;
    return nbytes;
}


/*
 * dir_write
 *   DESCRIPTION: Writes to a directory
 *   INPUTS: fd: the file descriptor
 *           buf: the buffer to be written into
 *           nbytes: the number of bytes to be written
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */
int32_t dir_write(fd_array_member_t* f, const void* buf, int32_t nbytes){
    return -1;
}

/*
 * dir_open
 *   DESCRIPTION: Opens a directory
 *   INPUTS: filename: the name of the directory
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 if not successful
 *   SIDE EFFECTS: none
 */
int32_t dir_open(fd_array_member_t* f, const uint8_t* filename){
    if (filename == NULL) {
        return -1;
    }
    return read_dentry_by_name(filename, &curr_dentry);
}

/*
 * dir_close
 *   DESCRIPTION: Closes a directory
 *   INPUTS: fd: the file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t dir_close(fd_array_member_t* f) {
    return 0;
}
