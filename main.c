/*
 # //***************************************************************************************************************************
# //* Author:   Warren Seely //Matthew Ferran                                                                                                 *
# //* Course: NA                                                                                                             *
# //* Project:    Autonomous robotic weeder in C                                                                                     *
# //* File: Robot_2C                                                                                                   *
# //* Description: Robot control. GPS positioning and path accuracy                                                  *
# //*                   NOTE: field_end IS ONLY WORKABLE FOR 4 CORNER SQUARE IN THIS VERSION                                                                                                      *
# //* Inputs:       Buttons, GPS,                                                                                                          *
# //* Outputs:    UART (JE 0:4), motors (JD 0:15)                                                                                                            *
# //* Computations:     BAUD Rate, heading angle                                                                                                      *
# //*      REFER TO Robot_1 main.s for further programming info                                                                                                                   *
# //* Revision History:    3                                                                                                   *
# //***************************************************************************************************************************
*/

//#include <plib.h>


#include "header.h"

/*Global variables*/

int width = 15;//width for robot(default) ADD INTO load_info later

int main()
{
   /* boundary.lat1 = 46.725692;
    boundary.lon1 = 117.162027;
    boundary.lat2 = 46.726196;
    boundary.lon2 = 117.162090;
    boundary.lat3 = 46.726194;
    boundary.lon3 = 117.161652;
    boundary.lat4 = 46.725802;
    boundary.lon4 = 117.161615;*/
    //local variables
    int i = 0;
    //Note: LCD commands are in the print()

    reset(); //make sure robot functions are off                        //*********************************
    setup_UART1();  //Initialize the UART1 module                       //*                                *
    setup_UART2(); //Initialize the UART2 module                        //* These are the setup functions  *
    setupPORTs(); //Initialize pin RF8 as digital output                //*                                *
    setup_SPI1(); //Initialize the SPI1 module for screen communication //**********************************

    for(i=0; i<=3;i++)
    {
        *(&pass.nav_from_lat + i) = *(&boundary.lat1 + i);//copy first 2 coordinate pairs of boundary for first pass coordinates
    }

    while(1)//embeded systems run forever
    {
        print(0);//setup screen and let user know waiting
        startup();//choose auto/manual/info mode; only auto mode goes past here
        //interference on RD14
        //get_GPS_started();//if robot not at start area, calculate path to start area
        //navigate_area_start();//if robot not at area start coordinates, navigate to them(boundary.lat1 and boundary.lon1)
        set_path(0); //set headings required for back and forth; 0 indicates area heading, 1 indicates to/from area heading
        pass.direction = 1; //set initial direction
        pass.D_heading = pass.Master; //desired initial heading is MAS_head

        while(1)//main program loop for auto mode
        {
            get_current_data();//current lat/lon/heading
            correct_path(); //keeps robot on line

            if (job_done())//is robot finished?
            {
                do//robot done and program finished
                {

                } while((PORTRead(IOPORT_A) & 0xC0) == 0);//loop until someone wakes robot again by pressing a button
                break;//break out of the current while loop
            }

            field_end();//is robot at field end? if yes then turn around
            //update field end coordinates here
        }
    }

    return 1;//should never actually reach here
}

