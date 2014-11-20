//#include "compass.h"
//#include <math.h>
//
//
//
//void HMC5883L_startMeasurements()
//{
//    int i = 0;
//    uint8 buffer[10];
//
//    //Config Register A
//    buffer[0] = 0x74; //111 0100  //30hz refresh rate
//
//    //Config Register B
//    buffer[1] = 0xA0;
//
//    //Mode Register
//    buffer[2] = 0x00; //cont meaurement mode
//
//    FIFOI2C_addQueue_writeDeviceRegisters(0, 0x00, buffer, 3);
//}
//
//
//void HMC5883L_queueReadXZY()
//{
//    FIFOI2C_addQueue_readDeviceRegisters(0, 0x03, 6);
//}
//
//
//void HMC5883L_interpretXZY()
//{
//    uint8 x_msb = 0, x_lsb = 0;
//    uint8 z_msb = 0, z_lsb = 0;
//    uint8 y_msb = 0, y_lsb = 0;
//    short x_tmp = 0, z_tmp = 0, y_tmp= 0;
//    double gauss_per_LSB = 0, front_heading = 0, XVector = 0, YVector = 0, ZVector = 0;
//
//    x_msb = FIFOI2C_readQueue(0).rx_byte;
//    x_lsb = FIFOI2C_readQueue(0).rx_byte;
//
//    z_msb = FIFOI2C_readQueue(0).rx_byte;
//    z_lsb = FIFOI2C_readQueue(0).rx_byte;
//
//    y_msb = FIFOI2C_readQueue(0).rx_byte;
//    y_lsb = FIFOI2C_readQueue(0).rx_byte;
//
//    x_tmp = (x_msb << 8) | x_lsb;
//    z_tmp = (z_msb << 8) | z_lsb;
//    y_tmp = (y_msb << 8) | y_lsb;
//
//    //Based on devie 'Mode Register.' Refer to HMC5883L data sheet.
//    gauss_per_LSB = 0.92e-3;
//
//    heading.x1 = gauss_per_LSB * (double)x_tmp;
//    heading.y1 = gauss_per_LSB * (double)y_tmp;
//    heading.z1 = gauss_per_LSB * (double)z_tmp;
//
//    heading.course = atan2(heading.y1, heading.x1) * (180 / 3.14159); //convert radians to degrees
//
//    if (heading.course < 0)
//    {
//        heading.course += 360; //make sure a positive angle between 0-360
//    }
//    if (heading.course > 360)
//    {
//        heading.course -= 360; //make sure angle between 0-360
//    }
//}
//
//
