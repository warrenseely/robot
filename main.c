/*
 # //***************************************************************************************************************************
# //* Author:   Warren Seely // Matthew Ferran                                                                                                 *
# //* Course: NA                                                                                                             *
# //* Project:    Autonomous robotic weeder in C                                                                                     *
# //* File: robot                                                                                                   *
# //* Description: Robot control. GPS positioning and path accuracy. Manual and Automatic guidance.                                                   *
# //*                   NOTE: field_end() IS ONLY WORKABLE FOR 4 CORNER SQUARE IN THIS VERSION                                                                                                      *
# //* Inputs:       Buttons, GPS, BT module                                                                                                          *
# //* Outputs:    UART (JE 0:4), Display screen, robot motors                                                                                                            *
# //* Computations:     BAUD Rate, heading angle, distance traveled                                                                                                      *
# //*      REFER TO Robot_1 main.s for original programming info                                                                                                                   *
# //* Revision History:    4                                                                                                   *
# //***************************************************************************************************************************
*/



#include "header.h"


int main()
{
    //local variables
    int i = 0, flag;

    //Note: LCD commands are in print()
    boundary.lat1 = 46.435394;
    boundary.lon1 = 117.09728;
    boundary.lat2 = 46.435577;
    boundary.lon2 = 117.09727;
    boundary.lat3 = 46.435566;
    boundary.lon3 = 117.09707;

    boundary.width = 15; //initialize width
    reset(); //make sure robot functions are off                        //*********************************
    setup_UART1();  //Initialize the UART1 module                       //*                                *
    setup_UART2(); //Initialize the UART2 module                        //* These are the setup functions  *
    setupPORTs(); //Initialize pin RF8 as digital output                //*                                *
    setup_SPI1(); //Initialize the SPI1 module for screen communication //**********************************

    for(i = 0; i <= 3; i++)
    {
        *(&pass.nav_from_lat + i) = *(&boundary.lat1 + i); //copy first 2 coordinate pairs of boundary for first pass coordinates
    }

    while(1) //embeded systems run forever
    {
        print(0); //setup screen and let user know waiting
        startup(); //choose auto/manual/info mode; only auto mode goes past here
        get_GPS_started(); //wait here to make sure GPS has acquired signal; checks the status slot of the GPS data
        navigate_area_start(); //if robot not at area start coordinates, navigate to them(boundary.lat1 and boundary.lon1)
        set_path(0); //set headings required for back and forth; 0 indicates area heading, 1 indicates to/from area heading
        pass.direction = 1; //set initial direction
        pass.D_heading = pass.Master; //desired initial heading is MAS_head
        start_guidance(); //start externals and start robot moving forward

        while(1) //main program loop for auto mode
        {
            get_current_data(); //current lat/lon/heading
            correct_path(); //keeps robot on line

            if (job_done()) //is robot finished?
            {
                sleep(); //wait until button pressed
                break; //break out of the current while loop
            }

            field_end(); //is robot at field end? if yes then turn around
            //update field end coordinates here
            //delay (5); //time to move before turning again
            flag = 0; //reset flag each time
            while(distance(&flag) < 5); //loop until traveled 5 distance
        }
    }

    return 1; //should never actually reach here
}

