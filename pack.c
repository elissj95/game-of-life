#include "pack.h"
#include <math.h>
#include <stdio.h>

//Takes grid, and section, and returns that section of the grid with the two shadow rows above and below
struct subGrid divideGrid(struct byteGrid grid, int section) {
    struct subGrid subgrid;
    //Where the sugrid will start
    int base =  (((section - 1)*(IMHT/4))-1)%IMHT;

    for(int x = 0; x < (IMHT/4)+2; x++){
        for(int y = 0; y < IMWD/8; y++) {
            subgrid.board[x][y] = grid.board[(x+base)%IMHT][y];
        }
    }
    return subgrid;
}

//Takes four subgrids and returns one byteGrid
struct byteGrid undivideGrid(struct subGrid grid1, struct subGrid grid2, struct subGrid grid3, struct subGrid grid4){
    struct byteGrid grid;

    //Add top subgrid to grid
    for(int x = 0; x < IMHT/4; x++) {
        for(int y = 0; y < IMWD/8; y++) {
            grid.board[x][y] = grid1.board[x+1][y];
        }
    }
    //Add second subgrid to grid etc.
    for(int x = IMHT/4; x < IMHT/2; x++) {
        for(int y = 0; y < IMWD/8; y++) {
            grid.board[x][y] = grid2.board[x-IMHT/4+1][y];
        }
    }
    for(int x = IMHT/2; x < 3*IMHT/4; x++) {
        for(int y = 0; y < IMWD/8; y++) {
            grid.board[x][y] = grid3.board[x-IMHT/2+1][y];
        }
    }
    for(int x = 3*IMHT/4; x < IMHT; x++) {
        for(int y = 0; y < IMWD/8; y++) {
            grid.board[x][y] = grid4.board[x-(3*IMHT/4)+1][y];
        }
    }
return grid;
}

// Takes in the Grid, line and linenum and encodes that line of the Grid to be the binary line given in.
struct byteGrid addlinetogrid(struct byteGrid grid, unsigned char line[], int lineNum){
    int counter = 0;

    for(int i = 0; i<IMWD/8; i++) { //zero line before writing to it
        grid.board[lineNum][i] = 0;
    }

    for (int j = 0; j<IMWD ; j++){
        if(j%8 == 0 && j!=0){
            counter++;
        }
        if(line[j] == 0){
            grid.board[lineNum][counter] = grid.board[lineNum][counter] + pow(2,((7)-j%8));
        }
    }
    return grid;
}

//Takes a pixel and determines how many of its surrounding pixels are alive
uchar GridToNine(uchar above, uchar middle, uchar below, uchar edgeleft, uchar edgeright){
    uchar test = pow(2, 7);
    int counter = 0;
    int val = 0;

    for(int i = 7;x=0;x--){


    }


    return 0;
}


