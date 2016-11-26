#include "pack.h"
#include <math.h>
#include <stdio.h>

// Takes in the Grid, line and linenum and encodes that line of the Grid to be the binary line given in.
struct byteGrid addlinetogrid(struct byteGrid grid, unsigned char line[], int lineNum){
    int counter = 0;

    for(int i = 0; i<IMWD/32; i++) { //zero line before writing to it
        grid.board[lineNum][i] = 0;
    }

    for (int j = 0; j<IMWD ; j++){
        if(j%32 == 0 && j!=0){
            counter++;
        }
        if(line[j] == 0){
            grid.board[lineNum][counter] = grid.board[lineNum][counter] + pow(2,((31)-j%32));
        }
    }
    return grid;
}

//Takes in a Grid and sets all the values to 0, all 64x64 entries. Then returns the Grid.
struct byteGrid setzerogrid(struct byteGrid grid){
    struct byteGrid testGrid = grid;
    for(int y = 0 ; y<IMHT ; y++){
        for(int x = 0 ; x<IMWD ; x++){
            testGrid.board[x][y] = 0;
        }
    }
    return testGrid;
}

//Takes a pixel and determines how many of its surrounding pixels are alive
int GridToNine(struct byteGrid grid, int ypos, int xpos, int i, int isdead){
    //Take 3 numbers, start from above and continuing to below
    unsigned long threerow[3];
    unsigned long test = 2147483648;
    int counter = 0;
    int val = 0;
    //Overall counter for number of 1's
    //Initialise the top row, middle row and bottom row
    for(int y = ypos-1;y<ypos+2;y++){
        threerow[val] = grid.board[(y+IMHT)%IMHT][xpos];
        //Check the MSB for a 0 or a 1
        val++;
    }

    if(i==31){
        for(int y = ypos-1; y<ypos+2 ; y++){
            if((grid.board[(y+IMHT)%IMHT][(xpos+1)%(IMWD/32)] & 1) == 1){
                counter++;
            }
        }
        for(int x = 0 ; x<3 ; x++){
            for(int t = i-1;t<i+1;t++){
                unsigned long testval = pow(2, t);
                if(((threerow[x] & testval) == testval) && !((x==1) && (t==i))){
                    counter++;
                }
            }
        }
    }
    if(i==0){
        for(int y = ypos-1; y<ypos+2 ; y++){
            if((grid.board[(y+IMHT)%IMHT][(xpos+1)%(IMWD/32)] & test) == test){
                counter++;
            }
        }
        for(int x = 0 ; x<3 ; x++){
            for(int t = i+1;t>i-1;t--){
                unsigned long testval = pow(2, t);
                if(((threerow[x] & testval) == testval) && !((x==1) && (t==i))){
                    counter++;
                }
            }
        }
    }
    //Check the middle of the number for 1's
    else if((i!=0) && (i!=31)){
        for(int x = 0 ; x<3 ; x++){
            for(int t = i-1;t<i+2;t++){
                unsigned long testval = pow(2, t);
                if(((threerow[x] & testval) == testval) && !((x==1) && (t==i))){
                    counter++;
                }
            }
        }
    }
    //Alive counter
    if((isdead == 1) && ((counter == 0) || (counter > 3))){
        return 1;
    }
    if((isdead == 0) && (counter == 3)){
        return 1;
    }
    return 0;
}

//Takes a grid and returns its evolved state
struct byteGrid worker(struct byteGrid grid){
    struct byteGrid test;
    test = grid;
    //Create a new Grid to operate on
    //Iterate through the Grid passed in
    for(int y = 0; y<IMHT ; y++){
        for(int x = 0; x<2 ; x++){
            unsigned long val = grid.board[y][x];
            //Iterate through each number
            for(int i = 31; i>-1 ;i--){
                unsigned long powerTwo = pow(2,i);
                //Check for a 1
                if((powerTwo & val) == powerTwo){
                    //Alive (1)
                    //Returns true or false to update the cell
                    if(GridToNine(grid, y, x, i, 1) == 1){
                        test.board[y][x] = test.board[y][x] - pow(2,i);
                        // printf("ypos %d xpos %d i %d not alive\n", y, x, i);
                    }
                }
                //Check for a 0
                else if((powerTwo & ~val) == powerTwo){
                    //Dead (0)
                    //Returns true or false to update the cell
                    if(GridToNine(grid, y, x, i, 0) == 1){
                        test.board[y][x] = test.board[y][x] + pow(2,i);
                    }
                }
            }
        }
    }
    return test;
}
