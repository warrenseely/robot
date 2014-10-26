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

//Function prototypes
    double set_path(int var1lat, int var1lon, int var2lat, int var2lon, double *MAS_head, double *MAS_head1);//set heading for robot to follow
    void reset(void); //make sure everything off when starting
    void setupPORTs(void); //setup ports
    void setup_UART1(void);//communication for the display screen
    double correct_path(double *D_heading, double *C_pos_Lat, double *C_pos_Lon, int var1lat, int var1lon, int direction);
    void setup_UART2(void);//communication for the GPS receiver
    void get_current_data(double *C_pos_Lat, double *C_pos_Lon, double *C_heading);//get current lat/lon/heading from GPS
    void print(int choice);//print desired string
    void startup(void);//beginning program function; use to select mode, and maybe other?
    void mode(void);//choose auto/manual/info modes
    void delay(int i);//a delay function for turning or screen display
    void job_done(double C_pos_Lat, double C_pos_Lon, int var9lat, int var9lon);//checks to see if robot has finished job coverage area
    void shut_down(void);//shuts robot down
    void get_started(double C_pos_Lat, double C_pos_Lon, int var1lat, int var1lon);//coming up. autonomous arrival of robot from drop site to job area
    void manual(void);//manual mode
    double field_end(double *C_pos_Lat, double *C_pos_Lon, int direction, double *D_heading, double *C_heading, double *MAS_head1, double *MAS_head);//for turning at field ends
    void load_info(void);//coming up. load GPS coordinates into memory through interface
    void LCD_rst(void);//clear and reset display screen



#ifdef	__cplusplus
}
#endif

#endif	/* HEADER_H */

