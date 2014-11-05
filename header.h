/*
 * File:   header.h
 * Author: Warren Seely // Matthew Ferran
 *
 * Created on January 2, 2014, 9:59 PM
 */

#ifndef HEADER_H
#define	HEADER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <plib.h>
#include <math.h>
#include "string.h"

//extern int width;


struct PASS_INFO
{
    //The coordinate to navigate from on the current pass
    double nav_from_lat;
    double nav_from_lon;

    //The coordinate to navigate to on the current pass
    double nav_to_lat;
    double nav_to_lon;

    //The headings necessary for pass guidance
    double Master;
    double Secondary;
    double D_heading;
    double C_heading;

    //Indicates the current direction; 1 for same as first pass, 0 for opposite first pass
    int direction;

    //The current latitude and longitude for distance calculations in distance()
    double clat1;
    double clon1;
};

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
    int width; //currently present in load_info() but not utilized
};

struct GPS_TRANS//base for containing the current robot position
{
    double lat;
    double lon;
    double time;
    double course;
    double date;
};

typedef struct   //85 long; base for containing the GPS string
{

    char ID[6];
    char UTC_time[11];
    char STATUS[2];
    char LAT[10];
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

}GPS_DATA_T;

//struct GPS_DATA_T GPSdata;
struct GPS_TRANS Position;//define struct to hold robot current position
struct BOUNDARY boundary;//define struct to hold the coverage boundary coordinates
struct PASS_INFO pass;//define struct to contain information for each pass

//Function prototypes
    void set_path(int flag);//set heading for robot to follow
    void reset(void); //make sure everything off when starting
    void setupPORTs(void); //setup ports
    void setup_UART1(void);//communication for the display screen
    void correct_path(void);
    void setup_UART2(void);//communication for the GPS receiver
    char get_current_data(void);//get current lat/lon/heading from GPS
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
    double field_end(void);//for turning at field ends
    void load_info(void);//coming up. load GPS coordinates into memory through interface
    void load_info_get_modify(int *pair, char type[], char *flag);
    void load_coordinate(int pairnum, int type, char *flag);
    void LCD_rst(void);//clear and reset display screen
    double distance(int *flag);//computes distance from initial call until current call;
                              //set flag = 0 before initial call;used with loop, returns distance
    void setup_SPI1 (void);
    void compute_pass_point(void); //Compute the point to navigate to on current pass
    void start_guidance(void); //start the robot moving forward; Eventually check to make sure all external sensors
                               //working before continuing
    void traverse_boundary(int flag); //for gathering the boundary coordinates by driving around the boundary.
                                  //current position stored in boundary every time 'r' is hit
    void sleep(void); //wait in here until a button pressed. "Sleeping".
    double convert_feet(double, double); //convert the latitude/longitude coordinates to feet for distance calculation

#ifdef	__cplusplus
}
#endif

#endif	/* HEADER_H */

