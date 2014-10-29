/*
#// ***************************************************************************************************************************
#// *                                                                                                                         *
#// *                        THIS FILE PERFORMS ROBOT MOVEMENT COMMANDS                                                *
#// *                                                                                                                         *
#// ***************************************************************************************************************************

#//*********INFORMATION**********************************************************************************************
#//redo correct_path so works on all lines every width-->create c_pass[4] for storing lat/lon every width across area;also pass into set_path
#//load_info set up for BT2, in port JH(uart2)
#//GPS set up in port JE(uart1)
#//For screen on UART: MD0,MD2 are shorted; for screen on SPI: MD0 is shorted, Jp1 SS is shorted;
#//Set up for SPI1 right now
#//look into sensors; light, IR, touch
#//look into robot tracking(in case robot stolen)
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
    int x = 0, y = 0, z = 0;


//weaties was here bwhahaha


    x = boundary.lat1 - Position.lat; //difference in the latitudes
    if (x < 0)
    {
        x = x * (-1);   //make sure x is positive
    }

    y = boundary.lon1 - Position.lon;   //difference in the longitudes
    if (y < 0)
    {
        y = y * (-1);   //make sure y is positive
    }

    z = atan (x/y); //get heading, use this to compare to north and desired heading
    z = (180/3.14159) * z;  //convert to degrees
    if (pass.direction == 1) //if current direction is 1, initial heading to process left/right commands
    {
        x = pass.D_heading - z;  //subtract heading here to compare

        if (x > 0)  //go right
        {
            PORTWrite (IOPORT_B, 2<<10);    //right side off, left side on
            delay (1);  //time to turn
            PORTWrite (IOPORT_B, 3<<10);    //both back on
        }
        else if(x < 0)  //go left
        {
            PORTWrite (IOPORT_B, 1<<10);    //left side off, right side on
            delay (1);  //time to turn
            PORTWrite (IOPORT_B, 3<<10);    //both back on
        }
        else    //x = 0; need to check headingand make sure it is correct(Think position is correct, but robot going 90deg)
        {

        }
    }
    else    //direction is 0
    {
        x = pass.D_heading - z;  //subtract heading here to compare

        if(x < 0)   //go left
        {
            PORTWrite(IOPORT_B, 1<<10); //left side off, right side on
            delay(5); //time to turn
            PORTWrite(IOPORT_B, 3<<10); //both back on
        }
        else if(x > 0)  //go right
        {
            PORTWrite(IOPORT_B, 2<<10); //right side off, left side on
            delay(5); //time to turn
            PORTWrite(IOPORT_B, 3<<10); //both back on
        }
        else    //x = 0; need to check headingand make sure it is correct(Think position is correct, but robot going 90deg)
        {

        }
    }
}

//****************************************************************************************************************

/*
 * Function: correct_path1 ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: ???
 * Discription: Moves the path-to-follow lines across the field (in theory...)
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
void correct_path1 (void) //moves the path-to-follow lines across field
{
    int x = 0, y = 0, z = 0;

    x = boundary.lat1 - Position.lat; //difference in the latitudes
    if (x < 0)
    {
        x = x * (-1); //make sure x is positive
    }

    y = boundary.lon1 - Position.lon; //difference in the longitudes
    if (y < 0)
    {
        y = y *(-1); //make sure y is positive
    }

    z = atan (x/y); //get heading, use this to compare to north and desired heading
    z = (180/3.14159) * z; //converto to degrees
    if (pass.direction == 1) //if current direction is 1, initial heading to process left/right commands
    {
        x = pass.D_heading - z; //subtract heading here to compare

        if (x < 0) //go right
        {
            PORTWrite (IOPORT_B, 2<<10); //right side off, left side on
            delay (5); //time to turn
            PORTWrite (IOPORT_B, 3<<10); //both back on
        }
        else //go left
        {
            PORTWrite (IOPORT_B, 1<<10); //left side off, right side on
            delay (5); //time to turn
            PORTWrite (IOPORT_B, 3<<10); //both back on
        }
    }
    else //direction is 0
    {
        x = pass.D_heading - z; //subtract heading here to compare

        if (x < 0) //go left
        {
            PORTWrite (IOPORT_B, 1<<10); //left side off, right side on
            delay (5); //time to turn
            PORTWrite (IOPORT_B, 3<<10); //both back on
        }
        else //go right
        {
            PORTWrite (IOPORT_B, 2<<10); //right side off, left side on
            delay (5); //time to turn
            PORTWrite (IOPORT_B, 3<<10); //both back on
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

    if (x < 0)
    {
        x = x * (-1);   //make sure x is positive
    }
    if (y < 0)
    {
        y = y * (-1);   //make sure y is positive
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
 * Function: get_current_data ()
 * Author: Warren Seely // Matthew Ferran
 * Date Created: 10/25/14
 * Date Last Modified: 10/26/14
 * Discription: Reads the RMC data string from the GPSdata struct and parses
 *              the latitude and longitude into a global struct for current data.
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
void get_current_data (void) //gets current lat/lon/speed and heading
{
    struct GPS_DATA_T GPSdata;  //Struct to read the RMC data into

    char temp = '\0';
    int flag = 0;   //Loop control flag

    while (flag != 1) //Keeps going until the full RMC string is read into
                    ////GPSdata
    {

        UART1ClearAllErrors (); //Prep buffer for reading

        //check if new data is ready

        if (DataRdyUART1()) //if new data is ready...
        {

            temp = U1RXREG; //Get the first char from stream

            if (temp == '$')    //if at start of string
            {
                  read_GPS_fields (GPSdata.ID); //grab string

                  if(GPSdata.ID[3] == 'M')  //check to make sure correct string (GPRMC)
                  {
                    flag = 1;   //set flag to exit
                  }
            }
        }

    }

   //Translate GPS data into usable form by
   //converting to a string, then to a double, then storing.
   Position.lat = strtod(GPSdata.LAT,NULL);
   Position.lon = strtod(GPSdata.LON,NULL);
   Position.time = strtod(GPSdata.UTC_time,NULL);
   Position.date = strtod(GPSdata.Date,NULL);
   Position.course = strtod(GPSdata.Course_over_ground,NULL);
}

//****************************************************************************************************************

/*
 * Function: read_GPS_fields ()
 * Author: Warren Seely // Matthew Ferran
 * Date Created: 10/25/14
 * Date Last Modified: 10/26/14
 * Discription: Reads the RMC data string from the GPS and parses the latitude and longitude
 *              into a  struct for the GPS data.
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
void read_GPS_fields (char * address)
{
    char temp = '\0';
    int index = 0;

    while (index < 86)   //loop until end of struct
    {

        UART1ClearAllErrors (); //clear overflow errors
        if (DataRdyUART1()) //new data ready?
        {
            temp = U1RXREG; //get a character
            if(temp != ',') //if not the comma seperators between fields
            {
                *(address + index) = temp;  //load data into struct fields
                index++;    //increment
            }
            else    //if a comma is found, instead populate with NULL to make it a string
            {
                *(address + index) = '\0';  //assign a NULL to make string
                index++;    //increment
            }
        }

    }
}

//****************************************************************************************************************

/*
 * Function: print ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: 10/26/14
 * Discription: Prints text to the display screen depending on the value of choice
 * Input: Choice: integer that determines what to display
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
void print (int choice)
{
    if (choice == 0)
    {
        LCD_rst ();
        SpiChnPutS (1,(unsigned int*)"Begin Program",14);
        delay (5);
        LCD_rst ();
        SpiChnPutS (1,(unsigned int*)"Awaiting input for mode",24);
        delay (5);
        LCD_rst ();
        SpiChnPutS (1,(unsigned int*)"B1:Auto; B2:Man Both:Info",26);
    }
    else if (choice == 1)
    {
        LCD_rst ();
        SpiChnPutS (1,(unsigned int*)"Acquiring GPS \nSignal",20);
    }
    else if (choice == 2)
    {
        LCD_rst ();
        SpiChnPutS (1,(unsigned int*)"GPS Signal Acquired",20);
    }
//    else if (choice == 3)
//    {
//        //Nothing here yet.
//    }
}

//****************************************************************************************************************

/*
 * Function: LCD_rst ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: 10/26/14
 * Discription: Set and home display cursor
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
void LCD_rst (void)  //set and home display cursor
{
    //char enable[5] = {27, '[', '3', 'e', 0};  //enable command for display
    char set_cursor[] = {27, '[','1','c', '\0'};    //set cursor
    char home_cursor[] = {27, '[', 'j', '\0'};  //homes cursor
    //char wrap_line[5] = {27, '[', '0', 'h', 0};   //wrap line command
    

    SpiChnPutS (1,(unsigned int*)set_cursor,4);
    SpiChnPutS (1,(unsigned int*)home_cursor,3);

}

//****************************************************************************************************************

/*
 * Function: startup ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: 10/26/14
 * Discription: Choose a mode, check RF12 pin (?)
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
void startup (void)
{
    mode (); //choose auto/manual/info-loading mode
             //loop and check status of RF12 pin; if cylcing then wait until stays off before proceeding
             //eventually write in a print message for the LCD to let user know whats going on
    delay (5); //short delay to back away from robot
}

//****************************************************************************************************************

/*
 * Function: mode ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: 10/26/14
 * Discription: Select an functionality mode for the robot
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
void mode (void) //*****NOTE: ONLY WAY OUT IS EITHER AUTO MODE OR BOARD SHUTDOWN*****
{
    int state;  //declare
    while (1)
    {

        state = 0;  //initialize to 0
        while (state == 0)  //wait until a button is pressed
        {
            state = PORTRead (IOPORT_A) & 0xC0;  //current state of port A
        }

        if (state == 0x40)  //button 1 pressed(auto mode)
        {
            LCD_rst (); //reset screen
            SpiChnPutS (1,(unsigned int*)"Auto mode chosen",17);
            delay (2);   //wait for a bit
            LCD_rst ();  //reset screen
            SpiChnPutS (1,(unsigned int*)"Please stand back",18);
            delay (2);  //wait for a bit
            return; //auto mode chosen; continue program
        }
        else if (state == 0x80) //button 2 pressed(manual mode)
        {
            LCD_rst (); //rest screen
            SpiChnPutS (1,(unsigned int*)"Manual mode chosen",19);
            delay (2);   //wait for a bit
            manual ();   //maual mode function
            LCD_rst ();  //reset screen
            SpiChnPutS (1,(unsigned int*)"Select mode",12);
        }
        else if(state == 0xC0)  //both buttons pressed(info mode)
        {
            LCD_rst();  //reset screen
            SpiChnPutS (1,(unsigned int*)"Info mode chosen",17);
            delay (2);   //wait for a bit
            load_info ();   //load GPS boundary information into memory remotely with bluetooth
            LCD_rst (); //reset screen
            SpiChnPutS (1,(unsigned int*)"Select mode",12);
        }
    }
}

//****************************************************************************************************************

/*
 * Function: delay ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: ???
 * Discription: Delay everything by ~ i seconds
 * Input: number of seconds to delay
 * Returns: n/a
 * Preconditions: The program must be running
 * Postconditions: The program will continue running
 */
void delay (int i)
{
    i = i * 1000000;  //scale
    while (i > 0)
    {
        i--;    //decrement 1
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
 * Function: shut_down ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: ???
 * Discription: Stops the robot and shuts down booms
 * Input: n/a
 * Returns: n/a
 * Preconditions: The robot is running
 * Postconditions: The robot is no longer running
 */
void shut_down (void)    //stop robot, shut down booms
{
    PORTWrite (IOPORT_B, 0); //shut off robot motors
    //PORTWrite (IOPORT_A, 0);  //shut off other
    //Here incorporate remote message send to phone/command app
}

//****************************************************************************************************************

/*
 * Function: get_GPS_started ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: 10/26/14
 * Discription: If robot is at start point, skip,
 *              else plots course to get robot to start point
 * Input: n/a
 * Returns: n/a
 * Preconditions: The robot is somewhere
 * Postconditions: n/a
 */
void get_GPS_started (void)  //if robot is at location var1, skip. Else plot course to get to location var1
{
    int flag = 0,temp = 0;

    print(1);   //let user know GPS being acquired
    //may implement an interrupt here for the GPS instead. Enable it here?
    while (flag < 10000)    //wait for GPS to acquire signal; sampling a pulsing signal
    {
        temp = PORTRead (IOPORT_D) & 1 << 3;
        if (temp == 0)   //get current status of GPS
        {
            //delay (1);    //wait for some time
            flag++; //increment flag
        }
        else
        {
            flag = 0;   //reset flag
        }
    }
    print (2);  //let user know GPS acquired
    //delay (2);
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
    if ( (Position.lat == boundary.lat1) && 
         (Position.lon == boundary.lon1) ) //if current position is correct, skip function
    {
        return; //exit function
    }
    set_path (1); //need to compute path from current to var1 coordinates
    while ( (Position.lat != boundary.lat1) && 
            (Position.lon != boundary.lon1) ) //while not at start destination
    {
        correct_path (); //guide robot
        get_current_data (); //current lat/lon/heading
    }
}

//****************************************************************************************************************

/*
 * Function: manual ()
 * Author: Warren Seely
 * Date Created: ???
 * Date Last Modified: 10/26/14
 * Discription: Remote control mode (x: exit, w: forward, a: turn left, d: turn right,
 *                                   s: reverse, spacebar: stop)
 * Input: n/a
 * Returns: n/a
 * Preconditions: Manual mode is selected by the user
 * Postconditions: The robot can be controled by WASDX and SPACE
 */
 void manual (void) //remote control; utilizes BT2 to control robot movement keys w,a,d,z and spacebar control movement; x is exit manual mode
 {
    char choice = '\0';

    while (choice != 'x') //while choice is not to exit
    {
        if (DataRdyUART2()) //new data ready?
        {
            choice = U2RXREG; //copy to variable
           // U1TXREG = choice; //write to screen(optional)
        }

        if (choice == 'd') //right key
        {
            PORTWrite (IOPORT_B, 1<<10); //right
        }
        else if (choice == 'a') //left key
        {
            PORTWrite (IOPORT_B, 2<<10); //left
        }
        else if (choice == 'w') //forward key
        {
            PORTWrite (IOPORT_B, 3<<10); //forward
        }
        else if (choice == 's') //backward key
        {
            PORTWrite (IOPORT_B, 7<<10); //backward
        }
        else if (choice == 'x') //exit key
        {
            PORTWrite (IOPORT_B, 0); //stop
            //putsUART1 ("exit");
            SpiChnPutS (1, (unsigned int*)"exit", 5);
        }
        if (choice == ' ') //stop key
        {
            PORTWrite (IOPORT_B, 0); //stop
        }
    }
 }

 //****************************************************************************************************************

 void load_info(void) //get coordinates from BT2 module; x is exit info mode
 {                   
    char type[3] = {'\0'}, flag = '\0'; //temps to hold selection number
    int i = 1, pairnum = 0; //i set to 1 as default(longitude)
        
    while(flag != 'x') //while choice is not to exit
    {
        load_info_get_modify(&pairnum, type, &flag); //gets the coordinate pair to modify(1-9) and whether modifying latitude or longitude

        if (type[1] == 'a') //if "lat" entered, type[1] is an 'a'; if "lon" entered, type[1] is an 'o'
        {
            i = 0; //if latitude, set i to 0 and pass
        }

        load_coordinate(pairnum, i, &flag); //get the coordinate number and load into appropriate location in struct
     }

    putsUART2("Exiting load_info().\n"); //inform user eixting load_info()
 }

//****************************************************************************************************************

 void load_info_get_modify(int *pairnum, char type[], char *flag) //gets the coordinate pair, and if it is latitude or longitude
 {
     char pair = '\0';
     int i = 0;

     putsUART2("Which coordinate pair do you wish to enter(1-9). Enter x to exit.\n"); //write to terminal and prompt for data

    while (pair == '\0') // loop until something read into pair
    {
        if (DataRdyUART2())
         {
             pair = U2RXREG; //get coordinate pair to modify
             if (pair == 'x')
             {
                *flag = 'x';
                return; //exit helper
             }
         }
    }

     *pairnum = (pair - '0'); //send the pair number back as an integer

        putsUART2("Modifying latitude or longitude? (Enter lat or lon). Enter x to exit.\n"); //write to terminal and prompt for data

        while (type[2] == '\0') // loop until have "lat" or "lon"
        {
            if (DataRdyUART2())
             {
                 type[i] = U2RXREG; //get first char; latitude/longitude to modify
                 if (type[i] == 'x')
                 {
                    *flag = 'x';
                    return; //exit helper
                 }
                 i++; //increment array index
             }            
        }
 }

 //****************************************************************************************************************

void load_coordinate(int pairnum, int i, char *flag) //actually load the coordinate information
{
    int counter = 0;
    char temp[20] = {'\0'}, temp1 = '\0';

    pairnum = (pairnum - 1) * 2; //convert pairnum (from 1-9) to a number (from 0-17) to use as the struct address offset

    putsUART2("Enter the coordinate number followed by a comma(Maximum of 19 characters). Enter x to exit.\n"); //write to terminal and prompt for data

    do
    {
        if (DataRdyUART2()) //new data ready
        {
            temp1 = U2RXREG; //copy data to variable

            if (temp1 == 'x') //exit command
            {
                *flag = 'x'; //set exit flag
                return; //exit helper function
            }
            if ((temp1 >= '0') && (temp1 <= '9') || (temp1 == '.')) //read to array only if input is a number or period
            {
                temp[counter] = temp1; //get the data
                counter++; //increment array address
            }
        }
    }while(temp1 != ','); // End of number string signaled by a comma

    for (counter = 0; counter < 18; counter++) //loop to check which coordinate pair to load into
    {
        if (pairnum == counter) //found correct coordinate pair
        {
            //match counter to boundary.coordinate
           if (i == 0) //loading into latitude
           {
               *(&boundary.lat1 + counter) = atof(temp); //convert string to double and store in struct
           }
           else //loading into longitude
           {
               *(&boundary.lon1 + counter) = atof(temp); //convert string to double and store in struct
           }
        }
    }
}

 //****************************************************************************************************************

 double field_end(void)
 {
     int flag = 0, x, y;

     //if current position is not the same as the end coordinates of the current pass, return
     //otherwise continue(current position is the same as end coordinates of current pass)-> reached end
     if((Position.lat != pass.nav_to_lat) && (Position.lon != pass.nav_to_lon))
     {
         return;
     }

     if(pass.direction == 1)//if current direction is initial direction
     {
         int x = 0;
         PORTWrite(IOPORT_B, 2<<10);//right side off
         pass.D_heading += 90;//add 90 degrees
         while(x > 0)// loop until C_heading = D_heading + 90
         {
             get_current_data();//get current heading, mainly going for compass heading here
             x = pass.D_heading - Position.course;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue

         while(distance(flag) < width)//wait until traveled width
         {
         }

         pass.D_heading = pass.Secondary; //desired new heading is secondary heading(Secondary) 180 degrees off previous heading

         PORTWrite(IOPORT_B, 2<<10);//right side off

         while(x > 0)// loop until C_heading = MAS_hed1
         {
             get_current_data();//get current heading, mainly going for compass heading here
             x = pass.D_heading - Position.course;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue
         pass.direction = 0; //update direction
     }
     else//current direction is secondary direction
     {
         int x = 0;
         PORTWrite(IOPORT_B, 1<<10);//left side off
         pass.D_heading -= 90;//subtract 90 degrees

         while(x > 0)// loop until course = D_heading - 90
         {
             get_current_data();//get current heading, mainly going for compass heading here
             x = pass.D_heading - Position.course;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue
         flag = 0;//reset flag
         while(distance(flag) < width)//wait until traveled width
         {
         }

         //update the coordinates for the field end points(Simplified, assuming a square field)

         x = Position.lat - pass.nav_to_lat + pass.nav_from_lat;//the new latitude coordinate to navigate to
         y = Position.lon - pass.nav_to_lon + pass.nav_from_lon; //the new longitude coordinate to navigate to

         //load updated line coordinates into the "current pass" struct
         pass.nav_from_lat = Position.lat;//latitude traveling from
         pass.nav_from_lon = Position.lon;//longitude traveling from
         pass.nav_to_lat = x; //latitude traveling to
         pass.nav_to_lon = y; //longitude traveling to
         
         pass.D_heading = pass.Master; //desired new heading is primary heading(Master) 180 degrees off previous heading

         PORTWrite(IOPORT_B, 1<<10);//left side off

         while(x > 0)// loop until course = desired heading
         {
             get_current_data();//get current heading, mainly going for compass heading here
             x = pass.D_heading - Position.course;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue
         pass.direction = 1; //update direction
     }
 }


 double distance(int flag)//computes distance from initial call until current call; set flag = 0 before initial call;
 {                          //used with loop, returns distance
     double c1, c2 = 0, clat2 = 0, clon2 = 0, d = 0;//temps for latitude, longitude, distance

     if(flag == 0)//first time, copy current(start) location and get start coordinate
     {
        pass.clat1 = Position.lat;//copy Position.lat, Position.lon; preserve
        pass.clon1 = Position.lon;
     }

     get_current_data();//get current position
     clat2 = Position.lat;//copy Position.lat, Position.lon
     clon2 = Position.lon;

     c1 = pow((pass.clat1 - clat2), 2);//get "x" coordinates
     c2 = pow((pass.clon1 - clon2), 2);//get "y" coordinates

     d = sqrt(c1 + c2);//current distance
     flag = 1;//set flag

     return d;//current distance
 }
