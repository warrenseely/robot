/*
#// ***************************************************************************************************************************
#// *                                                                                                                         *
#// *                        THIS FILE PERFORMS AUTONOMOUS ROBOT GUIDANCE COMMANDS                                                *
#// *                                                                                                                         *
#// ***************************************************************************************************************************

#//*********INFORMATION**********************************************************************************************
#//GPS set up in port JE(uart1)
#//
#//******************************************************************************************************************
*/

#include "header.h"
#include "compass.h"
#include <plib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <peripheral/spi.h>


/*
 * Function: correct_path ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: ???
 * Discription: Takes readings from (C_heading, D_heading) and keeps robot on correct path
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
void correct_path (void)
{
    double x = 0, y = 0, z = 0;
    unsigned int motor = 0;


//weaties was here bwhahaha


    x = pass.nav_from_lat - Position.lat; //difference in the latitudes

    y = pass.nav_from_lon - Position.lon;   //difference in the longitudes

    z = atan (x / y); //get position heading, use this to compare to north and desired heading
    z = ((double)180 / 3.14159) * z;  //convert to degrees
    x = pass.D_heading - z;  //subtract heading here to compare

    if (pass.direction == 1) //if current direction is 1, initial heading to process left/right commands
    {
        if (x > pass.D_heading)  //go right
        {
            motor = 8 << 10;
        }
        else if (x < pass.D_heading)  //go left
        {
            motor = 3 << 10;
        }
        else //case that robot is on the path, but going 90 deg to it
        {
            
        }
    }
    else    //direction is 0
    {
        if (x < pass.D_heading)   //go left
        {
            motor = 8 << 10;
        }
        else if (x > pass.D_heading)  //go right
        {
            motor = 3 << 10;
        }
        else //case that robot is on the path, but going 90 deg to it
        {

        }
    }

    PORTWrite (IOPORT_B, motor); //turn direction
    //delay(1); //time to turn; (change to a below amount eventually)

     z = abs(z); //make sure z positive
     y = 2*(z - 2); //amount to turn to correct -> amount = 2*((amount off) - 2)
     //z = Position.course; //preserve current heading on GPS
     read_compass(); //get compass heading
     z = heading.course; //preserve current course
     
     do
     {
          //get_current_data(); //update Position.course
         read_compass(); //update current course
          //x = Position.course - z; //change in course from beginning of loop until current
         x = abs(heading.course - z); //change in course from beginning of loop until current
     }while (x <= y); //loop until turned by amount to get back on track

    PORTWrite (IOPORT_B, 11 << 10); //both back on
}

//****************************************************************************************************************

/*
 * Function: set_path ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: 10/26/14
 * Discription: Sets a robo-nav path between first 2 coordinate pairs (in theory...)
 * Input: 0 or 1, flags if it is going forward (0) or 180 degrees from forward (1)
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
void set_path (int flag) //sets path between first 2 coordinate pairs for robot to follow
{
    double x = 0, y = 0, z = 0;

    if (flag == 0)  //area heading
    {
        x = boundary.lat1 - boundary.lat2;  //difference in the latitudes
        y = boundary.lon1 - boundary.lon2;  //difference in the longitudes
    }
    else if (flag == 1)  //to/from area heading
    {
        x = boundary.lat1 - Position.lat;   //difference in the latitudes(current to desired start)
        y = boundary.lon1 - Position.lon;   //difference in the longitudes(current to desired start)
    }

    z = atan (x / y);  //get heading, use this to compare to north and
                     ////desired heading -->****IN RADIANS RIGHT NOW****
    z = (180 / 3.14159) * z;   //convert to degrees

    pass.Master = z;    //master heading
    if (flag == 1)   //to/from area heading
    {
        return; //done in this function, exit
    }
    z = pass.Master - 180; //if less than or equal 0, know add 180 to get degrees for opposite direction

    if (z <= 0) //add 180 to get opposite direction
    {
        pass.Secondary = pass.Master + 180;
    }
    else    //difference is greater than 0, so subtract 180 to get opposite direction
    {
        pass.Secondary = z;
    }

}

//****************************************************************************************************************

/*
 * Function: job_done ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: ???
 * Discription: Singnals the program to stop
 * Input: n/a
 * Returns: 1 if job's done, 0 if job's not done
 * Preconditions: The program must be running
 * Postconditions: The program will either stop or continue
 */
int job_done (void)
{
    int i = 0;

    while ((*(&boundary.lat1 + i) != 0) && (i < 19)) //find end of boundary coordinates(may be less than 9 pairs)
    {
        i = i + 2;  //only looking at the latitudes
    }
    if ( (Position.lat == *(&boundary.lat1 + (i - 2)) ) && //if current position is
         (Position.lon == *(&boundary.lat1 + (i - 1)) ) )   ////end of boundary coordinates
    {
        shut_down ();
        return 1;   //job is done
    }
    return 0;   //job is not done
}

//****************************************************************************************************************

/*
 * Function: navigate_area_start ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: 10/26/14
 * Discription: Navigates the robot to the start point
 * Input: n/a
 * Returns: n/a
 * Preconditions: The robot is somewhere
 * Postconditions: The robot is at the start point
 */
void navigate_area_start (void)
{
    double clat2, clon2, temp;

    clat2 = boundary.lat1;
    clon2 = boundary.lon1;

    get_current_data (); //current lat/lon/heading
    
    if (convert_feet(clat2, clon2) <= 5) //if within 5 feet of start coordinates
    {
        return;
    }
    
    set_path(1); //path to follow to start point

    while(convert_feet(clat2, clon2) >= 5) //get distance to start point in feet
    {
        get_current_data(); //get the current latitude/longitude
        correct_path(); //guide the robot
    }
}

//****************************************************************************************************************

/*
 * Function: field_end ()
 * Author: Warren Seely
 * Date Created: 10/29/14
 * Date Last Modified: 10/29/14
 * Discription: check if robot is at the end of pass; if yes, turn around
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */

 double field_end(void)
 {
     int flag = 0, x = 1;
     double clat2, clon2, temp = 0;

     pass.clat1 = Position.lat; //copy current position coordinates
     pass.clon1 = Position.lon;
     clat2 = pass.nav_to_lat; //get the field end coordinates
     clon2 = pass.nav_to_lon;

     //if current position is not the same as the end coordinates of the current pass, return
     //otherwise continue(current position is the same as end coordinates of current pass)-> reached end
     if (convert_feet(clat2, clon2) > 5) //if farther than 5 feet away from end
     {
         return;
     }

     if (pass.direction == 1) //if current direction is initial direction
     {
         PORTWrite(IOPORT_B, 8 << 10); //right side off
         pass.D_heading += 90; //add 90 degrees
         read_compass(); //get current heading
         temp = heading.course; //copy current heading

         while(x > 0) // loop until current heading is 90 degrees
         {
             //get_current_data(); //get current heading, mainly going for compass heading here
             read_compass(); //get current heading
             //x = pass.D_heading - Position.course; //subtract to compare
             //x = pass.D_heading - heading.course; //subtract to compare
             x = abs(temp) - abs(heading.course); //change in heading from start to current
         }

         PORTWrite(IOPORT_B, 11 << 10); //turned desired amount, continue

         while(distance(&flag) < boundary.width); //wait until traveled width
         
          //update the coordinates for the field end points(Simplified, assuming a square field)
         compute_pass_point();

         pass.D_heading = pass.Secondary; //desired new heading is secondary heading(Secondary) 180 degrees off previous heading

         PORTWrite(IOPORT_B, 8 << 10); //right side off
         read_compass(); //get current heading
         temp = heading.course; //copy current heading

         while(x > 0) // loop until current heading is 90 degrees
         {
             //get_current_data(); //get current heading, mainly going for compass heading here
             //x = pass.D_heading - Position.course; //subtract to compare
             read_compass(); //get current heading
             x = abs(temp) - abs(heading.course); // change in heading from start to current
         }

         PORTWrite(IOPORT_B, 11 << 10); //turned desired amount, continue
         pass.direction = 0; //update direction
     }
     else //current direction is secondary direction
     {
         PORTWrite(IOPORT_B, 3 << 10); //left side off
         pass.D_heading -= 90; //subtract 90 degrees
         read_compass(); //get current heading
         temp = heading.course; //copy current heading

         while(x > 0) // loop until course = D_heading - 90
         {
             //get_current_data(); //get current heading, mainly going for compass heading here
             //x = pass.D_heading - Position.course; //subtract to compare
             read_compass(); //get current heading
             x = abs(temp) - abs(heading.course); // change in heading from start to current
         }

         PORTWrite(IOPORT_B, 11 << 10); //turned desired amount, continue
         flag = 0; //reset flag
         while(distance(&flag) < boundary.width); //wait until traveled width

         //update the coordinates for the field end points(Simplified, assuming a square field)
         compute_pass_point();

         pass.D_heading = pass.Master; //desired new heading is primary heading(Master) 180 degrees off previous heading

         PORTWrite(IOPORT_B, 3 << 10); //left side off
         read_compass(); //get current heading
         temp = heading.course; //copy current heading
         
         while(x > 0) // loop until course = desired heading
         {
             //get_current_data(); //get current heading, mainly going for compass heading here
             //x = pass.D_heading - Position.course; //subtract to compare
             read_compass(); //get current heading
             x = abs(temp) - abs(heading.course); // change in heading from start to current
         }

         PORTWrite(IOPORT_B, 11 << 10); //turned desired amount, continue
         pass.direction = 1; //update direction
     }
 }

 //****************************************************************************************************************

/*
 * Function: comput_pass_point ()
 * Author: Warren Seely
 * Date Created: 10/29/14
 * Date Last Modified: 10/29/14
 * Discription: Compute the point to navigate to on current pass
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */

 void compute_pass_point()
 {
     double x, y;

     x = Position.lat - pass.nav_to_lat + pass.nav_from_lat; //the new latitude coordinate to navigate to
     y = Position.lon - pass.nav_to_lon + pass.nav_from_lon; //the new longitude coordinate to navigate to

     //load updated line coordinates into the "current pass" struct
     pass.nav_from_lat = Position.lat; //latitude traveling from
     pass.nav_from_lon = Position.lon; //longitude traveling from
     pass.nav_to_lat = x; //latitude traveling to
     pass.nav_to_lon = y; //longitude traveling to
 }

//****************************************************************************************************************

/*
 * Function: distance ()
 * Author: Warren Seely
 * Date Created: 10/29/14
 * Date Last Modified: 10/29/14
 * Discription: Compute distance traveled from initial call to current call.
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */

 double distance(int *flag) //computes distance from initial call until current call; set flag = 0 before initial call;
 {                          //used with loop, returns distance
     double clat2 = 0, clon2 = 0, d = -1; //temps for latitude, longitude, distance

     if (*flag == 0) //first time, copy current(start) location and get start coordinate
     {
        pass.clat1 = Position.lat; //copy Position.lat, Position.lon; preserve
        pass.clon1 = Position.lon;
        *flag = 1; //set flag
        return d;
     }

     get_current_data(); //get current position
     clat2 = Position.lat; //copy Position.lat, Position.lon
     clon2 = Position.lon;

     //ocnvert the coordinates to feet
     d = convert_feet(clat2, clon2); //pass in clat2, clon2; can get clat1 and clon1 from pass struct

     return d; //current distance
 }

 //****************************************************************************************************************

/*
 * Function: get_GPS_started ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: 11/3/14
 * Discription: Wait until GPS signal acquired
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
void get_GPS_started (void)
{

    print(1);   //let user know GPS being acquired

    while(get_current_data() != 'A'); //If it returns an 'A', GPS is connected

    print (2);  //let user know GPS acquired
}

//****************************************************************************************************************
/*
 * Function: start_guidance ()
 * Author: Warren Seely
 * Date Created: 10/30/14
 * Date Last Modified: 10/30/14
 * Discription: Start the robot moving forward; Eventually check to make sure all external sensors
 *              working before continuing
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */

 void start_guidance(void)
 {
     //here check external sensors for valid feedback(working) before continuing

     //start externals
     PORTWrite(IOPORT_E, 0xFF); //Enable boom nozzels
     PORTWrite(IOPORT_B, 1 << 10); //set direction forward
     PORTWrite(IOPORT_B, 11 << 10); //start robot going forward

 }

 //****************************************************************************************************************

 /*
 * Function: convert_feet ()
 * Author: Warren Seely
 * Date Created: 11/4/14
 * Date Last Modified: 11/4/14
 * Discription: convert latitude/longitude coordinates to feet
 * Input: n/a
 * Returns: distance in feet
 * Preconditions: n/a
 * Postconditions: n/a
 */

 double convert_feet(double clat2, double clon2)
 {
     double c1 = 0, c2 = 0, temp1 = 0;

     //get the stored current latitude and longitude
     c1 = pass.clat1;             
     c2 = pass.clon1;             
                           
     //first latitude conversion
     temp1 = ((clat2 - (int)clat2) * 100) / 60; //get the minutes(decimal part) and divide by 60 to convert to degrees
     clat2 = (int)clat2 + temp1; //put the number back together; In degrees now
     clat2 *= ((1000 * 3280.4) / 9); //convert to kilometers(10000/90) then feet(*3280.4)

     //first longitude conversion
     temp1 = ((clon2 - (int)clon2) * 100) / 60; //get the minutes(decimal part) and divide by 60 to convert to degrees
     clon2 = (int)clon2 + temp1; //put the number back together; In degrees now
     clon2 *= ((1000 * 3280.4) / 9); //convert to kilometers(10000/90) then feet(*3280.4)

     //second latitude conversion
     temp1 = ((c1 - (int)c1) * 100) / 60; //get the minutes(decimal part) and divide by 60 to convert to degrees
     c1 = (int)c1 + temp1; //put the number back together; In degrees now
     c1 *= ((1000 * 3280.4) / 9); //convert to kilometers(10000/90) then feet(*3280.4)

     //second longitude conversion
     temp1 = ((c2 - (int)c2) * 100) / 60; //get the minutes(decimal part) and divide by 60 to convert to degrees
     c2 = (int)c2 + temp1; //put the number back together; In degrees now
     c2 *= ((1000 * 3280.4) / 9); //convert to kilometers(10000/90) then feet(*3280.4)

     c1 = pow((c1 - clat2), 2); //get "x" coordinates
     c2 = pow((c2 - clon2), 2); //get "y" coordinates

     temp1 = sqrt(c1 + c2); //current distance in feet

     return temp1; //return the distance in feet
 }

 //****************************************************************************************************************

 /*
 * Function: killswitch ()
 * Author: Matthew Ferran // Warren Seely
 * Date Created: 11/5/14
 * Date Last Modified: 11/5/14
 * Discription: Allows the user to stop the robot while its in auto mode through bluetooth or by hitting button 1
 * Input: n/a
 * Returns: n/a
 * Preconditions: robot is in auto mode
 * Postconditions: robot has stopped moving
 */
 int killswitch (void)
 {
     int state = 0;
     char temp = '\0';

    if(DataRdyUART2()) //check if new data ready
    {
        temp = U2RXREG; //get the character
        if(temp == ' ') //if character is a "space"(halt command)
        {
            return 1;
        }
    }

     state = PORTRead (IOPORT_A) & 0xC0; //get button status
     
     if (state == 0x40) //button one
     {
         return 1;
     }

     return 0;
 }

 //****************************************************************************************************************

 /*
 * Function: auto_mode ()
 * Author: Warren Seely
 * Date Created: 11/5/14
 * Date Last Modified: 11/5/14
 * Discription: the main automatic guidance function
 * Input: n/a
 * Returns: n/a
 * Preconditions:
 * Postconditions:
 */

 int auto_mode(void)
 {
     int flag = 0, i, exit, temp, temp1;

     for(i = 0; i <= 3; i++)
     {
         *(&pass.nav_from_lat + i) = *(&boundary.lat1 + i); //copy first 2 coordinate pairs of boundary for first pass coordinates
     }

     get_GPS_started(); //wait here to make sure GPS has acquired signal; checks the status slot of the GPS data

     /*****ENABLE WHEN DONE TESTING PATH FOLLOWING*****/
     //navigate_area_start(); //if robot not at area start coordinates, navigate to them(boundary.lat1 and boundary.lon1)

     set_path(0); //set headings required for back and forth; 0 indicates area heading, 1 indicates to/from area heading
     pass.direction = 1; //set initial direction
     pass.D_heading = pass.Master; //desired initial heading is MAS_head
     start_guidance(); //start externals and start robot moving forward

     while(1) //main program loop for auto mode
     {
         get_current_data(); //current lat/lon/heading
         correct_path(); //keeps robot on line

         field_end(); //is robot at field end? if yes then turn around

         temp = killswitch();
         temp1 = job_done();
         //delay (5); //time to move before turning again
         if (temp || temp1) //emergency stop or robot finished
         {
             if(temp)
             {
                 exit = 0; //terminated on user order
             }
             else
             {
                 exit = 1; //successfully completed field
             }
             
             shut_down(); //stop robot and shut booms off
             sleep(); //wait until a button is pressed
             break; //break out of current while loop
         }

         flag = 0; //reset flag each time
         while(distance(&flag) < 5) ; //loop until traveled 5 feet
     }
     return exit;
 }

 //****************************************************************************************************************

 /*
 * Function: compass_startMeasurements ()
 * Author: Warren Seely
 * Date Created: 11/20/14
 * Date Last Modified: 11/20/14
 * Discription: set compass up
 * Input: n/a
 * Returns: n/a
 * Preconditions:
 * Postconditions:
 */

 void compass_startMeasurements()
{
    int i = 0;
    uint8 buffer[10];

    //Config Register A
    buffer[0] = 0x74; //111 0100  //30hz refresh rate

    //Config Register B
    buffer[1] = 0xA0;

    //Mode Register
    buffer[2] = 0x00; //cont meaurement mode

    FIFOI2C_addQueue_writeDeviceRegisters(0, 0x00, buffer, 3);
}

//****************************************************************************************************************

 /*
 * Function: compass_queueReadXZY ()
 * Author: Warren Seely
 * Date Created: 11/20/14
 * Date Last Modified: 11/20/14
 * Discription: reads compass data
 * Input: n/a
 * Returns: n/a
 * Preconditions:
 * Postconditions:
 */

void compass_queueReadXZY()
{
    FIFOI2C_addQueue_readDeviceRegisters(0, 0x03, 6);
}

//****************************************************************************************************************

 /*
 * Function: compass_interpretXZY ()
 * Author: Warren Seely
 * Date Created: 11/20/14
 * Date Last Modified: 11/20/14
 * Discription: converts compass readings to degrees
 * Input: n/a
 * Returns: n/a
 * Preconditions:
 * Postconditions:
 */

void compass_interpretXZY()
{
    uint8 x_msb = 0, x_lsb = 0;
    uint8 z_msb = 0, z_lsb = 0;
    uint8 y_msb = 0, y_lsb = 0;
    short x_tmp = 0, z_tmp = 0, y_tmp= 0;
    double gauss_per_LSB = 0, front_heading = 0, XVector = 0, YVector = 0, ZVector = 0;

    x_msb = FIFOI2C_readQueue(0).rx_byte;
    x_lsb = FIFOI2C_readQueue(0).rx_byte;

    z_msb = FIFOI2C_readQueue(0).rx_byte;
    z_lsb = FIFOI2C_readQueue(0).rx_byte;

    y_msb = FIFOI2C_readQueue(0).rx_byte;
    y_lsb = FIFOI2C_readQueue(0).rx_byte;

    x_tmp = (x_msb << 8) | x_lsb;
    z_tmp = (z_msb << 8) | z_lsb;
    y_tmp = (y_msb << 8) | y_lsb;

    //Based on devie 'Mode Register.' Refer to HMC5883L data sheet.
    gauss_per_LSB = 0.92e-3;

    heading.x1 = gauss_per_LSB * (double)x_tmp;
    heading.y1 = gauss_per_LSB * (double)y_tmp;
    heading.z1 = gauss_per_LSB * (double)z_tmp;

    heading.course = atan2(heading.y1, heading.x1) * (180 / 3.14159); //convert radians to degrees

    if (heading.course < 0)
    {
        heading.course += 360; //make sure a positive angle between 0-360
    }
    if (heading.course > 360)
    {
        heading.course -= 360; //make sure angle between 0-360
    }
}

//****************************************************************************************************************

 /*
 * Function: read_compass ()
 * Author: Warren Seely
 * Date Created: 11/20/14
 * Date Last Modified: 11/20/14
 * Discription: calls the functions to get compass readings
 * Input: n/a
 * Returns: n/a
 * Preconditions:
 * Postconditions:
 */

void read_compass()
{
    int i = 0;

    while (i++ < 3) //loop 3 times to get a reading
    {
        compass_queueReadXZY(); //read compass
        compass_interpretXZY(); //convert to degrees
        DelayTime(200); //wait some time
    }
}