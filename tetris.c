#include "tetris.h"

// Stores screen of height and width, where screen[i][j] is the color value at height i and width j.
char screen[SCREEN_HEIGHT][SCREEN_WIDTH];

// Global structs
block activeBlock;
coords previous[BLOCK_SIZE];
coords current[BLOCK_SIZE];

// Global variables
int slotIndex;
int score;
int tick;
bool gameLoop;
bool tempBlock[BLOCK_SIZE][BLOCK_SIZE];

int main(void)
{
    int timeoutms = 10;
    struct timespec to_sleep = {0, timeoutms * 1e6};

    setConioTerminalMode();

    initiateGame();

    while (true)
    {
        // Compute and print screen
        computeScreen();
        printScreen();

        // Handle user input
        int userInput = getchNonblockInput();

        handleUserInput(userInput);

        // Apply gravity every 50 ticks
        if (tick == 50)
        {
            collision('d');
            activeBlock.y++;
            tick = 0;
        }

        while (!gameLoop)
        {
            nanosleep(&to_sleep, &to_sleep);
            printScreen();
            printf("\nYou lost! You had a max score of %i\nPress 'p' to play again, or 'CTRL+C' to quit.\n\n", score);

            if (getchNonblockInput() == 'p')
            {
                // Reset screen
                for (int i = 0; i < SCREEN_HEIGHT; i++)
                {
                    for (int j = 0; j < SCREEN_WIDTH; j++)
                    {
                        screen[i][j] = '\0';
                    }
                }

                // Start new game
                initiateGame();
            }
        }

        // Delay
        nanosleep(&to_sleep, &to_sleep);
        tick++;
    }

    resetTerminalMode();
}

void initiateGame(void)
{
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            screen[i][j] = '\0';
        }
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand(usec);

    tick = 0;
    score = 0;
    slotIndex = 0;
    gameLoop = true;

    createNewBlock();
}

void checkLoseState(void)
{
    // Check collision of newly created block
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        int currentY = activeBlock.y + i - 2;
        if (currentY < 0 || currentY >= SCREEN_HEIGHT) continue;

        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            int currentX = activeBlock.x + j - 1;
            if (currentX < 0 || currentX >= SCREEN_WIDTH) continue;

            if (activeBlock.block[i][j] && screen[currentY][currentX] != '\0') gameLoop = false;
        }
    }

    return;
}

void checkLineClear(void)
{
    bool clearLine = true;
    int clearedCount = 0;

    while (clearLine)
    {
        for (int i = 0; i < SCREEN_HEIGHT; i++)
        {
            clearLine = true;

            for (int j = 0; j < SCREEN_WIDTH; j++)
            {
                if (screen[i][j] == '\0')
                {
                    clearLine = false;
                }
            }

            if (clearLine)
            {
                for (int y = i; y > 0; y--)
                {
                    for (int x = 0; x < SCREEN_WIDTH; x++)
                    {
                        screen[y][x] = screen[y - 1][x];
                    }
                }

                clearedCount++;
                break;
            }
        }
    }

    score += (int) round(pow(clearedCount, 1.34) * 10.0);

    return;
}

void createNewBlock(void)
{
    checkLineClear();

    // Reset current and previous
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        current[i].y = -1;
        current[i].x = -1;

        previous[i].x = -1;
        previous[i].y = -1;
    }

    // Create randomized order of BLOCK_COUNT blocks
    if (slotIndex == 0)
    {
        for (int i = 0; i < BLOCK_COUNT; i++)
        {
            int j = (rand() % BLOCK_COUNT);

            char temp = blockTypes[i];
            blockTypes[i] = blockTypes[j];
            blockTypes[j] = temp;
        }

        slotIndex = BLOCK_COUNT;
    }

    slotIndex--;
    activeBlock.x = 5;
    activeBlock.y = 0;
    activeBlock.type = blockTypes[slotIndex];

    // Apply correct block information
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            switch (activeBlock.type)
            {
                case 'r':
                    activeBlock.block[i][j] = redBlock[i][j];
                    break;
                case 'b':
                    activeBlock.block[i][j] = blueBlock[i][j];
                    break;
                case 'y':
                    activeBlock.block[i][j] = yellowBlock[i][j];
                    break;
                case 'Y':
                    activeBlock.block[i][j] = yellowBlockM[i][j];
                    break;
                case 'p':
                    activeBlock.block[i][j] = purpleBlock[i][j];
                    break;
                case 'g':
                    activeBlock.block[i][j] = greenBlock[i][j];
                    break;
                case 'G':
                    activeBlock.block[i][j] = greenBlockM[i][j];
                    break;
            }
        }
    }

    checkLoseState();

    return;
}

bool collision(char direction)
{
    switch (direction)
    {
        case 'd': // Down collision
            for (int i = 0; i < BLOCK_SIZE; i++)
            {
                int checkY = current[i].y + 1;
                if (checkY >= SCREEN_HEIGHT)
                {
                    createNewBlock();
                    return true;
                }

                bool continueLoop = false;

                for (int j = 0; j < BLOCK_SIZE; j++)
                {
                    if (checkY == current[j].y && current[i].x == current[j].x) continueLoop = true;
                }
                if (continueLoop) continue;

                if (screen[checkY][current[i].x] != '\0')
                {
                    createNewBlock();
                    return true;
                }
            }
            break;
        case 'l': // Left collision
            for (int i = 0; i < BLOCK_SIZE; i++)
            {
                int checkX = current[i].x - 1;
                if (checkX < 0) return true;

                bool continueLoop = false;

                for (int j = 0; j < BLOCK_SIZE; j++)
                {
                    if (checkX == current[j].x) continueLoop = true;
                }
                if (continueLoop) continue;

                if (screen[current[i].y][checkX] != '\0') return true;
            }
            break;
        case 'r': // Right collision
            for (int i = 0; i < BLOCK_SIZE; i++)
            {
                int checkX = current[i].x + 1;
                if (checkX >= SCREEN_WIDTH) return true;

                bool continueLoop = false;

                for (int j = 0; j < BLOCK_SIZE; j++)
                {
                    if (checkX == current[j].x) continueLoop = true;
                }
                if (continueLoop) continue;

                if (screen[current[i].y][checkX] != '\0') return true;
            }
            break;
        case 'R': // Rotation collision
            for (int i = 0; i < BLOCK_SIZE; i++)
            {
                int currentY = activeBlock.y + i - 2;
                if (currentY < 0 || currentY >= SCREEN_HEIGHT) continue;

                for (int j = 0; j < BLOCK_SIZE; j++)
                {
                    int currentX = activeBlock.x + j - 1;
                    if (currentX < 0 || currentX >= SCREEN_WIDTH) return true;

                    if (tempBlock[i][j])
                    {
                        if (screen[currentY][currentX] == '\0') continue;

                        bool collisionSelf = false;
                        for (int idx = 0; idx < BLOCK_SIZE; idx++)
                        {
                            if (currentY == current[idx].y && currentX == current[idx].x) collisionSelf = true;
                        }
                        if (collisionSelf) continue;

                        return true;
                    }
                }
            }
            break;
    }

    return false;
}

void rotateActive(void)
{
    // Transpose activeBlock block info
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            tempBlock[i][j] = activeBlock.block[j][i];
        }
    }

    // Reverse each row
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        int l = 0, r = BLOCK_SIZE - 1;
        while (l < r)
        {
            bool temp = tempBlock[i][l];
            tempBlock[i][l] = tempBlock[i][r];
            tempBlock[i][r] = temp;

            l++;
            r--;
        }
    }

    // Check for collision, skip rotation if colliding
    if (collision('R')) return;

    // Apply rotation
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            activeBlock.block[i][j] = tempBlock[i][j];
        }
    }

    return;
}

void handleUserInput(char input)
{
    switch (input)
    {
        case 'w': // Rotate
            rotateActive();
            break;
        case 'a': // Move active left
            if (collision('l')) break;
            if (activeBlock.x <= 0)
            {
                activeBlock.x = SCREEN_WIDTH - 1;
                break;
            }
            activeBlock.x--;
            break;
        case 's': // Move active down
            if (collision('d')) break;
            activeBlock.y++;
            break;
        case 'd': // Move active right
            if (collision('r')) break;
            if (activeBlock.x >= SCREEN_WIDTH - 1)
            {
                activeBlock.x = 0;
                break;
            }
            activeBlock.x++;
            break;
    }

    return;
}

void computeScreen(void)
{
    // Clear screen of previous block position
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (previous[i].y == -1) continue;
        screen[previous[i].y][previous[i].x] = '\0';
    }

    int count = 0;

    // Place block on screen
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        int currentY = activeBlock.y + i - 2;
        if (currentY < 0 || currentY >= SCREEN_HEIGHT) continue;

        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            int currentX = activeBlock.x + j - 1;
            if (currentX < 0 || currentX >= SCREEN_WIDTH) continue;

            if (activeBlock.block[i][j])
            {
                current[count].x = currentX;
                current[count].y = currentY;
                count++;
            }
        }
    }

    // Save current block positions for later use
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (current[i].y == -1) continue;
        screen[current[i].y][current[i].x] = tolower(activeBlock.type);

        previous[i].x = current[i].x;
        previous[i].y = current[i].y;
    }

    return;
}

void printScreen(void)
{
    clear();
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        printf("<!");
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            switch (screen[y][x])
            {
                case '\0':
                    printf(" .");
                    break;
                case 'r':
                    printf("\033[31m[]\033[0m");
                    break;
                case 'b':
                    printf("\033[34m[]\033[0m");
                    break;
                case 'y':
                    printf("\033[93m[]\033[0m");
                    break;
                case 'p':
                    printf("\033[35m[]\033[0m");
                    break;
                case 'g':
                    printf("\033[92m[]\033[0m");
                    break;
            }
        }
        printf("!>\n");
    }

    printf("<!====================!>\n");
    printf("  \\/\\/\\/\\/\\/\\/\\/\\/\\/\\/  \n");
    printf("Current score -> %i\n", score);

    return;
}
