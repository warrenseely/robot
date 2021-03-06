/*
 # //***************************************************************************************************************************
# //* Author:   Warren Seely // Matthew Ferran                                                                                                 *
# //* Course: NA                                                                                                             *
# //* Project:    Autonomous robotic weeder in C                                                                                     *
# //* File: robot                                                                                                   *
# //* Description: Robot control. GPS positioning and path accuracy. Manual and Automatic guidance.                                                   *
# //*                   NOTE: field_end() IS ONLY WORKABLE FOR 4 CORNER SQUARE IN THIS VERSION                                                                                                      *
# //* Inputs:        Buttons, GPS, BT module, compass module                                                                                                          *
# //* Outputs:    UART (JE 0:4), Display screen, robot motors                                                                                                            *
# //* Computations:     BAUD Rate, heading angle, distance traveled                                                                                                      *
# //*      REFER TO Robot_1 main.s for original programming info                                                                                                                   *
# //* Revision History:    5                                                                                                   *
# //***************************************************************************************************************************
*/



#include "header.h"


int main()
{
    //local variables
    int choice, status;

    //Temps for testing purposes
    boundary.lat1 = 46.435436;
    boundary.lon1 = 117.09737;
    boundary.lat2 = 46.435577;
    boundary.lon2 = 117.09738;
    boundary.lat3 = 46.435566;
    boundary.lon3 = 117.09707;

    boundary.width = 15; //initialize width
    reset(); //make sure robot functions are off                        //*********************************
    setup_UART1();  //Initialize the UART1 module                       //*                                *
    setup_UART2(); //Initialize the UART2 module                        //* These are the setup functions  *
    setupPORTs(); //Initialize pin RF8 as digital output                //*                                *
    setup_SPIs(); //Initialize the SPI modules for screen communication //**********************************

/*********************COMPASS STUFF*********************************************/
    //Configures system for optimum preformance without changing PB divider     *
    SYSTEMConfig(GetSystemClock(), SYS_CFG_PCACHE | SYS_CFG_WAIT_STATES);

    FIFOI2C_initialize(); //setup I2C interrupts

    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR); //setup multi vector mode
    INTEnableInterrupts(); //enable interrupts for I2C

    compass_startMeasurements(); //setup compass module
/*******************************************************************************/

    while(1) //embeded systems run forever
    {
        print(0); //setup screen and let user know waiting
        choice = startup(); //choose auto/manual/info mode

        switch (choice)
        {
            case 0:
                status = auto_mode(); //Automatic guidance mode; status indicated whether completed task or user terminated
                break;

            case 1:
                manual_mode(); //manual mode
                break;

            case 2:
                load_info(); //load information into robot mode
                break;
        }
    }

    return 1; //should never actually reach here
}

