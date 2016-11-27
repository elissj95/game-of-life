// COMS20001 - Cellular Automaton Farm
// (using the XMOS i2c accelerometer demo code)
#include <platform.h>
#include <xs1.h>
#include <stdio.h>
#include <math.h>
#include "pgmIO.h"
//#include "i2c.h"
#include "pack.h"

#define  IMHT 64                 //image height
#define  IMWD 64                  //image width

typedef unsigned char uchar;      //using uchar as shorthand

on tile[0]: port p_scl = XS1_PORT_1E;         //interface ports to orientation
on tile[0]: port p_sda = XS1_PORT_1F;
on tile[0]: in port buttons = XS1_PORT_4E; //port to access buttons
on tile[0]: out port leds = XS1_PORT_4F;   //port to access LEDs

#define FXOS8700EQ_I2C_ADDR 0x1E  //register addresses for orientation
#define FXOS8700EQ_XYZ_DATA_CFG_REG 0x0E
#define FXOS8700EQ_CTRL_REG_1 0x2A
#define FXOS8700EQ_DR_STATUS 0x0
#define FXOS8700EQ_OUT_X_MSB 0x1
#define FXOS8700EQ_OUT_X_LSB 0x2
#define FXOS8700EQ_OUT_Y_MSB 0x3
#define FXOS8700EQ_OUT_Y_LSB 0x4
#define FXOS8700EQ_OUT_Z_MSB 0x5
#define FXOS8700EQ_OUT_Z_LSB 0x6

//Display LED patterns
int showLEDs(out port p, chanend fromDistributor) {
  int pattern; //1st bit...separate green LED
               //2nd bit...blue LED
               //3rd bit...green LED
               //4th bit...red LED
  while (1) {
    fromDistributor :> pattern;   //receive new pattern from visualiser
    p <: pattern;                //send pattern to LED port
  }
  return 0;
}

//Read buttons and send to the distributor
void buttonListener(in port b, chanend toDistributor) {
    int r;

    while (1) {
        b when pinseq(15)  :> r;    // check that no button is pressed
        b when pinsneq(15) :> r;    // check if some buttons are pressed
        if ((r==13) || (r==14)) {    // if either button is pressed
           toDistributor <: r;             // send button pattern to distributor
        }
    }
}


// Read Image from PGM file from path infname[] to channel c_out
void DataInStream(chanend c_out) {
    int res;
    uchar line[ IMWD ];
    struct byteGrid grid;
    char infname[] = "64x64.pgm";

    printf( "DataInStream: Start...\n" );

    //Open PGM file
    res = _openinpgm( infname, IMWD, IMHT );
    if( res ) {
        printf( "DataInStream: Error opening %s\n.", infname );
        return;
    }

    //Read image line-by-line and send byte by byte to channel c_out
    for( int y = 0; y < IMHT; y++ ) {
        _readinline( line, IMWD );
        grid = addlinetogrid(grid, line, y);
    }

    //Send each element of grid to distributor
    for( int y = 0; y< IMHT;y++){
        for(int x = 0; x<(IMWD/32) ;x++){
            c_out <: grid.board[y][x];
        }
    }
    //Close PGM image file
    _closeinpgm();
    printf( "DataInStream: Done...\n" );
    return;
}

// Distributes work to worker threads
void distributor(chanend c_in, chanend c_out, chanend fromAcc, chanend fromButtons, chanend toLEDs){

    struct byteGrid grid;
    int buttonInput;
    int stopEvolving = 0;

    //Starting up and wait for tilting of the xCore-200 Explorer
    printf( "ProcessImage: Start, size = %dx%d\n", IMHT, IMWD );
    printf( "Processing...\n" );

    //Populate grid with values from DataIn
    for( int y = 0; y < IMHT; y++ ) {   //go through all lines
        for( int x = 0; x < (IMWD/32); x++ ) { //go through each pixel per line
            c_in :> grid.board[y][x];   //read the pixel value
        }
    }

    while (!stopEvolving) {
        select {
            case fromButtons :> buttonInput: //expect values 13 and 14
                //Run another round
                if(buttonInput == 14) {
                    toLEDs <: 1;
                    //Evolve grid
                    grid = worker(grid);
                    toLEDs <: 0;
                    printf( "\nOne processing round completed...\n" );
                }
                //End processing
                if(buttonInput == 13) {
                    toLEDs <: 2;
                    for(int x = 0; x < IMHT; x++) {
                        for(int y = 0; y < IMWD/32; y++) {
                            c_out <: grid.board[x][y];
                        }
                    }
                    toLEDs <: 0;
                    stopEvolving = 1;
                }
                break;
        }
    }
}

// Write pixel stream from channel c_in to PGM image file
void DataOutStream(chanend c_in){

    int offset;
    int res;
    uchar line[ IMWD ];
    unsigned long intLine[IMWD/32];
    char outfname[] = "testout.pgm";

    //Open PGM file
    printf( "DataOutStream: Start...\n" );
    res = _openoutpgm( outfname, IMWD, IMHT );
    if( res ) {
        printf( "DataOutStream: Error opening %s\n.", outfname );
        return;
    }

    //Compile each line of the image and write the image line-by-line
    for( int y = 0; y < IMHT; y++ ) {
        for( int x = 0; x < IMWD/32; x++ ) {
            c_in :> intLine[x];
        }
        offset = 0;
        for(int a = 0; a < IMWD/32; a++) {
            int p = 32-1;
            for(int z = offset; z < 32+offset; z++){
                unsigned long powerTwo = pow(2,p);
                if((powerTwo & intLine[a]) == powerTwo) { line[z] = 0;}
                else { line[z] = 255;}
                p--;
            }
            offset+=32;
        }
        _writeoutline( line, IMWD );
        //printf( "DataOutStream: Line written...\n" );
    }
    //Close the PGM image
    _closeoutpgm();
    printf( "DataOutStream: Done...\n" );
    return;
}

// Initialise and  read orientation, send first tilt event to channel
/*void orientation( client interface i2c_master_if i2c, chanend toDist) {
    i2c_regop_res_t result;
    char status_data = 0;
    int tilted = 0;
    // Configure FXOS8700EQ
    result = i2c.write_reg(FXOS8700EQ_I2C_ADDR, FXOS8700EQ_XYZ_DATA_CFG_REG, 0x01);
    if (result != I2C_REGOP_SUCCESS) {
        printf("I2C write reg failed\n");
    }
    // Enable FXOS8700EQ
    result = i2c.write_reg(FXOS8700EQ_I2C_ADDR, FXOS8700EQ_CTRL_REG_1, 0x01);
    if (result != I2C_REGOP_SUCCESS) {
        printf("I2C write reg failed\n");
    }
    //Probe the orientation x-axis forever
    while (1) {
        //check until new orientation data is available
        do {
            status_data = i2c.read_reg(FXOS8700EQ_I2C_ADDR, FXOS8700EQ_DR_STATUS, result);
        } while (!status_data & 0x08);
        //get new x-axis tilt value
        int x = read_acceleration(i2c, FXOS8700EQ_OUT_X_MSB);
        //send signal to distributor after first tilt
        if (!tilted) {
            if (x>30) {
                tilted = 1 - tilted;
                toDist <: 1;
            }
        }
    }
}*/


// Orchestrate concurrent system and start up all threads
int main(void) {

//i2c_master_if i2c[1];               //interface to orientation
//char infname[] = "64x64.pgm";     //put your input image path here
//char outfname[] = "testout.pgm"; //put your output image path here

chan c_inIO, c_outIO, c_control, buttonsToDistributor, distributorToLEDs;    //extend your channel definitions here

par {
  //  i2c_master(i2c, 1, p_scl, p_sda, 10);   //server thread providing orientation data
  //  orientation(i2c[0],c_control);        //client thread reading orientation data
    on tile[1]: DataInStream(c_inIO);          //thread to read in a PGM image
    on tile[1]: DataOutStream(c_outIO);       //thread to write out a PGM image
    on tile[1]: distributor(c_inIO, c_outIO, c_control, buttonsToDistributor, distributorToLEDs);//thread to coordinate work on image

    on tile[0]: buttonListener(buttons, buttonsToDistributor);
    on tile[0]: showLEDs(leds, distributorToLEDs);
    }

  return 0;
}
