/*
#// ***************************************************************************************************************************
#// *                                                                                                                         *
#// *                        THIS FILE PERFORMS ROBOT HARDWARE COMMANDS                                                *
#// *                                                                                                                         *
#// ***************************************************************************************************************************

#//*********INFORMATION**********************************************************************************************
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
char get_current_data (void) //gets current lat/lon/speed and heading
{
    GPS_DATA_T GPSdata;  //Struct to read the RMC data into

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
   Position.lat = strtod(GPSdata.LAT,NULL)/100;
   Position.lon = strtod(GPSdata.LON,NULL)/100;
   Position.time = strtod(GPSdata.UTC_time,NULL);
   Position.date = strtod(GPSdata.Date,NULL);
   Position.course = strtod(GPSdata.Course_over_ground,NULL);
   temp = GPSdata.STATUS[0]; //copy status to temp
   return temp; //used during startup to determine if connected
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

    while (index < 88)   //loop until end of struct
    {

        UART1ClearAllErrors (); //clear overflow errors
        if (DataRdyUART1()) //new data ready?
        {
            temp = U1RXREG; //get a character
            if ((temp != ',') && (temp != '-')) //if not the comma seperators between fields
            {
                *(address + index) = temp;  //load data into struct fields
                index++;    //increment
            }
            else if (temp == ',')   //if a comma is found, instead populate with NULL to make it a string
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
    char temp;
    if (choice == 0)
    {
        LCD_rst ();
        SpiChnPutS (1,(unsigned int*)"Begin Program",14);
        delay (5);
        LCD_rst ();
        SpiChnPutS (1,(unsigned int*)"Awaiting input for mode",23);
        delay (5);
        LCD_rst ();
        SpiChnPutS (1,(unsigned int*)"B1:Auto; B2:Man Both:Info",25);
    }
    else if (choice == 1)
    {
        LCD_rst ();
        SpiChnPutS (1,(unsigned int*)"Acquiring GPS Signal",20);
    }
    else if (choice == 2)
    {
        LCD_rst ();
        SpiChnPutS (1,(unsigned int*)"GPS Signal Acquired",19);
    }
    else if (choice == 3)
    {
        LCD_rst ();
        SpiChnPutS (1, (unsigned int*)"Pass width:", 11);
        temp = boundary.width + '0'; //convert width to a char to write
        //SpiChnWriteC (1,(unsigned char*)temp); //write the current width setting of the robot to the display screen
        SpiChnPutS (1,(unsigned int*)"GPS Signal Acquired",19);
    }
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
        delay(3); //short delay to give time to press both buttons
        state =PORTRead (IOPORT_A) & 0xC0; //read the buttons
        switch (state)
        {
            case 0x40: // Button 1 pressed(auto mode)
                LCD_rst (); //reset screen
                SpiChnPutS (1,(unsigned int*)"Auto mode chosen",17);
                delay (2);   //wait for a bit
                LCD_rst ();  //reset screen
                SpiChnPutS (1,(unsigned int*)"Please stand back",18);
                delay (2);  //wait for a bit
                return; //auto mode chosen; continue program
            break;
            
            case 0x80: // Button 2 pressed(manual mode)
                LCD_rst (); //rest screen
                SpiChnPutS (1,(unsigned int*)"Manual mode chosen",19);
                delay (2);   //wait for a bit
                manual ();   //manual mode function
                LCD_rst ();  //reset screen
                SpiChnPutS (1,(unsigned int*)"Select mode",12);
            break;
            
            case 0xC0: //Both buttons pressed(info mode)
                LCD_rst();  //reset screen
                SpiChnPutS (1,(unsigned int*)"Info mode chosen",17);
                delay (2);   //wait for a bit
                load_info ();   //load GPS boundary information into memory remotely with bluetooth
                LCD_rst (); //reset screen
                SpiChnPutS (1,(unsigned int*)"Select mode",12);
            break;
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
    PORTWrite (IOPORT_E, 0);  //shut off boom nozzels
    //Here incorporate remote message send to phone/command app
}

//****************************************************************************************************************

/*
 * Function: manual ()
 * Author: Warren Seely // Matthew Ferran
 * Date Created: ???
 * Date Last Modified: 11/4/14
 * Discription: Remote control mode (x: exit, w: forward, a: turn left, d: turn right,
 *                                   s: reverse, spacebar: stop, r: record gps data)
 * Input: n/a
 * Returns: n/a
 * Preconditions: Manual mode is selected by the user
 * Postconditions: The robot can be controled by WASDXR and SPACE
 */
 void manual (void) //remote control; utilizes BT2 to control robot movement keys w,a,d,z and spacebar control movement; x is exit manual mode
 {
    char choice = '\0';
    int flag = 0;
    unsigned int motors = 0;

    while (choice != 'x') //while choice is not to exit
    {
        if (DataRdyUART2()) //new data ready?
        {
            choice = U2RXREG; //copy to variable

            switch (choice)
            {

                case 'd': //right key
                    motors = 1 << 10; //right
                    break;

                case 'a': //left key
                    motors = 2 << 10; //left
                    break;

                case 'w': //forward key
                    motors = 3 << 10; //forward
                    break;

                case 's': //backward key
                    motors = 7 << 10; //backward
                    break;

                case 'x': //exit key
                case ' ': //stop key
                    motors = 0; //stop
                    break;

                case 'r': //record current data key
                    traverse_boundary (flag); //get data
                    flag++;
                    break;

            }
            PORTWrite (IOPORT_B, motors); //issue command to board
//            if (choice == 'd') //right key
//            {
//                PORTWrite (IOPORT_B, 1<<10); //right
//            }
//            else if (choice == 'a') //left key
//            {
//                PORTWrite (IOPORT_B, 2<<10); //left
//            }
//            else if (choice == 'w') //forward key
//            {
//                PORTWrite (IOPORT_B, 3<<10); //forward
//            }
//            else if (choice == 's') //backward key
//            {
//                PORTWrite (IOPORT_B, 7<<10); //backward
//            }
//            else if (choice == 'x') //exit key
//            {
//                PORTWrite (IOPORT_B, 0); //stop
//                //putsUART1 ("exit");
//                SpiChnPutS (1, (unsigned int*)"exit", 5);
//            }
//            else if (choice == 'r') //read data from GPS
//            {
//                traverse_boundary(flag); //record current position in boundary struct
//                flag++;
//            }
//            else if (choice == ' ') //stop key
//            {
//                PORTWrite (IOPORT_B, 0); //stop
//            }
        }
    }
 }

//****************************************************************************************************************

 /*
 * Function: load_info ()
 * Author: Warren Seely
 * Date Created: 10/29/14
 * Date Last Modified: 10/29/14
 * Discription: Get's coordinates from BT2 module; x is exit info mode
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
 void load_info (void) //get coordinates from BT2 module; x is exit info mode
 {                   
    char type[3] = {'\0'}, flag = '\0'; //temps to hold selection number
    int i = 0, pairnum = 0; //i set to 1 as default(latitude)
        
    do
    {
        load_info_get_modify(&pairnum, type, &flag); //gets the coordinate pair to modify(1-9) and whether modifying latitude or longitude
        if (flag == 'x')
        {
            break;
        }
        else if (flag == 'w') //load into robot width
        {
            i = -1; //set flag
        }
        if (type[1] == 'a') //if "lat" entered, type[1] is an 'a'; if "lon" entered, type[1] is an 'o'
        {
            i = 0; //if latitude, set i to 0 and pass
        }
        else
        {
            i = 1; //Only other option is longitude
        }

        load_coordinate(pairnum, i, &flag); //get the coordinate number and load into appropriate location in struct
     }while(flag != 'x'); //while choice is not to exit

    putsUART2("\n\rExiting load_info().\n"); //inform user eixting load_info()
 }

//****************************************************************************************************************

 /*
 * Function: load_info_get_modify ()
 * Author: Warren Seely
 * Date Created: 10/29/14
 * Date Last Modified: 10/29/14
 * Discription: Gets coordinate pair number(1-9), and whether latitude or longitude from BT2 module; x is exit info mode
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */
 void load_info_get_modify (int *pairnum, char type[], char *flag) //gets the coordinate pair, and if it is latitude or longitude
 {
     char pair = '\0';
     int i = 0;

     putsUART2("\n\n\rWhich coordinate pair do you wish to enter(1-9)? Enter 'w' for robot width, enter x to exit.\t> "); //write to terminal and prompt for data

    while ((pair == '\0') || ((pair < '1') && (pair > '9'))) // loop until something read into pair
    {
        if (DataRdyUART2())
         {
             pair = U2RXREG; //get coordinate pair to modify
             if (pair == 'x')
             {
                 *flag = 'x';
                 return; //exit helper
             }
             else if (pair == 'w')
             {
                 *flag = 'w';
                 return;
             }
         }
    }

     *pairnum = (pair - '0'); //send the pair number back as an integer

     putsUART2("\n\n\rModifying latitude or longitude? (Enter lat or lon). Enter x to exit.\t> "); //write to terminal and prompt for data

     do
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
              if ((i == 3) && (type[0] != 'l') && ((type[1] != 'a') || (type[1] != 'o')) && ((type[2] != 'n') || (type[2] != 't')))
              {
                  type[2] = '\0';
                  i = 0; //reset i
                  putsUART2("\n\n\rModifying latitude or longitude? (Enter lat or lon). Enter x to exit.\t> "); //write to terminal and prompt for data

              }
         }
     }while (type[2] == '\0'); // loop until have something
 }

//****************************************************************************************************************

 /*
 * Function: load_coordinate ()
 * Author: Warren Seely
 * Date Created: 10/29/14
 * Date Last Modified: 10/29/14
 * Discription: loads the coordinate number from BT2 module; x is exit info mode
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */

void load_coordinate (int pairnum, int i, char *flag) //actually load the coordinate information
{
    int counter = 0;
    char temp[20] = {'\0'}, temp1 = '\0';

    pairnum = (pairnum - 1) * 2; //convert pairnum (from 1-9) to a number (from 0-17) to use as the struct address offset

    putsUART2("\n\n\rEnter the number followed by a comma(Maximum of 19 characters). Enter x to exit.\t> "); //write to terminal and prompt for data

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

    if (i == -1) //If i is -1, know to load the width
    {
        boundary.width = atoi(temp); //load into width
        return;
    }

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

/*
 * Function: traverse_boundary ()
 * Author: Warren Seely
 * Date Created: 10/30/14
 * Date Last Modified: 10/30/14
 * Discription: //for gathering the boundary coordinates by driving around the boundary.
                //current position stored in boundary every time 'r' is hit
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */

 void traverse_boundary (int flag)
 {
     if (flag == 9) //check to see if 9 pairs read in already
     {
         SpiChnPutS(1,(unsigned int*)"Boundary full.", 14);
         return;
     }
     flag = flag * 2; //change offset from (0-9) to (0-17)
     get_current_data (); //get the position data for the robot
     *(&boundary.lat1 + flag) = Position.lat; //update the current latitude and load into boundary
     *(&boundary.lon1 + flag) = Position.lon; //update the current longitude and load into boundary
 }

//****************************************************************************************************************

/*
 * Function: sleep ()
 * Author: Warren Seely
 * Date Created: 10/30/14
 * Date Last Modified: 10/30/14
 * Discription: wait in here until a button pressed. "Sleeping"
 * Input: n/a
 * Returns: n/a
 * Preconditions: n/a
 * Postconditions: n/a
 */

 void sleep (void)
 {
     do
    {

    } while((PORTRead(IOPORT_A) & 0xC0) == 0); //loop until someone wakes robot again by pressing a button

 }

//****************************************************************************************************************
