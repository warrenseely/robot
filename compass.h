#ifndef COMPASS_H
#define	COMPASS_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <plib.h>

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


    void compass_startMeasurements();
    void compass_queueReadXZY();
    void compass_interpretXZY();
    void read_compass();

    struct HEADING //for storing the heading readings
    {
        double x1;

        double y1;

        double z1;

        double course;
    };

    struct HEADING heading;



#ifdef	__cplusplus
}
#endif

#endif	/* HMC5883L_H */
