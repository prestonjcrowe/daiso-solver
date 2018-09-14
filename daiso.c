#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

const uint8_t SIDE_LENGTH = 10;
const uint8_t PIECE_SIZE = 4;
const char* RESET = "\033[0m;";

typedef struct board {
    uint8_t sidelength;
    char state[SIDE_LENGTH][SIDE_LENGTH];
} board;

const char PIECE_IDS[12] = {'a', 'b', 'c', 'd', 'e', 'f',
                            'g', 'h', 'i', 'j', 'k', 'l'};

const char COLORS[1][7] = {"\033[1;31m"};

const int PIECES[1][4][4] = {
    {{1,0,0,0},
     {1,0,0,0},
     {1,0,0,0},
     {1,0,0,0}}
};

void printboard(board *b);
void place(const int piece[4][4], int pindex, board *b, int row, int col);
void unplace(const int piece[4][4], board *b, int row, int col);
int isSafe(const int piece[4][4], board *b, int row, int col);
void explore(board *b, uint8_t index);

int main() {
    // Initialize the board state. 0 represents a free
    // space, 1 represents a cell that is off limits
    board *b = malloc(sizeof(board));
    b->sidelength = SIDE_LENGTH;
    for (int i = 0; i < SIDE_LENGTH; i++) {
        for (int j = 0; j < SIDE_LENGTH; j++) {
            if (i  >= (SIDE_LENGTH - j)) {
                b->state[i][j] = 1;
            }
            else {
                b->state[i][j] = 0;
            }
        }
    }

    explore(b, 0);
    
}

void explore(board *b, uint8_t index) {
    if (index >= SIDE_LENGTH) {
        printf("%s", "Nice, found a solution!");
        printboard(b);
    }
    for (int i = 0; i < SIDE_LENGTH; i++) {
        for (int j = 0; j < SIDE_LENGTH - i; j++) {
            if(isSafe(PIECES[index], b, i, j)) {
                place(PIECES[index], index, b, i, j);
                printboard(b);
                //explore(index + 1, b)
                unplace(PIECES[index], b, i, j);
            }
        }
    }
}

void place(const int piece[4][4], int pindex, board *b, int row, int col) {
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            if (piece[i][j] != 0) {
                b->state[row+i][col+j] = PIECE_IDS[pindex];
            }
        }
    }
}

int isSafe(const int piece[4][4], board *b, int row, int col) {
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            if (piece[i][j] != 0)  {
                if (row+i >= SIDE_LENGTH || col+j >= SIDE_LENGTH ||
                    b->state[row+i][col+j] != 0) {
                    
                    return 0;
                }
            }
        }
    }
    return 1;
}

void unplace(const int piece[4][4], board *b, int row, int col) {
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            if (piece[i][j] != 0) {
                b->state[row+i][col+j] = 0;
            }
        }
    }
}

void printboard(board *b) {
    for (int i = 0; i < SIDE_LENGTH; i++) {
        for (int j = 0; j < SIDE_LENGTH; j++) {
            if (b->state[i][j] == 1) {
                printf("%s ", "X");
            }
            else if (b->state[i][j] == 0) {
                printf("%s ", "O");
            }
            else {
                printf("\033[1;31m");
                printf("%c ", b->state[i][j]);
                printf("\033[0m");
            }
        }
        printf("%s", "\n");
    }
    printf("%s", "\n");
}

