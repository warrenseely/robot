/*
 # //***************************************************************************************************************************
# //* Author:   Warren Seely                                                                                                  *
# //* Course: NA                                                                         *
# //* Project:    Autonomous robotic weeder in assembly                                                                                     *
# //* File: CodeTemplate.s                                                                                                    *
# //* Description: Robot control. GPS positioning and path accuracy                                                  *
# //*                                                                                                                         *
# //* Inputs:       Buttons, GPS, CMPS                                                                                                          *
# //* Outputs:    UART (JE 0:4), motors (JD 0:15)                                                                                                            *
# //* Computations:     BAUD Rate, heading angle                                                                                                      *
# //*      REFER TO Robot_1 main.s for further programming info                                                                                                                   *
# //* Revision History:    1                                                                                                   *
# //***************************************************************************************************************************
*/
#include <p32xxxx.h>
#include <plib.h>
#include <math.h>
#include "config.c"
#include "setup.c"
#include "robot_control.c"
#include "header.h"
#include "string.h"


int main()
{
    //variables
    int var1lat = 0, var1lon = 0, var2lat = 0, var2lon = 0, var3 = 0, var4 = 0, var5 = 0, var6 = 0, var7 = 0, var8 = 0, var9lat = 0, var9lon = 0;
    double MAS_head = 0, MAS_head1 = 0, D_heading = 0, C_heading = 0, direction = 0;
    double C_pos_Lat = 0, C_pos_Lon = 0;
    int x = 0;
    //Note: LCD commands are in the print function


    reset(); //make sure robot functions are off
    setup_UART1();   // Initialize the UART1 module
    setup_UART2(); //Initialize the UART2 module
    setupPORTs(); // Initialize pin RF8 as digital output
    print(0);//setup screen and let user know waiting
    startup();

    //set_path(var1lat, var1lon, var2lat, var2lon, &MAS_head, &MAS_head1); //set headings required for back and forth
    direction = 1; //set initial direction
    D_heading = MAS_head; //desired initial heading is MAS_head

    while(1)
    {
        get_current_data(&C_pos_Lat, &C_pos_Lon, &C_heading);//current lat/lon/heading
        correct_path(&D_heading, &C_pos_Lat, &C_pos_Lon, var1lat, var1lon, direction); //keeps robot on line
        //field_end(&C_pos_Lat, &C_pos_Lon, &D_heading, direction, &C_heading, &MAS_head1, &MAS_head);
    }



    return 1;
}

