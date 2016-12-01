#include "pack.h"
#include <math.h>
#include <stdio.h>

// Takes in the Grid, line and linenum and encodes that line of the Grid to be the binary line given in.
void ucharRow(uchar line[], uchar row[]){
    int counter = 0;

    for(int i = 0; i<IMWD/8; i++) { //zero line before writing to it
        row[i] = 0;
    }

    for (int j = 0; j<IMWD ; j++){
        if(j%8 == 0 && j!=0){
            counter++;
        }
        if(line[j] == 255){
            row[counter] = row[counter] + pow(2,((7)-j%8));
        }
    }
}

uchar leftEdgeCounter(uchar top, uchar middle, uchar bottom){
    uchar returnval = top%2 + middle%2 + bottom%2;
    return returnval;
}

uchar rightEdgeCounter(uchar top, uchar middle, uchar bottom){
    uchar returnval = (top >> 7)%2 + (middle >> 7)%2 + (bottom >> 7)%2;
    return returnval;
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

        if((state==1) && ((counter == 1) || (counter > 3) || (counter == 0))){
            middle = middle - pow(2, i);
        }
        if((state == 0) && counter == 3){
            middle = middle + pow(2, i);
        }
        counter = 0;
        state   = 0;

    }
    return middle;
}


