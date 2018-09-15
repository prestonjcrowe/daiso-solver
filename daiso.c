#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const uint8_t SIDE_LENGTH = 10;
const uint8_t PIECE_SIZE = 4;
const char* RESET = "\033[0m;";
const uint8_t NUM_PIECES = 12;
const uint16_t MAX_SOLUTIONS = 1000;
const uint16_t SOLUTION_LENGTH = 55;

typedef struct board {
    uint8_t sidelength;
    char state[SIDE_LENGTH][SIDE_LENGTH];
} board;

typedef struct solution_set {
    uint16_t count;
    uint32_t solutions[MAX_SOLUTIONS];
} solution_set;

const char PIECE_IDS[12] = {'a', 'b', 'c', 'd', 'e', 'f',
                            'g', 'h', 'i', 'j', 'k', 'l'};

const char COLORS[12][10] = {"\033[1;31m", "\033[1;31m",
                            "\033[0;32m", "\033[1;32m",
                            "\033[0;33m", "\033[0;36m",
                            "\033[0;34m", "\033[1;34m",
                            "\033[0;35m", "\033[1;35m",
                            "\033[2;35m", "\033[2;33m"};

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
void place(int piece[4][4], int pindex, board *b, int row, int col);
void unplace(int piece[4][4], board *b, int row, int col);
int isSafe(int piece[4][4], board *b, int row, int col);
void explore(board *b, uint8_t index, solution_set *s);
int getindexfromid(char c);
void rotatePiece(int mat[4][4], int N);
char* stringifyboard(board *b);
int solutionexists(solution_set *s, uint32_t sol);
uint32_t hashboard(board *b);

int main() {
    // Initialize the board state. 0 represents a free
    // space, 1 represents a cell that is off limits

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
        uint32_t str = hashboard(b);

        // This is a new solution
        if (!solutionexists(s, str)) {
            s->solutions[s->count] = hashboard(b);
            s->count++;
            printf("Nice, found a solution! Total solutions: %d\n", s->count);
            printboard(b);
        }
        return;
    }
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < SIDE_LENGTH; i++) {
            for (int j = 0; j < SIDE_LENGTH - i; j++) {
                if(isSafe(PIECES[index], b, i, j)) {
                    place(PIECES[index], index, b, i, j);
                    explore(b, index + 1, s);
                    unplace(PIECES[index], b, i, j);
                }
            }
        }
        rotatePiece(PIECES[index], PIECE_SIZE);
    }
}

void place(int piece[4][4], int pindex, board *b, int row, int col) {
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            if (piece[i][j] != 0) {
                b->state[row+i][col+j] = PIECE_IDS[pindex];
            }
        }
    }
}

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

void unplace(int piece[4][4], board *b, int row, int col) {
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

int getindexfromid(char c) {
    for (int i = 0; i < NUM_PIECES; i++) {
        if (PIECE_IDS[i] == c) {
            return i;
        }
    }
    return -1;
}

char* stringifyboard(board *b) {
    char* res = malloc(sizeof(b->state));
    int index = 0;
    for (int i = 0; i < SIDE_LENGTH; i++) {
        for (int j = 0; j < SIDE_LENGTH; j++) {
            res[index] = b->state[i][j];
            index++;
        }
    }
    return res;

}

int solutionexists(solution_set *s, uint32_t sol) {
    for(int i = 0; i < s->count; i++) {
        if (sol == s->solutions[i]) {
            return 1;
        }
    }
    return 0;
}

uint32_t hashboard(board *b) {
    uint32_t res = 1;
    uint8_t pos = 10;
    for (int i = 0; i < SIDE_LENGTH; i++) {
        for (int j = 0; j < SIDE_LENGTH; j++) {
            pos++;
            res += (b->state[i][j] * pos  * 37);
        }
    }
    return res;

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
