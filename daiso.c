#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "daiso.h"

#define PRINT_PROGRESS 1

const char PIECE_IDS[12] = {
    'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L'
};

const char COLORS[12][10] = {
    "\033[0;31m", "\033[0;32m",
    "\033[0;33m", "\033[0;34m",
    "\033[0;35m", "\033[0;36m",
    "\033[0;37m", "\033[1;31m",
    "\033[1;32m", "\033[1:33m",
    "\033[1;34m", "\033[1;35m"
};

uint8_t PIECES[12][4][4] = {
    {{1,1,0,0},
     {1,1,0,0},
     {1,0,0,0},
     {0,0,0,0}},

    {{1,1,0,0},
     {1,1,0,0},
     {0,0,0,0},
     {0,0,0,0}},

    {{1,1,0,0},
     {1,0,0,0},
     {1,0,0,0},
     {1,0,0,0}},

    {{1,0,0,0},
     {1,0,0,0},
     {1,1,0,0},
     {0,0,0,0}},

    {{0,0,0,0},
     {0,1,1,0},
     {1,1,0,0},
     {1,0,0,0}},

    {{0,1,0,0},
     {1,1,1,0},
     {0,1,0,0},
     {0,0,0,0}},

    {{1,1,0,0},
     {1,0,0,0},
     {0,0,0,0},
     {0,0,0,0}},

    {{0,1,0,0},
     {0,1,0,0},
     {1,1,0,0},
     {1,0,0,0}},

    {{1,0,0,0},
     {1,0,0,0},
     {1,1,0,0},
     {1,0,0,0}},

    {{1,1,1,0},
     {1,0,0,0},
     {1,0,0,0},
     {0,0,0,0}},

    {{1,1,0,0},
     {1,0,0,0},
     {1,1,0,0},
     {0,0,0,0}},

    {{1,0,0,0},
     {1,0,0,0},
     {1,0,0,0},
     {1,0,0,0}}

};

int main() {

    clock_t start = clock();
    board *b = malloc(sizeof(board));
    solution_set *solutions = malloc(sizeof(solution_set));
    solutions->count = 0;
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

    explore(b, 0, solutions);
    free(b);
    free(solutions);
    clock_t stop = clock();
    double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("Time elapsed in s: %f\n", elapsed / 1000);
}

void explore(board *b, uint8_t index, solution_set *s) {
    if (index == NUM_PIECES) {
        // All pieces have been successfully placed on the board
        // This solution hasn't been seen before, print the board and save
        uint32_t hash = hashBoard(b);
        if (!solutionExists(s, hash)) {
            s->solutions[s->count] = hashBoard(b);
            s->count++;
            printf("Nice, found a solution! Total solutions: %d\n", s->count);
            printBoard(b);
        }
        return;
    }

    // Set timeouts for nanosleep
    struct timespec req = {};
    struct timespec rem = {};
    req.tv_sec = 0;
    req.tv_nsec = 550000000;

    // Try all possible positions for each piece, in each orientation
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < SIDE_LENGTH; i++) {
            for (int j = 0; j < SIDE_LENGTH - i; j++) {
                if (isSafe(PIECES[index], b, i, j)) {
                    place(PIECES[index], index, b, i, j);

                    if (PRINT_PROGRESS) {
                        printBoard(b);
                        nanosleep(&req, &rem);
                    }

                    if (isPossible(b) != 0) {
                        explore(b, index + 1, s);
                    }
                    unplace(PIECES[index], b, i, j);
                }
            }
        }
        rotatePiece(PIECES[index], PIECE_SIZE);
    }
}

// Places the given piece on the given board, where row and col represent
// the upper left corner of the piece
void place(uint8_t piece[4][4], uint8_t pindex, board *b, uint8_t row, uint8_t col) {
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            if (piece[i][j] != 0) {
                b->state[row+i][col+j] = PIECE_IDS[pindex];
            }
        }
    }
}

// Unlaces the given piece on the given board, where row and represent the 
// upper left corner of the piece
void unplace(uint8_t piece[4][4], board *b, uint8_t row, uint8_t col) {
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            if (piece[i][j] != 0) {
                b->state[row+i][col+j] = 0;
            }
        }
    }
}

// Returns 1 if the piece can be placed at the given (row, col) without
// touching another piece or illegal cell
int isSafe(uint8_t piece[4][4], board *b, uint8_t row, uint8_t col) {
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


// Prints the current board state using the colors and symbols
// defined in COLORS and PIECE_IDS
void printBoard(board *b) {
    for (int i = 0; i < SIDE_LENGTH; i++) {
        for (int j = 0; j < SIDE_LENGTH; j++) {
            if (b->state[i][j] == 1) {
                printf("%s ", "X");
            }
            else if (b->state[i][j] == 0) {
                printf("%s ", "O");
            }
            else {
                char id = b->state[i][j];
                uint8_t index = getIndexFromId(id);
                printf("%s", COLORS[index]);
                printf("%c ", b->state[i][j]);
                printf("\033[0m");
            }
        }
        printf("%s", "\n");
    }
    printf("%s", "\n");
}

// Given a char corresponding to a piece id in PIECE_IDS,
// returns the index assigned to that char.
int getIndexFromId(char c) {
    for (int i = 0; i < NUM_PIECES; i++) {
        if (PIECE_IDS[i] == c) {
            return i;
        }
    }
    return -1;
}

// (TODO: typdef solution hash)
// Given a solution_set and a solution hash, returns 1 if the 
// solution has been seen before, and 0 otherwise.
int solutionExists(solution_set *s, uint32_t sol) {
    for(int i = 0; i < s->count; i++) {
        if (sol == s->solutions[i]) {
            return 1;
        }
    }
    return 0;
}

// Given a board, returns a 32-bit hash encapsulating its state.
uint32_t hashBoard(board *b) {
    uint32_t hash = 0;
    for (int i = 0; i < SIDE_LENGTH; i++) {
        for (int j = 0; j < SIDE_LENGTH; j++) {
            hash = 31 * hash + b->state[i][j];
        }
    }
    return hash;
}

// Returns 1 if a solution is possible given the current board state,
// 0 otherwise
int isPossible(board *b) {
    int whitespace = 0;
    for (int i = 0; i < SIDE_LENGTH; i++) {
        for (int j = 0; j < SIDE_LENGTH; j++) {

            // Look for empty cells that are touching 0 other empty cells
            // It is impossible to find a solution from this state, return 0
            if (b->state[i][j] == 0 && numEmptyNeighbors(b, i, j) == 0) {
                return 0;
            }
        }
    }
    return 1;

}

// Returns the number of empty neighboring cells (N,W,E,S)
int numEmptyNeighbors(board *b, int i, int j) {
    int whitespace = 0;

    if ((i - 1) >= 0 && b->state[i - 1][j] == 0) {
        whitespace++;
    }
    if ((i + 1) < SIDE_LENGTH && b->state[i + 1][j] == 0) {
        whitespace++;
    }
    if ((j - 1) >= 0 && b->state[i][j - 1] == 0) {
        whitespace++;
    }
    if ((j + 1) < SIDE_LENGTH && b->state[i][j + 1] == 0) {
        whitespace++;
    }
    return whitespace;
}

void rotatePiece(uint8_t mat[4][4], uint8_t N) {
    for (int x = 0; x < N / 2; x++) {
        for (int y = x; y < N-x-1; y++) {
            
            uint8_t temp = mat[x][y];
 
            // move values from right to top
            mat[x][y] = mat[y][N-1-x];
 
            // move values from bottom to right
            mat[y][N-1-x] = mat[N-1-x][N-1-y];
 
            // move values from left to bottom
            mat[N-1-x][N-1-y] = mat[N-1-y][x];
 
            // assign temp to left
            mat[N-1-y][x] = temp;
        }
    }
}

