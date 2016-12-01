#ifndef PACK_H_
#define PACK_H_

#include <platform.h>
#include <xs1.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IMWD 16
#define IMHT 16

typedef unsigned char uchar;

struct subGrid {
    uchar board[(IMHT/4)+2][IMWD/8];
};

uchar leftEdgeCounter(uchar top, uchar middle, uchar bottom);
uchar rightEdgeCounter(uchar top, uchar middle, uchar bottom);
uchar GridToNine(uchar above, uchar middle, uchar below, uchar edgeLeft, uchar edgeRight );
void ucharRow(uchar line[], uchar row[]);

#endif //PACK_H_
