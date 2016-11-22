#ifndef PACK_H_
#define PACK_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IMWD 64
#define IMHT 64

struct Grid {
    unsigned char board[IMHT][IMWD];
};

struct byteGrid {
    unsigned long board[IMHT][IMWD/32];
};

struct byteGrid addlinetogrid(struct byteGrid grid, unsigned char line[], int lineNum);
struct Grid setzerogrid(struct Grid grid);
struct byteGrid zeroByteGrid(struct byteGrid grid);
struct Grid worker(struct Grid grid);
void BackToPixels(unsigned char i[]);

#endif //PACK_H_
