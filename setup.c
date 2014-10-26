#// ***************************************************************************************************************************
#// *                                                                                                                         *
#// *                        THIS FILE PERFORMS BOARD SETUP AND INITALIZATION                                                *
#// *                                                                                                                         *
#// ***************************************************************************************************************************

#//*********INFORMATION**********************************************************************************************
#//
#// PORT B 10-13 for robot movement control
#// BIT 10 is RIGHT side; 11 is LEFT side
#//
#//
#//******************************************************************************************************************

#include "header.h"

void setupPORTs()
{
    PORTSetPinsDigitalIn(IOPORT_A, BIT_6 | BIT_7); //setup buttons as inputs
    PORTSetPinsDigitalOut(IOPORT_B, BIT_10 | BIT_11 | BIT_12 | BIT_13); //PORTB 10-13 as robot controls
    PORTSetPinsDigitalOut(IOPORT_F, BIT_8|BIT_5); //UART TX pins
    PORTSetPinsDigitalIn(IOPORT_F, BIT_2|BIT_4|BIT_12); //UART RX pins and GPS status pin
}

//****************************************************************************************************************

void reset(void)
{
    PORTWrite(IOPORT_B, 0);
}

//****************************************************************************************************************

void setup_UART1()
{
    U1MODE = 0;                         //*************888888*****************
    U1TXREG = 0;                        //*                                  *
    U1RXREG = 0;                        //* UART setup(mode, baud rate, enable
    U1BRG = 64;                         //*
    U1STA = 5<<10;                      //*                                  *
    U1MODE = 0b1000000000000000;        //************************************

    U1TXREG = 27; //escape sequence; precedes any command
    U1TXREG = ']'; //] and j are clear and home command
    U1TXREG = 'j';

}

//****************************************************************************************************************

void setup_UART2()
{
    U2MODE = 0;                         //*************888888*****************
    U2TXREG = 0;                        //*                                  *
    U2RXREG = 0;                        //* UART setup(mode, baud rate, enable
    U2BRG = 64;                         //*
    U2STA = 5<<10;  //both Rx and Tx    //*                                  *
    U2MODE = 0b1000000000010000;        //************************************

}

//****************************************************************************************************************

