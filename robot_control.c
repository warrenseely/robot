/*
 #// ***************************************************************************************************************************
#// *                                                                                                                         *
#// *                        THIS FILE PERFORMS ROBOT MOVEMENT COMMANDS                                                *
#// *                                                                                                                         *
#// ***************************************************************************************************************************

#//*********INFORMATION**********************************************************************************************
#// lines 276 and 306 in field_end need distance set up
#//Look into using the BT2 module to load the coordinates into the program rather than manual input--> load_info
#//modify job_done to check for last full var to use as final coordinate
#//Look into board shutoff command for shut_down
#//look into robot tracking(in case robot stolen)
#//******************************************************************************************************************
 */

#include "header.h"

//takes readings from (C_heading, D_heading) and keeps robot on correct path
double correct_path(double *D_heading, double *C_pos_Lat, double *C_pos_Lon, int var1lat, int var1lon, int direction)
{
    int x = 0, y = 0, z = 0;

    x = var1lat - *C_pos_Lat; //difference in the latitudes
    if(x<0)
    {
        x = x*(-1);//make sure x is positive
    }

    y = var1lon - *C_pos_Lon;//difference in the longitudes
    if(y<0)
    {
        y = y*(-1);//make sure y is positive
    }

    z = atan(x/y); //get heading, use this to compare to north and desired heading -->****IN RADIANS RIGHT NOW****

    if(direction == 1)//if current direction is 1, jump to initial heading to process left/right commands
    {
        x = *D_heading - z;//subtract heading here to compare

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
        x = *D_heading - z;//subtract heading here to compare

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

double set_path(int var1lat, int var1lon, int var2lat, int var2lon, double *MAS_head, double *MAS_head1)
{
    int x = 0, y = 0, z = 0;

    x = var1lat - var2lat; //difference in the latitudes
    if(x<0)
    {
        x = x*(-1);//make sure x is positive
    }

    y = var1lon - var2lon;//difference in the longitudes
    if(y<0)
    {
        y = y*(-1);//make sure y is positive
    }

    z = atan(x/y); //get heading, use this to compare to north and desired heading -->****IN RADIANS RIGHT NOW****

    *MAS_head = z;//master heading

    z = *MAS_head - 180;//if less than or equal 0, know add 180 to get degrees for opposite direction

    if(z <= 0)//add 180 to get opposite direction
    {
        *MAS_head1 = *MAS_head + 180;
    }
    else//difference is greater than 0, so subtract 180 to get opposite direction
    {
        *MAS_head1 = z;
    }
}

//****************************************************************************************************************

void get_current_data(double *C_pos_Lat, double *C_pos_Lon, double *C_heading)
{
    int z = 1, i = 0;
    char temp[160];


    while(z = 1)
   {

//      if(DataRdyUART2())
  //    {
          if(U2RXREG == '$')
          {
              while((U2RXREG != '<') || (i < 160))
              {
                  temp[i] = U2RXREG;
                  i++;
              }
              //getsUART2(160, temp, 5000);
              z = 0;
         // }
      }
   }

  z++;
  /*  while(temp != -1)
    {
        temp = DataRdyUART2();//checks to see if any new data is ready
        if(temp == 1)//new data
        {
            temp = 0;//reset temp
            temp2 = U2RXREG;//get current char
            if(temp2 == '$')//start of sequence
            {
                while(string[i] != '<')//loop until end sequence detected
                {
                    temp = DataRdyUART2();//checks to see if any new data is ready
                    if(temp == 1)
                    {
                        string[i] = U2RXREG;//store sequence char
                        i++;//increment storage location
                    }
                }
                temp = -1;
            }
        }
    }*/

    /*do//wait until string begins
    {
        temp = U2RXREG;//current transmitted char
        temp = temp;
    }while(temp != '$');//look for $

    while(string[i] != '\0')//loop until end of string
    {
        string[i] = U2RXREG;//data to string array slots
        while(BusyUART2());//wait until ready
    }*/
    //separate here; look for commas and leading $; put in variables
   /* num = strtok(string, ",");//get pointers for different chunks
    i = 0;
    while(num != NULL)//loop until end of string
    {
        string2[i] = strtok(NULL, ",");//split into separate string and store in string2
    }*/
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

void mode()
{
    int state = 0, temp = 0, z = 0;

    while(state == 0)//wait until a button is pressed
    {
        state = PORTRead(IOPORT_A) & 0xC0;//current state of port A
    }

    if(state == 0x40)//button 1 pressed(auto mode)
    {
        LCD_rst();//reset screen
        putsUART1("Auto Mode Chosen");//print this
        delay(10);//wait for a bit
        print(1);//print status message

       /* while(z < 50000)
        {
            temp = PORTRead(IOPORT_F) & 0x2000;//current state
            
            if(((PORTRead(IOPORT_F) & 0x2000) != 0) || (temp != 0))//if one is not 0, reset counter
            {
                z = 0;//reset z
            }
        }*/

     // while((PORTRead(IOPORT_F) & 1<<12) != 0); //here write sequence for checking RF 12 which cycles once persecond
        print(2);//print status message
        LCD_rst();
        putsUART1("Please Stand Back");//print this
        delay(12);//wait for a bit
        return;//auto mode chosen; continue program
    }
    else if(state == 0x80)//button 2 pressed(manual mode)
    {
        LCD_rst();//rest screen
        putsUART1("Manual Mode Chosen");//print this
        delay(10);//wait for a bit
        manual();//maual mode function
    }
    else if(state == 0xC0)//both buttons pressed(info mode)
    {
        LCD_rst();//reset screen
        putsUART1("Info Mode Chosen");//print this
        delay(10);//wait for a bit
        load_info();
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

void job_done(double C_pos_Lat, double C_pos_Lon, int var9lat, int var9lon)
{
    if((C_pos_Lat == var9lat) && (C_pos_Lon == var9lon))
    {
        shut_down();
    }
}

//****************************************************************************************************************

void shut_down()//stop robot, shut down booms
{
    PORTWrite(IOPORT_B, 0);//shut off robot motors

}

//****************************************************************************************************************
//if robot is at location var1, skip. Else plot course to get to location var1
void get_started(double C_pos_Lat, double C_pos_Lon, int var1lat, int var1lon)
{
    if((C_pos_Lat != var1lat) || (C_pos_Lon != var1lon))
    {
        //need to compute path from current to var1 coordinates
    }
}

//****************************************************************************************************************

 void manual()//remote control; utilizes BT2 to control robot movement
 {
     return;
 }

 //****************************************************************************************************************

 void load_info()//get coordinates from BT2 module
 {

 }

 double field_end(double *C_pos_Lat, double *C_pos_Lon, int direction, double *D_heading, double *C_heading, double *MAS_hed1, double *MAS_hed)
 {
     if(direction == 1)//if current direction is initial direction
     {
         int x = 0;
         PORTWrite(IOPORT_B, 2<<10);//right side off
         *D_heading += 90;//add 90 degrees
         while(x > 0)// loop until C_heading = D_heading + 90
         {
             get_current_data(C_pos_Lon,C_pos_Lat, C_heading);//get current heading, mainly going for compass heading here
             x = *D_heading - *C_heading;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue

         int a = 0, b = 0, c = 0;
         while(c < 20)//wait until gone distance, 20ft
         {
             get_current_data(C_pos_Lon, C_pos_Lat, C_heading);

         }

         *D_heading = *MAS_hed1; //desired new heading is secondary heading(MAS_head1) 180 degrees off previous heading

         PORTWrite(IOPORT_B, 2<<10);//right side off

         while(x > 0)// loop until C_heading = MAS_hed1
         {
             get_current_data(C_pos_Lat, C_pos_Lon, C_heading);//get current heading, mainly going for compass heading here
             x = *D_heading - *C_heading;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue
         direction = 0; //update direction
     }
     else//current direction is secondary direction
     {
         int x = 0;
         PORTWrite(IOPORT_B, 1<<10);//left side off
         *D_heading -= 90;//subtract 90 degrees
         while(x > 0)// loop until C_heading = D_heading - 90
         {
             get_current_data(C_pos_Lat, C_pos_Lon, C_heading);//get current heading, mainly going for compass heading here
             x = *D_heading - *C_heading;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue

         int a = 0, b = 0, c = 0;
         while(c < 20)//wait until gone distance, 20ft
         {

         }

         *D_heading = *MAS_hed; //desired new heading is secondary heading(MAS_head1) 180 degrees off previous heading

         PORTWrite(IOPORT_B, 1<<10);//left side off

         while(x > 0)// loop until C_heading = MAS_hed
         {
             get_current_data(C_pos_Lat, C_pos_Lon, C_heading);//get current heading, mainly going for compass heading here
             x = *D_heading - *C_heading;//subtract to compare
         }
         PORTWrite(IOPORT_B, 3<<10);//turned desired amount, continue
         direction = 1; //update direction
     }
 }
