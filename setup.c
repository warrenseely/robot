#// ****************************************************************************************************************
#// *                                                                                                              *
#// *                        THIS FILE PERFORMS BOARD SETUP AND INITALIZATION                                      *
#// *                                                                                                              *
#// ****************************************************************************************************************

#//*********INFORMATION**********************************************************************************************
#//
#// 
#// 
#//
#//
#//******************************************************************************************************************

#include "header.h"

void setupPORTs()
{
    PORTSetPinsDigitalIn(IOPORT_A, BIT_6 | BIT_7 | BIT_9); //setup buttons as inputs, and bit 9 is bluetooth status
    PORTSetPinsDigitalOut(IOPORT_B, BIT_10 | BIT_11 | BIT_12 | BIT_13); //PORTB 10-13 as robot movement controls
    PORTSetPinsDigitalOut(IOPORT_E, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_7); //boom nozzel monitoring
    PORTSetPinsDigitalOut(IOPORT_F, BIT_8|BIT_5); //UART TX pins
    PORTSetPinsDigitalIn(IOPORT_F, BIT_2|BIT_4|BIT_12); //UART RX pins and GPS status pin
    //PORTSetPinsDigitalIn(IOPORT_D, BIT_3);//for GPS status monitoring
    // Master Mode
	/* SDO1 - Output - RD0
	   SDI1 - Input - RC4
	   SCK1 - Ouput - RD10
	   SS1  - Output - RD9 */

    PORTSetPinsDigitalOut (IOPORT_D, BIT_0 | BIT_9 | BIT_10); //SPI pins
    PORTSetPinsDigitalIn (IOPORT_C, BIT_4);

    // I2C Port #2 - connected to pmod CLS
    // RA14 SCL1/INT3/RA14	 N/A 	I2C Bus #2, J2, not shared with Pmod connector
    // RA15 SDA1/INT4/RA15 	 N/A

    PORTSetPinsDigitalOut (IOPORT_A, BIT_14 | BIT_15); //I2C pins
}

//****************************************************************************************************************

void reset(void)
{
    PORTWrite(IOPORT_B, 0);
}

//****************************************************************************************************************

void setup_UART1()
{
    U1MODE = 0;                         //************************************
    U1TXREG = 0;                        //*                                  *
    U1RXREG = 0;                        //* UART setup(mode, baud rate, enable)
    U1BRG = 64;                         //*
    U1STA = 5<<10;    //both Rx and Tx  //*                                  *
    U1MODE = 0b1000000000000000;        //************************************
}

//****************************************************************************************************************

void setup_UART2()
{
    U2MODE = 0;                         //************************************
    U2TXREG = 0;                        //*                                  *
    U2RXREG = 0;                        //* UART setup(mode, baud rate, enable)
    U2BRG = 64;                         //*
    U2STA = 5<<10;  //both Rx and Tx    //*                                  *
    U2MODE = 0b1000000000000000;        //************************************

}

//****************************************************************************************************************

void setup_SPIs (void)
{
	SpiChnOpen (1, SPI_CON_MSTEN  | SPI_CON_MODE8 | SPI_CON_ON | CLK_POL_ACTIVE_LOW, 256);

	// Create a falling edge pin SS to start communication
	PORTSetBits (IOPORT_D, BIT_9);
	delay (1);
	PORTClearBits (IOPORT_D, BIT_9);

        SpiChnOpen (2, SPI_CON_MSTEN  | SPI_CON_MODE8 | SPI_CON_ON | CLK_POL_ACTIVE_LOW, 256);

	// Create a falling edge pin SS to start communication
	PORTSetBits (IOPORT_G, BIT_9);
	delay (1);
	PORTClearBits (IOPORT_G, BIT_9);
}

//****************************************************************************************************************

