#ifndef PACK_H_
#define PACK_H_

#include <platform.h>
#include <xs1.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IMWD 64
#define IMHT 64

typedef unsigned char uchar;

struct subGrid {
    uchar board[(IMHT/4)+2][IMWD/8];
};

struct byteGrid {
    uchar board[IMHT][IMWD/8];
};

struct subGrid divideGrid(struct byteGrid grid, int section);
struct byteGrid undivideGrid(struct subGrid grid1, struct subGrid grid2, struct subGrid grid3, struct subGrid grid4);
struct byteGrid addlinetogrid(struct byteGrid grid, unsigned char line[], int lineNum);
uchar GridToNine(uchar above, uchar middle, uchar below, uchar edgeLeft, uchar edgeRight);
/*struct subGrid worker(struct subGrid grid, chanend sendup, chanend senddown, chanend recieveabove, chanend recievebelow);*/


#endif //PACK_H_
