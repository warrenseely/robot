/*
 * File:   header.h
 * Author: Warren Seely // Matthew Ferran
 *
 * Created on January 2, 2014, 9:59 PM
 */

#ifndef HEADER_H
#define	HEADER_H
#define _SUPPRESS_PLIB_WARNING

#ifdef	__cplusplus
extern "C" {
#endif

#include <plib.h>
#include <math.h>
#include "string.h"

 #define FIFOI2C_BAUD_RATE 100000
    #define SLAVE_ADDRESS_7_BIT 0x1E
    //NOTE: Modification of FIFOI2C_initialize() is required to support
    //      more than 5 devices.
    #define FIFOI2C_DEVICES_COUNT 1
    #define FIFOI2C_DEVICE0_ADDRESS 0x1E  //7 bit address
    //#define FIFOI2C_DEVICE1_ADDRESS 0x1E
    //#define FIFOI2C_DEVICE2_ADDRESS 0x1E
    //#define FIFOI2C_DEVICE3_ADDRESS 0x1E
    //#define FIFOI2C_DEVICE4_ADDRESS 0x1E


    #define FIFOI2C_TRANSMIT_BUFFER_SIZE 50
    #define FIFOI2C_RECEIVE_BUFFER_SIZE 50 //Not having this large enough causes wierd bugs

        #if defined (__32MX460F512L__)
        #define SYS_FREQ (80000000L)
        #define GetSystemClock()           (SYS_FREQ)
        #define GetPeripheralClock()       (SYS_FREQ/8)
        #define GetInstructionClock()      (SYS_FREQ)
    #endif




    typedef unsigned char uint8;
    typedef unsigned short uint16;
    typedef unsigned int uint32;


    typedef enum
    {
        FIFOI2C_DEVICE_COMMAND_CMDERROR,    //Signifies an unknown Error
        FIFOI2C_DEVICE_COMMAND_CMDEND,      //Signifies and EOT to the program
        FIFOI2C_DEVICE_COMMAND_START,
        FIFOI2C_DEVICE_COMMAND_TX_BYTE,
        FIFOI2C_DEVICE_COMMAND_RESTART,
        FIFOI2C_DEVICE_COMMAND_RX_BYTE,
        FIFOI2C_DEVICE_COMMAND_ACK,
        FIFOI2C_DEVICE_COMMAND_NACK,
        FIFOI2C_DEVICE_COMMAND_STOP
    }FIFOI2C_Device_Commands;



    typedef struct fifoi2c_tx_byte
    {
        uint8 tx_byte;
        FIFOI2C_Device_Commands device_command;
    }FIFOI2C_TX_Byte;

    typedef struct fifoi2c_rx_byte
    {
        uint8 rx_byte;
        FIFOI2C_Device_Commands device_command;
    }FIFOI2C_RX_Byte;

    typedef struct fifoi2c_device
    {
        uint8 address;

        int transmit_buffer_current;
        int transmit_buffer_length;
        FIFOI2C_TX_Byte transmit_buffer[FIFOI2C_TRANSMIT_BUFFER_SIZE];

        int receive_buffer_current;
        int receive_buffer_length;
        FIFOI2C_RX_Byte receive_buffer[FIFOI2C_TRANSMIT_BUFFER_SIZE];
    }FIFOI2C_Device;

    void FIFOI2C_initialize();
    uint8 FIFOI2C_addQueue(uint16 device, uint8 byte_buffer[], FIFOI2C_Device_Commands state_buffer[], uint32 buffer_length);
    FIFOI2C_RX_Byte FIFOI2C_readQueue(uint16 device);

    uint8 FIFOI2C_addQueue_readDeviceRegisters(uint16 device, uint8 start_register, int number_to_read);
    uint8 FIFOI2C_addQueue_writeDeviceRegisters(uint16 device, uint8 start_address, uint8 byte_buffer[], uint32 buffer_length);



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

struct HEADING //for storing the heading readings
    {
        double x1;

        double y1;

        double z1;

        double course;
    };

struct HEADING heading;

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
    int startup(void);//beginning program function; use to select mode, and maybe other?
    int mode(void);//choose auto/manual/info modes
    void delay(int i);//a delay function for screen display
    int job_done(void);//checks to see if robot has finished job coverage area
    void shut_down(void);//shuts robot down
    void get_GPS_started(void);//coming up. autonomous arrival of robot from drop site to job area
    void navigate_area_start(void);//navigate from current position to start of area to cover
    void manual_mode(void);//manual mode
    double field_end(void);//for turning at field ends
    void load_info(void);//coming up. load GPS coordinates into memory through interface
    void load_info_get_modify(int *pair, char type[], char *flag);
    void load_coordinate(int pairnum, int type, char *flag);
    void LCD_rst(void);//clear and reset display screen
    double distance(int *flag);//computes distance from initial call until current call;
                              //set flag = 0 before initial call;used with loop, returns distance
    void setup_SPIs (void);
    void compute_pass_point(void); //Compute the point to navigate to on current pass
    void start_guidance(void); //start the robot moving forward; Eventually check to make sure all external sensors
                               //working before continuing
    void traverse_boundary(int flag); //for gathering the boundary coordinates by driving around the boundary.
                                  //current position stored in boundary every time 'r' is hit
    void sleep(void); //wait in here until a button pressed. "Sleeping".
    double convert_feet(double clat2, double clon2); //convert the latitude/longitude coordinates to feet for distance calculation

    int killswitch(void); //prototype emergency shutoff function
    int auto_mode(void); //the main auto mode runner

    void DelayTime(int ms);

    void compass_startMeasurements();
    void compass_queueReadXZY();
    void compass_interpretXZY();
    void read_compass();
    
#ifdef	__cplusplus
}
#endif

#endif	/* HEADER_H */

