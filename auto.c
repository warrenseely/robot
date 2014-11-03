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


//weaties was here bwhahaha


    x = pass.nav_from_lat - Position.lat; //difference in the latitudes

    y = pass.nav_from_lon - Position.lon;   //difference in the longitudes

    z = atan (x/y); //get position heading, use this to compare to north and desired heading
    z = ((double)180/3.14159) * z;  //convert to degrees

    if (pass.direction == 1) //if current direction is 1, initial heading to process left/right commands
    {
        x = pass.D_heading - z;  //subtract heading here to compare

        if (x > pass.D_heading)  //go right
        {
            PORTWrite (IOPORT_B, 2<<10);    //right side off, left side on
            delay (1);  //time to turn
            PORTWrite (IOPORT_B, 3<<10);    //both back on
        }
        else if(x < pass.D_heading)  //go left
        {
            PORTWrite (IOPORT_B, 1<<10);    //left side off, right side on
            delay (1);  //time to turn
            PORTWrite (IOPORT_B, 3<<10);    //both back on
        }
        else    //x = 0; need to check heading and make sure it is correct(Think position is correct, but robot going 90deg)
        {

        }
    }
    else    //direction is 0
    {
        x = pass.D_heading - z;  //subtract heading here to compare

        if(x < pass.D_heading)   //go left
        {
            PORTWrite(IOPORT_B, 1<<10); //left side off, right side on
            delay(1); //time to turn
            PORTWrite(IOPORT_B, 3<<10); //both back on
        }
        else if(x > pass.D_heading)  //go right
        {
            PORTWrite(IOPORT_B, 2<<10); //right side off, left side on
            delay(1); //time to turn
            PORTWrite(IOPORT_B, 3<<10); //both back on
        }
        else    //x = 0; need to check heading and make sure it is correct(Think position is correct, but robot going 90deg)
        {

        }
    }
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

    while ( (*(&boundary.lat1 + i) != 0) && (i < 19)) //find end of boundary coordinates(may be less than 9 pairs)
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
    get_current_data (); //current lat/lon/heading
//    if ( (Position.lat == boundary.lat1) &&
//         (Position.lon == boundary.lon1) ) //if current position is correct, skip function
//    {
//        return; //exit function
//    }
//    set_path (1); //need to compute path from current to var1 coordinates
//    while ( (Position.lat != boundary.lat1) &&
//            (Position.lon != boundary.lon1) ) //while not at start destination
//    {
//        correct_path (); //guide robot
//        get_current_data (); //current lat/lon/heading
//    }
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

     //if current position is not the same as the end coordinates of the current pass, return
     //otherwise continue(current position is the same as end coordinates of current pass)-> reached end
     if ((Position.lat != pass.nav_to_lat) || (Position.lon != pass.nav_to_lon))
     {
         return;
     }

     if (pass.direction == 1) //if current direction is initial direction
     {
         PORTWrite(IOPORT_B, 2<<10); //right side off
         pass.D_heading += 90; //add 90 degrees
         while(x > 0) // loop until C_heading = D_heading + 90
         {
             get_current_data(); //get current heading, mainly going for compass heading here
             x = pass.D_heading - Position.course; //subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10); //turned desired amount, continue

         while(distance(flag) < boundary.width) //wait until traveled width
         {
         }

          //update the coordinates for the field end points(Simplified, assuming a square field)
         compute_pass_point();

         pass.D_heading = pass.Secondary; //desired new heading is secondary heading(Secondary) 180 degrees off previous heading

         PORTWrite(IOPORT_B, 2<<10); //right side off

         while(x > 0) // loop until C_heading = MAS_hed1
         {
             get_current_data(); //get current heading, mainly going for compass heading here
             x = pass.D_heading - Position.course; //subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10); //turned desired amount, continue
         pass.direction = 0; //update direction
     }
     else //current direction is secondary direction
     {
         PORTWrite(IOPORT_B, 1<<10); //left side off
         pass.D_heading -= 90; //subtract 90 degrees

         while(x > 0) // loop until course = D_heading - 90
         {
             get_current_data(); //get current heading, mainly going for compass heading here
             x = pass.D_heading - Position.course; //subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10); //turned desired amount, continue
         flag = 0; //reset flag
         while(distance(flag) < boundary.width) //wait until traveled width
         {
         }

         //update the coordinates for the field end points(Simplified, assuming a square field)
         compute_pass_point();

         pass.D_heading = pass.Master; //desired new heading is primary heading(Master) 180 degrees off previous heading

         PORTWrite(IOPORT_B, 1<<10); //left side off

         while(x > 0) // loop until course = desired heading
         {
             get_current_data(); //get current heading, mainly going for compass heading here
             x = pass.D_heading - Position.course; //subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10); //turned desired amount, continue
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

 double distance(int flag) //computes distance from initial call until current call; set flag = 0 before initial call;
 {                          //used with loop, returns distance
     double c1, c2 = 0, clat2 = 0, clon2 = 0, d = 0; //temps for latitude, longitude, distance

     if (flag == 0) //first time, copy current(start) location and get start coordinate
     {
        pass.clat1 = Position.lat; //copy Position.lat, Position.lon; preserve
        pass.clon1 = Position.lon;
     }

     get_current_data(); //get current position
     clat2 = Position.lat; //copy Position.lat, Position.lon
     clon2 = Position.lon;

     c1 = pow((pass.clat1 - clat2), 2); //get "x" coordinates
     c2 = pow((pass.clon1 - clon2), 2); //get "y" coordinates

     d = sqrt(c1 + c2); //current distance
     flag = 1; //set flag

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
 * Discription: //start the robot moving forward; Eventually check to make sure all external sensors
                //working before continuing
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
     PORTWrite(IOPORT_B, 3 << 10); //start robot going forward
 }

 //****************************************************************************************************************
