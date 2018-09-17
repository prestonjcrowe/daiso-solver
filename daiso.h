#ifndef DAISO_H
#define DAISO_H

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

void printboard(board *b);
void explore(board *b, uint8_t index, solution_set *s);
void place(uint8_t piece[4][4], uint8_t pindex, board *b,
    uint8_t row, uint8_t col);
void unplace(uint8_t piece[4][4], board *b, uint8_t row, uint8_t col);
void rotatePiece(uint8_t mat[4][4], uint8_t N);
int isSafe(uint8_t piece[4][4], board *b, uint8_t row, uint8_t col);
int getindexfromid(char c);
int solutionexists(solution_set *s, uint32_t sol);
uint32_t hashboard(board *b);

#endif
