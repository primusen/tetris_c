#define _POSIX_C_SOURCE 199309L
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#include "helpers.h"

// Global static variables
#define SCREEN_HEIGHT 20
#define SCREEN_WIDTH 10
#define BLOCK_SIZE 4
#define BLOCK_COUNT 7

#define clear() printf("\033[H\033[J")

// Block definition
bool redBlock[BLOCK_SIZE][BLOCK_SIZE] = {{false, true, false, false},
                                        {false, true, false, false},
                                        {false, true, false, false},
                                        {false, true, false, false}};

bool blueBlock[BLOCK_SIZE][BLOCK_SIZE] = {{false, false, false, false},
                                        {false, true, true, false},
                                        {false, true, true, false},
                                        {false, false, false, false}};

bool yellowBlock[BLOCK_SIZE][BLOCK_SIZE] = {{false, true, false, false},
                                            {false, true, false, false},
                                            {false, true, true, false},
                                            {false, false, false, false}};

bool yellowBlockM[BLOCK_SIZE][BLOCK_SIZE] = {{false, false, true, false},
                                            {false, false, true, false},
                                            {false, true, true, false},
                                            {false, false, false, false}};


bool purpleBlock[BLOCK_SIZE][BLOCK_SIZE] = {{false, true, false, false},
                                            {false, true, true, false},
                                            {false, true, false, false},
                                            {false, false, false, false}};

bool greenBlock[BLOCK_SIZE][BLOCK_SIZE] = {{false, false, true, false},
                                            {false, true, true, false},
                                            {false, true, false, false},
                                            {false, false, false, false}};

bool greenBlockM[BLOCK_SIZE][BLOCK_SIZE] = {{false, false, false, false},
                                            {false, true, false, false},
                                            {false, true, true, false},
                                            {false, false, true, false}};


char blockTypes[BLOCK_COUNT] = {'r', 'b', 'y', 'Y', 'p', 'g', 'G'};

// Struct definition
typedef struct
{
    int x;
    int y;
    char type;
    bool block[BLOCK_SIZE][BLOCK_SIZE];
} block;

typedef struct
{
    int x;
    int y;
} coords;

// Function prototypes
void initiateGame(void);
void checkLoseState(void);
void checkLineClear(void);
void createNewBlock(void);
bool collision(char direction);
void rotateActive(void);
void handleUserInput(char input);
void computeScreen(void);
void printScreen(void);
