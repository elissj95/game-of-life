#ifndef PACK_H_
#define PACK_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IMWD 256
#define IMHT 256

struct subGrid {
    unsigned long board[(IMHT/4)+2][IMWD/32];
};

struct byteGrid {
    unsigned long board[IMHT][IMWD/32];
};

struct subGrid divideGrid(struct byteGrid grid, int section);
struct byteGrid undivideGrid(struct subGrid grid1, struct subGrid grid2, struct subGrid grid3, struct subGrid grid4);
struct byteGrid addlinetogrid(struct byteGrid grid, unsigned char line[], int lineNum);
struct subGrid worker(struct subGrid grid);


#endif //PACK_H_
