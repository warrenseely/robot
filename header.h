/* 
 * File:   header.h
 * Author: Warren
 *
 * Created on January 2, 2014, 9:59 PM
 */

#ifndef HEADER_H
#define	HEADER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <plib.h>
//#include <p32xxxx.h>
#include <math.h>
#include "string.h"
//#include <peripheral/spi.h>
    /*Declared as external and included here to give access to every file*/
extern double boundary[];//for the boundary coordinates; 9 coordinate pairs; even index is latitudes, odd is longitudes;
extern double MAS_head, MAS_head1, D_heading, C_heading, direction;
extern double C_pos_Lat, C_pos_Lon, speed, clat1, clon1;
extern int width;

//Function prototypes
    double set_path(int flag);//set heading for robot to follow
    void reset(void); //make sure everything off when starting
    void setupPORTs(void); //setup ports
    void setup_UART1(void);//communication for the display screen
    void correct_path(void);
    void setup_UART2(void);//communication for the GPS receiver
    void get_current_data(void);//get current lat/lon/heading from GPS
    void print(int choice);//print desired string
    void startup(void);//beginning program function; use to select mode, and maybe other?
    void mode(void);//choose auto/manual/info modes
    void delay(int i);//a delay function for screen display
    int job_done(void);//checks to see if robot has finished job coverage area
    void shut_down(void);//shuts robot down
    void get_GPS_started(void);//make sure GPS acquired signal
    void manual(void);//manual mode
    double field_end(void);//for turning at field ends
    void load_info(void);//load GPS coordinates into memory through interface
    void LCD_rst(void);//clear and reset display screen
    double distance(int flag);//computes distance from initial call until current call;
                              //set flag = 0 before initial call;used with loop, returns distance
    void setup_SPI1 (void);
    void navigate_start(void); //if at powerup robot is not near the first coordinate set, navigate to

#ifdef	__cplusplus
}
#endif

#endif	/* HEADER_H */

