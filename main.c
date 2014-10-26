/*
 # //***************************************************************************************************************************
# //* Author:   Warren Seely                                                                                                  *
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
double boundary[18] = {0};//for the boundary coordinates; 9 coordinate pairs; even index is latitudes, odd is longitudes;
double MAS_head = 0, MAS_head1 = 0, D_heading = 0, C_heading = 0, direction = 0;//master heading/secondary heading(180 off master)/desired heading/current heading/current direction
double C_pos_Lat = 0, C_pos_Lon = 0, speed = 0, clat1 = 0, clon1 = 0;//current latitude/longitude/speed; distance temp/distance temp
int width = 15;//width for robot(default) ADD INTO load_info later
double c_pass[4] = {0};//the coordinates to align every width

int main()
{
    //local variables
    int i = 0;
    //Note: LCD commands are in the print()

    reset(); //make sure robot functions are off
    setup_UART1();  //Initialize the UART1 module
    setup_UART2(); //Initialize the UART2 module
    setupPORTs(); //Initialize pin RF8 as digital output
    setup_SPI1();

    for(i=0; i<=3;i++)
    {
        c_pass[i] = boundary[i];//copy first 4 slots of boundary to move for first line coordinates
    }

    while(1)
    {
        print(0);//setup screen and let user know waiting
        startup();//choose auto/manual/info mode; only auto mode goes past here
        //get_GPS_started();//if robot not at start area, calculate path to start area
       // set_path(0); //set headings required for back and forth; 0 indicates area heading, 1 indicates to/from area heading
        direction = 1; //set initial direction
        D_heading = MAS_head; //desired initial heading is MAS_head

        while(1)//main program loop for auto mode
        {
            get_current_data();//current lat/lon/heading
            correct_path(); //keeps robot on line
            //need to check if at field end
            //field_end();//is robot at field end? if yes then turn around
            if (job_done())//is robot finished?
            {
                do//robot done and program finished
                {

                } while((PORTRead(IOPORT_A) & 0xC0) == 0);//loop until someone wakes robot again by pressing a button
                break;//break out of the current while loop
            }
        }
    }

    return 1;//should never actually reach here
}

