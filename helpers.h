
// Reset termios flags
void resetTerminalMode(void);

// Set termios flags for non canonical
void setConioTerminalMode(void);

// Check if a key was pressed
int kbhit(void);

// Read a single character
int getchNonblockInput(void);
