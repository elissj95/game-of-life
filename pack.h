#ifndef PACK_H_
#define PACK_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IMWD 64
#define IMHT 64

struct byteGrid {
    unsigned long board[IMHT][IMWD/32];
};

struct byteGrid addlinetogrid(struct byteGrid grid, unsigned char line[], int lineNum);
struct byteGrid setzerogrid(struct byteGrid grid);
struct byteGrid zeroByteGrid(struct byteGrid grid);
struct byteGrid worker(struct byteGrid grid);

#endif //PACK_H_
