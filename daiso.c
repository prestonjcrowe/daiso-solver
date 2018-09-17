/* TODO:
    type def BOARD_HASH
    header file
    int -> uintXX_t
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const uint8_t SIDE_LENGTH = 10;
const uint8_t PIECE_SIZE = 4;
const uint8_t NUM_PIECES = 12;
const uint16_t MAX_SOLUTIONS = 1000;

typedef struct board {
    uint8_t sidelength;
    char state[SIDE_LENGTH][SIDE_LENGTH];
} board;

typedef struct solution_set {
    uint16_t count;
    uint32_t solutions[MAX_SOLUTIONS];
} solution_set;

const char PIECE_IDS[12] = {
    'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l'
};

const char COLORS[12][10] = {
    "\033[1;31m", "\033[1;31m",
    "\033[0;32m", "\033[1;32m",
    "\033[0;33m", "\033[0;36m",
    "\033[0;34m", "\033[1;34m",
    "\033[0;35m", "\033[1;35m",
    "\033[2;35m", "\033[2;33m"
};

int PIECES[12][4][4] = {
    {{1,0,0,0},
     {1,0,0,0},
     {1,0,0,0},
     {1,0,0,0}},

    {{1,1,0,0},
     {1,1,0,0},
     {1,0,0,0},
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

    {{1,1,0,0},
     {1,0,0,0},
     {1,1,0,0},
     {0,0,0,0}}
};

void printboard(board *b);
void explore(board *b, uint8_t index, solution_set *s);
void place(int piece[4][4], int pindex, board *b, int row, int col);
void unplace(int piece[4][4], board *b, int row, int col);
void rotatePiece(int mat[4][4], int N);
int isSafe(int piece[4][4], board *b, int row, int col);
int getindexfromid(char c);
int solutionexists(solution_set *s, uint32_t sol);
uint32_t hashboard(board *b);

int main() {
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
}

void explore(board *b, uint8_t index, solution_set *s) {
    if (index == NUM_PIECES) {
        // All pieces have been successfully placed on the board
        // This solution hasn't been seen before, print the board and save
        uint32_t hash = hashboard(b);
        if (!solutionexists(s, hash)) {
            s->solutions[s->count] = hashboard(b);
            s->count++;
            printf("Nice, found a solution! Total solutions: %d\n", s->count);
            printboard(b);
        }
        return;
    }

    // Try all possible positions for each piece, in each orientation
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < SIDE_LENGTH; i++) {
            for (int j = 0; j < SIDE_LENGTH - i; j++) {
                if(isSafe(PIECES[index], b, i, j)) {
                    place(PIECES[index], index, b, i, j);
                    
                    //Uncomment to watch every move
                    //printboard(b);
                    //struct timespec *req = malloc(sizeof(struct timespec));
                    //struct timespec *rem = malloc(sizeof(struct timespec));
                    //req->tv_sec = 0;
                    //req->tv_nsec = 550000000;
                    //nanosleep(req, rem);
                    explore(b, index + 1, s);
                    unplace(PIECES[index], b, i, j);
                }
            }
        }
        rotatePiece(PIECES[index], PIECE_SIZE);
    }
}

// Places the given piece on the given board, where row and represent the 
// upper left corner of the piece
void place(int piece[4][4], int pindex, board *b, int row, int col) {
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            if (piece[i][j] != 0) {
                b->state[row+i][col+j] = PIECE_IDS[pindex];
            }
        }
    }
}

// Returns 1 if the piece can be placed at the given (row, col) without
// touching another piece or illegal cell
int isSafe(int piece[4][4], board *b, int row, int col) {
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

// Unlaces the given piece on the given board, where row and represent the 
// upper left corner of the piece
void unplace(int piece[4][4], board *b, int row, int col) {
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            if (piece[i][j] != 0) {
                b->state[row+i][col+j] = 0;
            }
        }
    }
}

// Prints the current board state using the colors and sumbols
// defined in COLORS and PIECE_IDS
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
                char id = b->state[i][j];
                int index = getindexfromid(id);
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
int getindexfromid(char c) {
    for (int i = 0; i < NUM_PIECES; i++) {
        if (PIECE_IDS[i] == c) {
            return i;
        }
    }
    return -1;
}

// (TODO: typeef solution hash)
// Given a solution_set and a solution hash, returns 1 if the 
// solution has been seen before, and 0 otherwise.
int solutionexists(solution_set *s, uint32_t sol) {
    for(int i = 0; i < s->count; i++) {
        if (sol == s->solutions[i]) {
            return 1;
        }
    }
    return 0;
}

// Given a board, returns a 32-bit hash encapsulating its state.
uint32_t hashboard(board *b) {
    uint32_t hash = 0;
    for (int i = 0; i < SIDE_LENGTH; i++) {
        for (int j = 0; j < SIDE_LENGTH; j++) {
            hash = 31 * hash + b->state[i][j];
        }
    }
    return hash;
}

// An Inplace function to rotate a N x N matrix
// by 90 degrees in anti-clockwise direction
void rotatePiece(int mat[4][4], int N)
{
    // Consider all squares one by one
    for (int x = 0; x < N / 2; x++)
    {
        // Consider elements in group of 4 in 
        // current square
        for (int y = x; y < N-x-1; y++)
        {
            // store current cell in temp variable
            int temp = mat[x][y];
 
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
