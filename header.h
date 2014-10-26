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
//extern double boundary[];//for the boundary coordinates; 9 coordinate pairs; even index is latitudes, odd is longitudes;
extern double MAS_head, MAS_head1, D_heading, C_heading, direction;
extern double clat1, clon1;
extern int width;


struct BOUNDARY//base for containing the area boundary
{
    double lat1;
    double lon1;
    double lat2;
    double lon2;
    double lat3;
    double lon3;
    double lat4;
    double lon4;
    double lat5;
    double lon5;
    double lat6;
    double lon6;
    double lat7;
    double lon7;
    double lat8;
    double lon8;
    double lat9;
    double lon9;
};

struct GPS_TRANS//base for containing the current robot position
{
    double lat;
    double lon;
    double time;
    double course;
    double date;
};

struct GPS_DATA_T //85 long; base for containing the GPS string
{

    char ID[6];
    char UTC_time[11];
    char STATUS[2];
    char LAT[8];
    char N_S_indicator[2];
    char LON[11];
    char E_W_indicator[2];
    char SPEED[5];
    char Course_over_ground[7];
    char Date[7];
    char MagVar[5];
    char E_W_Mag[2];
    char mode[2];
    char Checksum[4];
    char Eom[9];

};

struct GPS_TRANS Position;//define struct to hold robot current position
struct GPS_DATA_T GPSdata;//define struct to hold GPS string data
struct BOUNDARY boundary;//define struct to hold the coverage boundary coordinates

//Function prototypes
    double set_path(int flag);//set heading for robot to follow
    void reset(void); //make sure everything off when starting
    void setupPORTs(void); //setup ports
    void setup_UART1(void);//communication for the display screen
    void correct_path(void);
    void setup_UART2(void);//communication for the GPS receiver
    void get_current_data(void);//get current lat/lon/heading from GPS
    void read_GPS_fields(char * address); //Reads gps string to a comma, stores in struct pointed to by *address
    void print(int choice);//print desired string
    void startup(void);//beginning program function; use to select mode, and maybe other?
    void mode(void);//choose auto/manual/info modes
    void delay(int i);//a delay function for screen display
    int job_done(void);//checks to see if robot has finished job coverage area
    void shut_down(void);//shuts robot down
    void get_GPS_started(void);//coming up. autonomous arrival of robot from drop site to job area
    void navigate_area_start(void);//navigate from current position to start of area to cover
    void manual(void);//manual mode
    double field_end(double temp[]);//for turning at field ends
    void load_info(void);//coming up. load GPS coordinates into memory through interface
    void LCD_rst(void);//clear and reset display screen
    double distance(int flag);//computes distance from initial call until current call;
                              //set flag = 0 before initial call;used with loop, returns distance
    void setup_SPI1 (void);

#ifdef	__cplusplus
}
#endif

#endif	/* HEADER_H */

