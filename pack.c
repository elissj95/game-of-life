#include "pack.h"
#include <math.h>
#include <stdio.h>

//Converts an array of unsigned longs back to array of pixels
void toPixels(unsigned char i[], int j[]) {
    int offset = IMWD;

    for(int y = IMWD/32 -1; y >= 0; y--) {
        for(int x = offset; x >= offset - 32; x--) {
            if(i[y] % 2) { j[x] = 1; }
            else { j[x] = 0; }
            i[y] >>= 1;
        }
        offset = offset - 32;
    }
}

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
        if(line[j] != 0){
            grid.board[lineNum][counter] = grid.board[lineNum][counter] + pow(2,((31)-j%32));
        }
    }
    return grid;
}

//Takes in a Grid and sets all the values to 0, all 64x64 entries. Then returns the Grid.
struct Grid setzerogrid(struct Grid grid){
    struct Grid testGrid = grid;
    for(int y = 0 ; y<IMHT ; y++){
        for(int x = 0 ; x<IMWD ; x++){
            testGrid.board[x][y] = 0;
        }
    }
    return testGrid;
}

//Takes a pixel and determines how many of its surrounding pixels are alive
int GridToNine(struct Grid grid, int ypos, int xpos, int i, int isdead){
    //Take 3 numbers, start from above and continuing to below
    unsigned long threerow[3];
    //Check for reduce bit length and set all to zero
    int conditionthree[3];
    conditionthree[0], conditionthree[1], conditionthree[2] = 0;
    unsigned long test = 2147483648;

    int val = 0;
    //Overall counter for number of 1's
    int counter = 0;
    //Initialise the top row, middle row and bottom row
    for(int y = ypos-1;y<ypos+2;y++){
        threerow[val] = grid.board[(y+IMHT)%IMHT][xpos];
        //Check the MSB for a 0 or a 1
        if(!((grid.board[(y+IMHT)%IMHT][xpos] & test) == test)){
            threerow[val] = threerow[val] + pow(2,31);
            conditionthree[val] = 1;
        }
        val++;
    }

    if(i==31){
        for(int y = ypos-1; y<ypos+2 ; y++){
            if((grid.board[(y+IMHT)%IMHT][(xpos+1)%(IMWD/32)] & 1) == 1){
                counter++;
            }
        }
        for(int x = 0 ; x<3 ; x++){
            unsigned long testval = pow(2, i);
            if(((threerow[x] & testval) == testval) && x!=1){
                counter++;
            }
        }

        for(int x = 0 ; x<3 ; x++){
             unsigned long testval = pow(2, i-1);
             if((threerow[x] & testval) == testval){
                counter++;
            }
        }
        //Check for reduced number
     if(conditionthree[0] == 1){
         counter--;
     }
     if(conditionthree[2] == 1){
         counter--;
     }
    }
    if(i==0){
            for(int y = ypos-1; y<ypos+2 ; y++){
                if((grid.board[(y+IMHT)%IMHT][(xpos+1)%(IMWD/32)] & test) == test){
                    counter++;
                }
            }
            for(int x = 0 ; x<3 ; x++){
                unsigned long testval = pow(2, i);
                if(((threerow[x] & testval) == testval) && x!=1){
                    counter++;
                }
            }

            for(int x = 0 ; x<3 ; x++){
                 unsigned long testval = pow(2, i+1);
                 if((threerow[x] & testval) == testval){
                    counter++;
                }
            }
            //Check for reduced number

        }
     //Check the middle of the number for 1's
     else if((i!=0) && (i!=31)){
          for(int x = 0 ; x<3 ; x++){
                 unsigned long testval = pow(2, i);
                      if(((threerow[x] & testval) == testval) && x!=1){
                             counter++;
                      }
          }

          for(int x = 0 ; x<3 ; x++){
                 unsigned long testval = pow(2, i+1);
                      if((threerow[x] & testval) == testval){
                              counter++;
                      }
          }
          for(int x = 0 ; x<3 ; x++){
                  unsigned long testval = pow(2, i-1);
                      if((threerow[x] & testval) == testval){
                              counter++;
                      }
          }
          //Check reduce value only on 30
          if(i==30){
              for(int x = 0; x<3 ; x++){
                  if(conditionthree[x] == 1){
                      counter--;
                  }
              }
          }
      }

    printf("counter : %d, ypos : %d, xpos :%d, i : %d\n", counter, ypos, xpos, i);
    if((isdead == 1) && ((counter>3) || (counter == 0))){
        return 1;
    }
    if((isdead == 0) && (counter == 3)){
        return 1;
    }

    return 0;

}

//Takes a grid and returns its evolved state
struct Grid worker(struct Grid grid){
    struct Grid test;
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
                        test.board[y][x] = val - powerTwo;
                    }
                }
                //Check for a 0
                else {
                    //Dead (0)
                    //Returns true or false to update the cell
                    if(GridToNine(grid, y, x, i, 0)){
                        test.board[y][x] = val + powerTwo;
                    }
                }
            }
        }
    }
    for(int y = 0; y<IMHT ; y++){
        for(int x = 0; x<2 ; x++){
            unsigned long val = test.board[y][x];
            for(int i = 31; i>-1 ;i--){
                unsigned long powerTwo = pow(2,i);
                if((powerTwo & val) == powerTwo){
                    //Alive (1)
                    printf("1");
                }
                else{
                    //Dead
                    printf("0");
                }
            }
            if(x==1){
                printf("\n");
            }
        }
    }
    return test;
}

//Takes in the current Grid, and just from the integer passed in 16 more rows of the Grid. Then returns that portion of the Grid
/*struct Grid worker(struct Grid Grid, int start){
    struct Grid grid;
    for(int y = 0; y<18 ;y++){
        for(int x = 0; x<IMWD ;x++){
            grid.board[y][x] = 1;
        }
    }
    //Take a grid which has both the top and bottum neighboors.
    for(int y = start-1; y<start+17 ; y++){
        for(int x = 0 ; x<(IMWD/32) ; x++){
                if(y == -1){
                    y = 0;
                }
                unsigned long val = Grid.board[y][x];
                    for(int i = 31; i>-1 ; i--){
                        if(((val - (pow(2,i)))) > 0){
                            val = val - val;
                        }
                        if(((val - (pow(2,i))) < 0) && y == 0 ){
                            grid.board[y][(i-31)*(-1)] = 0;
                        }
                        else if(((val - (pow(2,i))) < 0) && y == 1 ){
                            grid.board[y][i+1] = 0;
                        }
                    }
        }
    }
    for( int y = 0; y < 18; y++) {      //Go through rows
                        for( int x = 0; x < IMWD; x++ ) { //Go through columns
                            printf("%u", grid.board[y][x]);
                            if(x==63){
                                           printf("\n");
                                       }
                        }

    }
    return Grid;
}*/
