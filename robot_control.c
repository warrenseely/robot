/*
 #// ***************************************************************************************************************************
#// *                                                                                                                         *
#// *                        THIS FILE PERFORMS ROBOT MOVEMENT COMMANDS                                                *
#// *                                                                                                                         *
#// ***************************************************************************************************************************

#//*********INFORMATION**********************************************************************************************
#//redo correct_path so works on all lines every width-->create move[4] for storing lat/lon every width across area
#//load_info set up for BT2, in port JH(uart)
#//GPS set up in port JH(uart) Modify accordingly when moved to port JE-->display screen moved
#//
#//look into sensors; light, IR, touch
#//look into robot tracking(in case robot stolen)
#//move display screen to SPI1
#//******************************************************************************************************************
 */

#include "header.h"
#include <plib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//takes readings from (C_heading, D_heading) and keeps robot on correct path
void correct_path()
{
    int x = 0, y = 0, z = 0;

    x = boundary[0] - C_pos_Lat; //difference in the latitudes
    if(x<0)
    {
        x = x*(-1);//make sure x is positive
    }

    y = boundary[1] - C_pos_Lon;//difference in the longitudes
    if(y<0)
    {
        y = y*(-1);//make sure y is positive
    }

    z = atan(x/y); //get heading, use this to compare to north and desired heading 
    z = (180/3.14159)*z;//converto to degrees
    if(direction == 1)//if current direction is 1, initial heading to process left/right commands
    {
        x = D_heading - z;//subtract heading here to compare

        if(x < 0)//go right
        {
            PORTWrite(IOPORT_B, 2<<10); //right side off, left side on
            delay(5); //time to turn
            PORTWrite(IOPORT_B, 3<<10); //both back on
        }
        else//go left
        {
            PORTWrite(IOPORT_B, 1<<10); //left side off, right side on
            delay(5); //time to turn
            PORTWrite(IOPORT_B, 3<<10); //both back on
        }
    }
    else//direction is 0
    {
        x = D_heading - z;//subtract heading here to compare

        if(x < 0)//go left
        {
            PORTWrite(IOPORT_B, 1<<10); //left side off, right side on
            delay(5); //time to turn
            PORTWrite(IOPORT_B, 3<<10); //both back on
        }
        else//go right
        {
            PORTWrite(IOPORT_B, 2<<10); //right side off, left side on
            delay(5); //time to turn
            PORTWrite(IOPORT_B, 3<<10); //both back on
        }
    }
}

//****************************************************************************************************************

double set_path(int flag)//sets path between 2 coordinate pairs for robot to follow
{
    int x = 0, y = 0, z = 0;
    if(flag == 0)//area heading
    {
        x = boundary[0] - boundary[2]; //difference in the latitudes
        y = boundary[1] - boundary[3];//difference in the longitudes
    }
    else if(flag == 1)//to/from area heading
    {
        x = boundary[0] - C_pos_Lat;//difference in the latitudes
        y = boundary[0] - C_pos_Lon;//difference in the longitudes
    }
    
    if(x<0)
    {
        x = x*(-1);//make sure x is positive
    }   
    if(y<0)
    {
        y = y*(-1);//make sure y is positive
    }

    z = atan(x/y); //get heading, use this to compare to north and desired heading -->****IN RADIANS RIGHT NOW****
    z = (180/3.14159)*z;//convert to degrees

    MAS_head = z;//master heading
    if(flag == 1)//to/from area heading
    {
        return;//done in this function, exit
    }
    z = MAS_head - 180;//if less than or equal 0, know add 180 to get degrees for opposite direction

    if(z <= 0)//add 180 to get opposite direction
    {
        MAS_head1 = MAS_head + 180;
    }
    else//difference is greater than 0, so subtract 180 to get opposite direction
    {
        MAS_head1 = z;
    }

}

//****************************************************************************************************************

void get_current_data()//gets current lat/lon/speed and heading
{
    int z = 1, i = 1, y = 0, flag = 0, w;//local variables
    char temp[110] = {'\0'}, temp1, temp3[20] = {'\0'};//to store GPS coordinates to

   while(z = 1)//loop while flag true
   {

      if(DataRdyUART2())//is new data ready?
      {
          temp1 = U2RXREG;//copy
          if(temp1 == '$')//begin of GPS String
          {
              temp[0] = temp1;//temp[0] = '$', beginning of string
              while((temp[i] != '<') || (i < 90)) //end of string indicator or end of array
              {
                  if(DataRdyUART2())//is new data ready?
                  {
                    temp[i] = U2RXREG;//read chars into temp
                    i++;//increment
                  }
                    z = 0;//set exit flag
              }
           }
      }
   }

    while(flag < 4)//loop 4 times to get C_pos_Lat, C_pos_Lon, C_heading, speed
    {

        i = 0;//reset i
        z = 0;//reset z
        y = 0;//reset y

        if(flag == 0)//start
        {
            w = 3;//find start of first wanted string
        }
        else if(flag == 1)//second string
        {
            w = 5;//find start of second wanted string
            sscanf(temp3, "%lf", &C_pos_Lat);//convert to double and store in C_pos_Lat
        }
        else if(flag == 2)//third string
        {
            w = 7;//find start of third wanted string
            sscanf(temp3, "%lf", &C_pos_Lon);//convert to double and store in C_pos_Lon
        }
        else if(flag == 3)//fourth string
        {
            w = 8;//find start of fourth wanted string
            sscanf(temp3, "%lf", &speed);//convert to double and store in C_heading
        }
        for(i = 0; i < 20; i++)//reset temp3
        {
            temp3[i] = '\0';
        }
        i = 0;
        while(z < w)//find start of first wanted char string
        {
            if((temp[i] == ',') && (temp[i] != NULL))//comma or end of string
            {
                z++;//increment z
            }
            i++;//increment
        }
        while((temp[i] != ',') && (temp[i] != NULL))//transfer wanted string to temp3, loop until comma or end of string
        {
            temp3[y] = temp[i];//copy wanted string
            y++;//increment
            i++;
        }

        flag++;//increment flag
   }
   sscanf(temp3, "%lf", &C_heading);//convert to double and store in C_heading

}

//****************************************************************************************************************

void print(int choice)
{
   
    if(choice == 0)
    {
        LCD_rst();
        putsUART1("Begin Program");
        delay(10);
        LCD_rst();
        putsUART1("Awaiting Input for Mode");
        delay(10);
        LCD_rst();
        putsUART1("B1:Auto; B2:Man Both:Info");
    }
    else if(choice == 1)
    {
        LCD_rst();
        putsUART1("Acquiring GPS Signal");
        //delay(10);
    }
    else if(choice == 2)
    {
        LCD_rst();
        putsUART1("GPS Signal Acquired");
        delay(10);
    }
    else if(choice == 3)
    {

    }
}

//****************************************************************************************************************

void LCD_rst()//set and home display cursor
{
    int enable[5] = {27, '[', '3', 'e', 0};//enable command for display
    int set_cursor[5] = {27, '[','1','c', 0};//set cursor
    int home_cursor[4] = {27, '[', 'j', 0};//homes cursor
    int wrap_line[5] = {27, '[', '0', 'h', 0};//wrap line command
    int i = 0, z = -1;

    while(z != 0)//wait until all done
        {
            z = set_cursor[i]; //copy
            //z += 48;// convert to ascii char; MUST be in ascii char to print
            U1TXREG = z; //write it
            while(BusyUART1()); // Wait until UART2 module is not busy
            i++; //increment
        }
        z = -1;
        i = 0;
        while(z != 0)//wait until all done
        {
            z = home_cursor[i]; //copy
            //z += 48;// convert to ascii char; MUST be in ascii char to print
            U1TXREG = z; //write it
            while(BusyUART1()); // Wait until UART2 module is not busy
            i++; //increment
        }
}

//****************************************************************************************************************

void startup()
{
    mode(); //choose auto/manual/info-loading mode
            //loop and check status of RF12 pin; if cylcing then wait until stays off before proceeding
            //eventually write in a print message for the LCD to let user know whats going on
    delay(20); //short delay to back away from robot
}

//****************************************************************************************************************

void mode()//*****NOTE: ONLY WAY OUT IS EITHER AUTO MODE OR BOARD SHUTDOWN*****
{
    int state;//declare
top:

    state = 0;//initialize to 0
    while(state == 0)//wait until a button is pressed
    {
        state = PORTRead(IOPORT_A) & 0xC0;//current state of port A
    }

    if(state == 0x40)//button 1 pressed(auto mode)
    {
        LCD_rst();//reset screen
        putsUART1("Auto Mode Chosen");//print this
        delay(10);//wait for a bit
        LCD_rst();//reset screen
        putsUART1("Please Stand Back");//print this
        delay(12);//wait for a bit
        return;//auto mode chosen; continue program
    }
    else if(state == 0x80)//button 2 pressed(manual mode)
    {
        LCD_rst();//rest screen
        putsUART1("Manual Mode Chosen");//print this
        //delay(10);//wait for a bit
        manual();//maual mode function
        LCD_rst();//reset screen
        putsUART1("Select Mode");//let user know to select new mode
        goto top;//loop back up to choose mode again
    }
    else if(state == 0xC0)//both buttons pressed(info mode)
    {
        LCD_rst();//reset screen
        putsUART1("Info Mode Chosen");//print this
        //delay(10);//wait for a bit
        load_info();//load GPS boundary information into memory remotely with bluetooth
        LCD_rst();//reset screen
        putsUART1("Select Mode");//let user know to select new mode
        goto top;//loop back up to choose mode again
    }
}

//****************************************************************************************************************

void delay(int i)
{
    i = i*1000000;//scale
    while(i > 0)
    {
        i--;//decrement 1
    }
}

//****************************************************************************************************************

int job_done()
{
    int i = 0;

    while(boundary[i] != 0)//end of boundary coordinates
    {
        i = i + 2;//only looking at the latitudes
    }
    if((C_pos_Lat == boundary[i-2]) && (C_pos_Lon == boundary[i-1]))//if current position is end of boundary coordinates
    {
        shut_down();
        return 1;//job is done
    }
    return 0;//job is not done
}

//****************************************************************************************************************

void shut_down()//stop robot, shut down booms
{
    PORTWrite(IOPORT_B, 0);//shut off robot motors
    //PORTWrite(IOPORT_A, 0);//shut off other
    //Here incorporate remote message send to phone
}

//****************************************************************************************************************
//if robot is at location var1, skip. Else plot course to get to location var1
void get_started()
{
    get_current_data();//current lat/lon/heading
    if((C_pos_Lat == boundary[0]) && (C_pos_Lon == boundary[1]))//if current position is correct, skip function
    {
        return;//exit function
    }
    set_path(1);//need to compute path from current to var1 coordinates
    while((C_pos_Lat != boundary[1]) && (C_pos_Lon != boundary[1]))//while not at start destination
    {
        correct_path();//guide robot
        get_current_data();//current lat/lon/heading
    }
}

//****************************************************************************************************************

 void manual()//remote control; utilizes BT2 to control robot movement keys w,a,d,z and spacebar control movement; x is exit manual mode
 {
     char choice;

     while(choice != 'x')//while choice is not to exit
    {
        if(DataRdyUART2())//new data ready?
        {
            choice = U2RXREG;//copy to variable
           // U1TXREG = choice;//write to screen(optional)
        }

        if(choice == 'd')//right key
        {
            PORTWrite(IOPORT_B, 1<<10);//right
        }
        else if(choice == 'a')//left key
        {
            PORTWrite(IOPORT_B, 2<<10);//left
        }
        else if(choice == 'w')//forward key
        {
            PORTWrite(IOPORT_B, 3<<10);//forward
        }
        else if(choice == 'z')//backward key
        {
            PORTWrite(IOPORT_B, 7<<10);//backward
        }
        else if(choice == 'x')//exit key
        {
            PORTWrite(IOPORT_B, 0);//stop
            putsUART1("exit");
        }
        if(choice == ' ')//stop key
        {
            PORTWrite(IOPORT_B, 0);//stop
        }
    }
 }

 //****************************************************************************************************************

 void load_info(void)//get coordinates from BT2 module; x is exit info mode
 {                   //numbers 1-9 are the latitudes, letters a-i are the longitdes denote the coordinates to choose
     char info, temp[20] = {'\0'};//temps to hold GPS chars and selection number
    int i = 0;
    double temp1;

     while(info != 'x')//while choice is not to exit
    {
        if(DataRdyUART2())//new data ready?
        {
            info = U2RXREG;//copy to variable
           // U1TXREG = choice;//write to screen(optional)
        }
        if(info == '1')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[0] = temp1;
            i = 0;//reset i
        }
        else if(info == 'a')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[1] = temp1;
            i = 0;//reset i
        }
        else if(info == '2')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[2] = temp1;
            i = 0;//reset i
        }
        else if(info == 'b')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[3] = temp1;
            i = 0;//reset i
        }
        else if(info == '3')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[4] = temp1;
            i = 0;//reset i
        }
        else if(info == 'c')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[5] = temp1;
            i = 0;//reset i
        }
        else if(info == '4')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[6] = temp1;
            i = 0;//reset i
        }
        else if(info == 'd')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[7] = temp1;
            i = 0;//reset i
        }
        else if(info == '5')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[8] = temp1;
            i = 0;//reset i
        }
        else if(info == 'e')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[9] = temp1;
            i = 0;//reset i
        }
        else if(info == '6')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[10] = temp1;
            i = 0;//reset i
        }
        else if(info == 'f')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[11] = temp1;
            i = 0;//reset i
        }
        else if(info == '7')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[12] = temp1;
            i = 0;//reset i
        }
        else if(info == 'g')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[13] = temp1;
            i = 0;//reset i
        }
        else if(info == '8')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[14] = temp1;
            i = 0;//reset i
        }
        else if(info == 'h')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[15] = temp1;
            i = 0;//reset i
        }
        else if(info == '9')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[16] = temp1;
            i = 0;//reset i
        }
        else if(info == 'i')//var1lat
        {
            do
            {
                if(DataRdyUART2())//new data ready?
                {
                        info = U2RXREG;//copy to variable
                        if(((info >= '0') && (info <= '9')) || (info == '.'))//make sure only numbers and decimal points
                        {
                            temp[i++] = info;//copy to array
                        }
                }
            }while(info != ',');//loop until read in seperator

            sscanf(temp, "%lf", &temp1);//convert to double number and store
            boundary[17] = temp1;
            i = 0;//reset i
        }
     }

 }

 //****************************************************************************************************************

 double field_end()
 {
     int flag = 0;
     if(direction == 1)//if current direction is initial direction
     {
         int x = 0;
         PORTWrite(IOPORT_B, 2<<10);//right side off
         D_heading += 90;//add 90 degrees
         while(x > 0)// loop until C_heading = D_heading + 90
         {
             get_current_data();//get current heading, mainly going for compass heading here
             x = D_heading - C_heading;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue

         while(distance(flag) < width)//wait until traveled width
         {
         }

         D_heading = MAS_head1; //desired new heading is secondary heading(MAS_head1) 180 degrees off previous heading

         PORTWrite(IOPORT_B, 2<<10);//right side off

         while(x > 0)// loop until C_heading = MAS_hed1
         {
             get_current_data();//get current heading, mainly going for compass heading here
             x = D_heading - C_heading;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue
         direction = 0; //update direction
     }
     else//current direction is secondary direction
     {
         int x = 0;
         PORTWrite(IOPORT_B, 1<<10);//left side off
         D_heading -= 90;//subtract 90 degrees
         while(x > 0)// loop until C_heading = D_heading - 90
         {
             get_current_data();//get current heading, mainly going for compass heading here
             x = D_heading - C_heading;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue
         flag = 0;//reset flag
         while(distance(flag) < width)//wait until traveled width
         {
         }

         D_heading = MAS_head; //desired new heading is secondary heading(MAS_head1) 180 degrees off previous heading

         PORTWrite(IOPORT_B, 1<<10);//left side off

         while(x > 0)// loop until C_heading = MAS_hed
         {
             get_current_data();//get current heading, mainly going for compass heading here
             x = D_heading - C_heading;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue
         direction = 1; //update direction
     }
 }


 double distance(int flag)//computes distance from initial call until current call; set flag = 0 before initial call;
 {                          //used with loop, returns distance
     double c1, c2 = 0, clat2 = 0, clon2 = 0, d = 0;//temps for latitude, longitude, distance
     
     if(flag == 0)//first time, copy current(start) location and get start coordinate
     {
        clat1 = C_pos_Lat;//copy C_pos_Lat, C_pos_Lon; preserve
        clon1 = C_pos_Lon;
     }

     get_current_data();//get current position
     clat2 = C_pos_Lat;//copy C_pos_Lat, C_pos_Lon
     clon2 = C_pos_Lon;

     c1 = pow((clat1 - clat2), 2);//get "x" coordinates
     c2 = pow((clon1 - clon2), 2);//get "y" coordinates

     d = sqrt(c1 + c2);//current distance
     flag = 1;//set flag

     return d;//current distance
 }
