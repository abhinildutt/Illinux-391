#define NUM_SCANCODES 89
#define RELEASED_SCANCODE_OFFSET 0x80

// Special scancodes
#define CODE_BACKSPACE 0x0E
#define CODE_TAB 0x0F
#define CODE_ENTER 0x1C
#define CODE_LEFT_CONTROL 0x1D
#define CODE_L 0x26
#define CODE_LEFT_SHIFT 0x2A
#define CODE_RIGHT_SHIFT 0x36
#define CODE_CAPS_LOCK 0x3A
#define CODE_EXTENDED 0xE0

// https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_1
// Index 0 is lowercase, index 1 is capital
const char scancodeToKey[NUM_SCANCODES][2] = {
    {'\0', '\0'},
    {'\0', '\0'}, // 0x01 = escape
    {'1', '!'},
    {'2', '@'},
    {'3', '#'},
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'},
    {'8', '*'},
    {'9', '('},
    {'0', ')'},
    {'-', '_'},
    {'=', '+'},
    {'\0', '\0'}, // 0x0E = backspace
    {'\0', '\0'}, // 0x0F = tab
    {'q', 'Q'},
    {'w', 'W'},
    {'e', 'E'},
    {'r', 'R'},
    {'t', 'T'},
    {'y', 'Y'},
    {'u', 'U'},
    {'i', 'I'},
    {'o', 'O'},
    {'p', 'P'},
    {'[', '{'},
    {']', '}'},
    {'\0', '\0'}, // 0x1C = enter
    {'\0', '\0'}, // 0x1D = left control
    {'a', 'A'},
    {'s', 'S'},
    {'d', 'D'},
    {'f', 'F'},
    {'g', 'G'},
    {'h', 'H'},
    {'j', 'J'},
    {'k', 'K'},
    {'l', 'L'},   // 0x26 = L
    {';', ':'},
    {'\'', '"'},
    {'`', '~'},
    {'\0', '\0'}, // 0x2A = left shift
    {'\\', '|'},
    {'z', 'Z'},
    {'x', 'X'},
    {'c', 'C'},
    {'v', 'V'},
    {'b', 'B'},
    {'n', 'N'},
    {'m', 'M'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
    {'\0', '\0'}, // 0x36 = right shift
    {'*', '*'},   // 0x37 = keypad *
    {'\0', '\0'}, // 0x38 = left alt
    {' ', ' '},   // 0x39 = space
    {'\0', '\0'}, // 0x3A = CapsLock
    {'\0', '\0'}, // 0x3B = F1
    {'\0', '\0'}, // 0x3C = F2
    {'\0', '\0'}, // 0x3D = F3
    {'\0', '\0'}, // 0x3E = F4
    {'\0', '\0'}, // 0x3F = F5
    {'\0', '\0'}, // 0x40 = F6
    {'\0', '\0'}, // 0x41 = F7
    {'\0', '\0'}, // 0x42 = F8
    {'\0', '\0'}, // 0x43 = F9
    {'\0', '\0'}, // 0x44 = F10
    {'\0', '\0'}, // 0x45 = NumLock
    {'\0', '\0'}, // 0x46 = ScrollLock
    {'7', '7'},   // 0x47 = keypad 7
    {'8', '8'},   // 0x48 = keypad 8
    {'9', '9'},   // 0x49 = keypad 9
    {'-', '-'},   // 0x4A = keypad -
    {'4', '4'},   // 0x4B = keypad 4
    {'5', '5'},   // 0x4C = keypad 5
    {'6', '6'},   // 0x4D = keypad 6
    {'+', '+'},   // 0x4E = keypad +
    {'1', '1'},   // 0x4F = keypad 1
    {'2', '2'},   // 0x50 = keypad 2
    {'3', '3'},   // 0x51 = keypad 3
    {'0', '0'},   // 0x52 = keypad 0
    {'.', '.'},   // 0x53 = keypad .
    {'\0', '\0'}, // unknown
    {'\0', '\0'}, // unknown
    {'\0', '\0'}, // unknown
    {'\0', '\0'},   // 0x57 = F11
    {'\0', '\0'},   // 0x58 = F12
};
