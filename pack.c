#include "pack.h"
#include <math.h>
#include <stdio.h>

//Takes grid, and section, and returns that section of the grid with the two shadow rows above and below
struct subGrid divideGrid(struct byteGrid grid, int section) {
    struct subGrid subgrid;

    for(int x = 0; x < (IMHT/4)+2; x++) {
        for(int y = 0; y < IMWD/8; y++) {
            subgrid.board[x][y] = 0;
        }
    }

    //Where the sugrid will start
    int base = (((section * (IMHT/4))-1)+IMHT)%IMHT;

    for(int x = 0; x < (IMHT/4)+2; x++) {
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
            grid.board[x][y] = grid2.board[x-((IMHT/4)-1)][y];
        }
    }
    for(int x = IMHT/2; x < 3*IMHT/4; x++) {
        for(int y = 0; y < IMWD/8; y++) {
            grid.board[x][y] = grid3.board[x-(2*(IMHT/4)-1)][y];
        }
    }
    for(int x = 3*IMHT/4; x < IMHT; x++) {
        for(int y = 0; y < IMWD/8; y++) {
            grid.board[x][y] = grid4.board[x-(3*(IMHT/4)-1)][y];
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
        if(line[j] == 255){
            grid.board[lineNum][counter] = grid.board[lineNum][counter] + pow(2,((7)-j%8));
        }
    }
    return grid;
}

//Takes a pixel and determines how many of its surrounding pixels are alive
uchar GridToNine(uchar above, uchar middle, uchar below, uchar edgeLeft, uchar edgeRight){
    uchar top[2], mid[3], bot[2];
    uchar ntop, nbot;
    top[0] = 0; top[1] = 0; ntop = 0;
    bot[0] = 0; bot[1] = 0; nbot = 0;
    mid[0] = 0; mid[1] = 0; mid[2] = 0;
    uchar counter = 0;
    uchar state = 0;
    for(int i = 7;i > -1;i--){
        if(i==7){
           if((above>>6)%2){
               top[1] = 1;
           }
           if((above>>7)%2){
               top[0] = 1;
           }
           if((below>>6)%2){
               bot[1] = 1;
           }
           if((below>>7)%2){
               bot[0] = 1;
           }
           if((middle>>7)%2){
               state = 1;
               mid[0] = 1;
           }
           if((middle>>6)%2){
               mid[1] = 1;
           }

           counter = edgeLeft + top[0] + top[1] + bot[0] + bot[1] + mid[1];
        }
        if(i!=7 && i!=0){
            if((above>>(i-1))%2){
                ntop = 1;
            }
            if((below>>(i-1))%2){
                nbot = 1;
            }
            if((middle>>(i-1))%2){
                mid[2] = 1;
            }
            counter = top[0] + top[1] + ntop + bot[0] + bot[1] + nbot + mid[0] + mid[2];
            top[0] = top[1];
            top[1] = ntop;
            ntop = 0;
            bot[0] = bot[1];
            bot[1] = nbot;
            nbot = 0;
            if(mid[1] == 1){
                state = 1;
            }
            mid[0] = mid[1];
            mid[1] = mid[2];
            mid[2] = 0;
        }
        if(i==0){
           counter = edgeRight + top[0] + top[1] + bot[0] + bot[1] + mid[0];
           if(mid[1] == 1){
               state = 1;
           }
        }
       // printf("Pos %d, Back %d, middle %d, front %d\n", i, mid[0], mid[1], mid[2]);
      //  printf("Position :%d State %d, %d\n", i, state, counter);
        if((state==1) && ((counter == 1) || (counter > 3) || (counter == 0))){
            middle = middle - pow(2, i);
           // printf("Die\n");
        }
        if((state == 0) && counter == 3){
            middle = middle + pow(2, i);
           //printf("Come Alive\n");
        }
        counter = 0;
        state   = 0;

    }
    return middle;
}


