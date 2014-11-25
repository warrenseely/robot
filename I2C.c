#include "header.h"

//Local to .c file
FIFOI2C_Device FIFOI2C_Devices_List[FIFOI2C_DEVICES_COUNT];
uint8 FIFOI2C_isRunning = 0;
uint16 FIFOI2C_currentDevice = 0;
uint8 FIFOI2C_nextInterruptReceive = 0;

void FIFOI2C_initialize()
{
    int i = 0;
    int device = 0;


    //Interrupt Stuff
    INTSetVectorPriority(INT_I2C_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_I2C_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);

    INTClearFlag(INT_I2C2B);
    //INTClearFlag(INT_I2C2S);
    INTClearFlag(INT_I2C2M);

    /*Bus Collision events that generate an interrupt are:
        ?During a Start sequence ? SDAx sampled before Start condition
        ?During a Start sequence ? SCLx = 0 before SDAx = 0
        ?During a Start sequence ? SDAx = 0 before BRG time out
        ?During a Repeated Start sequence ? If SDAx is sampled 0 when SCLx goes high
        ?During a Repeated Start sequence ? If SCLx goes low before SDAx goes low
        ?During a Stop sequence ? If SDAx is sampled low after allowing it to float
        ?During a Stop sequence ? If SCLx goes low before SDAx goes high*/
    INTEnable(INT_I2C2B, INT_ENABLED); //I2C2 Bus Collision Event
    /*Detection of a valid device address (including general call) ? Ninth falling edge of SCLx
        (after sending ACK to master. Address must match unless the STRICT bit = 1 (I2CxCON<11>) or the GCEN bit =1 (I2CxCON<7>)
        ?Reception of data ? Ninth falling edge of SCLx (after sending the ACK to master)
        ?Request to transmit data ? Ninth falling edge of SCLx (regardless of receiving an ACK from the master)*/
    //INTEnable(INT_I2C2S, INT_ENABLED); //I2C2 Slave Event
    /*Master mode operations that generate a master interrupt are:
        ?Start Condition ? 1 BRG time after falling edge of SDAx
        ?Repeated Start Sequence ? 1 BRG time after falling edge of SDAx
        ?Stop Condition ? 1 BRG time after the rising edge of SDAx
        ?Data transfer byte received ? Eighth falling edge of SCLx (after receiving eight bits of data from slave)
        ?During a Send ACK sequence ? Ninth falling edge of SCLx (after sending ACK or NACK to slave)
        ?Data transfer byte transmitted ? Ninth falling edge of SCLx (regardless of receiving ACK from slave)
        ?During a slave-detected Stop ? When slave sets the P bit (I2CxSTAT<4>)*/
    INTEnable(INT_I2C2M, INT_ENABLED); //I2C2 Master Event


    //Initialize Devices
    #if FIFOI2C_DEVICES_COUNT >= 1
        device = 0;
        FIFOI2C_Devices_List[device].address = FIFOI2C_DEVICE0_ADDRESS;
        FIFOI2C_Devices_List[device].transmit_buffer_current = 0;
        FIFOI2C_Devices_List[device].transmit_buffer_length = 0;
        FIFOI2C_Devices_List[device].receive_buffer_current = 0;
        FIFOI2C_Devices_List[device].receive_buffer_length = 0;

        i = 0;
        while(i < FIFOI2C_TRANSMIT_BUFFER_SIZE)
        {
            FIFOI2C_TX_Byte txb;
            txb.tx_byte = 0;
            txb.device_command = 0;
            FIFOI2C_Devices_List[device].transmit_buffer[i++] = txb;
        }
        i = 0;
        while(i < FIFOI2C_RECEIVE_BUFFER_SIZE)
        {
            FIFOI2C_RX_Byte rxb;
            rxb.rx_byte = 0;
            rxb.device_command = 0;
            FIFOI2C_Devices_List[device].receive_buffer[i++] = rxb;
        }
    #endif
    #if FIFOI2C_DEVICES_COUNT >= 2
        device = 1;
        FIFOI2C_Devices_List[device].address = FIFOI2C_DEVICE1_ADDRESS;
        FIFOI2C_Devices_List[device].transmit_buffer_current = 0;
        FIFOI2C_Devices_List[device].transmit_buffer_length = 0;
        FIFOI2C_Devices_List[device].receive_buffer_current = 0;
        FIFOI2C_Devices_List[device].receive_buffer_length = 0;

        i = 0;
        while(i < FIFOI2C_TRANSMIT_BUFFER_SIZE)
        {
            FIFOI2C_TX_Byte txb;
            txb.tx_byte = 0;
            txb.device_command = 0;
            FIFOI2C_Devices_List[device].transmit_buffer[i++] = txb;
        }
        i = 0;
        while(i < FIFOI2C_RECEIVE_BUFFER_SIZE)
        {
            FIFOI2C_RX_Byte rxb;
            rxb.rx_byte = 0;
            rxb.device_command = 0;
            FIFOI2C_Devices_List[device].receive_buffer[i++] = rxb;
        }
    #endif
    #if FIFOI2C_DEVICES_COUNT >= 3
        device = 2;
        FIFOI2C_Devices_List[device].address = FIFOI2C_DEVICE2_ADDRESS;
        FIFOI2C_Devices_List[device].transmit_buffer_current = 0;
        FIFOI2C_Devices_List[device].transmit_buffer_length = 0;
        FIFOI2C_Devices_List[device].receive_buffer_current = 0;
        FIFOI2C_Devices_List[device].receive_buffer_length = 0;

        i = 0;
        while(i < FIFOI2C_TRANSMIT_BUFFER_SIZE)
        {
            FIFOI2C_TX_Byte txb;
            txb.tx_byte = 0;
            txb.device_command = 0;
            FIFOI2C_Devices_List[device].transmit_buffer[i++] = txb;
        }
        i = 0;
        while(i < FIFOI2C_RECEIVE_BUFFER_SIZE)
        {
            FIFOI2C_RX_Byte rxb;
            rxb.rx_byte = 0;
            rxb.device_command = 0;
            FIFOI2C_Devices_List[device].receive_buffer[i++] = rxb;
        }
    #endif
    #if FIFOI2C_DEVICES_COUNT >= 4
        device = 3;
        FIFOI2C_Devices_List[device].address = FIFOI2C_DEVICE3_ADDRESS;
        FIFOI2C_Devices_List[device].transmit_buffer_current = 0;
        FIFOI2C_Devices_List[device].transmit_buffer_length = 0;
        FIFOI2C_Devices_List[device].receive_buffer_current = 0;
        FIFOI2C_Devices_List[device].receive_buffer_length = 0;

        i = 0;
        while(i < FIFOI2C_TRANSMIT_BUFFER_SIZE)
        {
            FIFOI2C_TX_Byte txb;
            txb.tx_byte = 0;
            txb.device_command = 0;
            FIFOI2C_Devices_List[device].transmit_buffer[i++] = txb;
        }
        i = 0;
        while(i < FIFOI2C_RECEIVE_BUFFER_SIZE)
        {
            FIFOI2C_RX_Byte rxb;
            rxb.rx_byte = 0;
            rxb.device_command = 0;
            FIFOI2C_Devices_List[device].receive_buffer[i++] = rxb;
        }
    #endif
    #if FIFOI2C_DEVICES_COUNT >= 5
        device = 4;
        FIFOI2C_Devices_List[device].address = FIFOI2C_DEVICE4_ADDRESS;
        FIFOI2C_Devices_List[device].transmit_buffer_current = 0;
        FIFOI2C_Devices_List[device].transmit_buffer_length = 0;
        FIFOI2C_Devices_List[device].receive_buffer_current = 0;
        FIFOI2C_Devices_List[device].receive_buffer_length = 0;

        i = 0;
        while(i < FIFOI2C_TRANSMIT_BUFFER_SIZE)
        {
            FIFOI2C_TX_Byte txb;
            txb.tx_byte = 0;
            txb.device_command = 0;
            FIFOI2C_Devices_List[device].transmit_buffer[i++] = txb;
        }
        i = 0;
        while(i < FIFOI2C_RECEIVE_BUFFER_SIZE)
        {
            FIFOI2C_RX_Byte rxb;
            rxb.rx_byte = 0;
            rxb.device_command = 0;
            FIFOI2C_Devices_List[device].receive_buffer[i++] = rxb;
        }
    #endif


    //configure the clock for the I2C2

    I2C2BRG = (1.0/(2.0*FIFOI2C_BAUD_RATE) - 104e-9) * GetPeripheralClock() - 1.5; //-1.5 takes into account rounding for -2.
    //Enable I2C2 Module
    I2C2CONbits.ON = 1;
}

uint8 FIFOI2C_addQueue(uint16 device, uint8 byte_buffer[], FIFOI2C_Device_Commands state_buffer[], uint32 buffer_length)
{
    int i = 0, ind = 0;

    //Check for potential overflow of the TX buffer.
    if ((FIFOI2C_Devices_List[device].transmit_buffer_length + buffer_length) >= (FIFOI2C_TRANSMIT_BUFFER_SIZE - 1))
    {
        return -1;
    }
    else
    {
        //Adds the byte and state buffers to the Device's buffer
        while (i < buffer_length)
        {
            FIFOI2C_TX_Byte txb;
            txb.tx_byte = byte_buffer[i];
            txb.device_command = state_buffer[i];
            ind = FIFOI2C_Devices_List[device].transmit_buffer_length;
            FIFOI2C_Devices_List[device].transmit_buffer[ind] = txb;
            FIFOI2C_Devices_List[device].transmit_buffer_length++;
            i++;
        }

        //If FIFOI2C isn't running, then start it
        if (FIFOI2C_isRunning == 0)
        {
            FIFOI2C_currentDevice = device;
            FIFOI2C_isRunning = 1;
            //Trigger to I2C2 Master IRQ is interrupts are enabled.
            INTSetFlag(INT_I2C2M);
        }
    }
}

FIFOI2C_RX_Byte FIFOI2C_readQueue(uint16 device)
{
    int ind = 0;
    FIFOI2C_RX_Byte rxb;

    //Checks for read-overflow error
    if (FIFOI2C_Devices_List[device].receive_buffer_length >= (FIFOI2C_RECEIVE_BUFFER_SIZE - 1))
    {
        rxb.device_command = FIFOI2C_DEVICE_COMMAND_CMDERROR;
        rxb.rx_byte = 0;
    }
    else
    {
        ind = FIFOI2C_Devices_List[device].receive_buffer_current;
        rxb.device_command = FIFOI2C_Devices_List[device].receive_buffer[ind].device_command;
        rxb.rx_byte = FIFOI2C_Devices_List[device].receive_buffer[ind].rx_byte;
        FIFOI2C_Devices_List[device].receive_buffer_current++;


        //If all the bytes have been read from the receive buffer reset the indexes
        if (FIFOI2C_Devices_List[device].receive_buffer_current >= FIFOI2C_Devices_List[device].receive_buffer_length)
        {
            //Sensitive Code. Disable temporarily masterinterrupt (if it's enabled.)
            if (INTGetEnable(INT_I2C2M) != 0)
            {
                INTEnable(INT_I2C2M, INT_DISABLED);
                FIFOI2C_Devices_List[device].receive_buffer_length = 0;
                FIFOI2C_Devices_List[device].receive_buffer_current = 0;
                INTEnable(INT_I2C2M, INT_ENABLED);
            }
            else
            {
                FIFOI2C_Devices_List[device].receive_buffer_length = 0;
                FIFOI2C_Devices_List[device].receive_buffer_current = 0;
            }

        }

        return rxb; //Return the received byte.
    }
}

uint8 FIFOI2C_addQueue_readDeviceRegisters(uint16 device, uint8 start_register, int number_to_read)
{
    int i = 0;
    int ind = 0;
    int j = 0;
    uint8 byte_buffer[FIFOI2C_TRANSMIT_BUFFER_SIZE];
    FIFOI2C_Device_Commands state_buffer[FIFOI2C_TRANSMIT_BUFFER_SIZE];
    if (number_to_read > 0)
    {
        //Start
        state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_START; //Start it
        byte_buffer[ind++] = 0x13; //filler


        //Device Addr + write
        state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_TX_BYTE; //Transmit Byte
        byte_buffer[ind++] = (FIFOI2C_Devices_List[device].address << 1) | 0x00;

        //Send Start Register Address
        state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_TX_BYTE; //Transmit Byte
        byte_buffer[ind++] = start_register;

        //restart
        state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_RESTART; //Transmit Byte
        byte_buffer[ind++] = 0x00; //filler


        //Device Address + Read
        state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_TX_BYTE; //Transmit Byte
        byte_buffer[ind++] = (FIFOI2C_Devices_List[device].address << 1) | 0x01;

        while (i < (number_to_read - 1))
        {
            //Sensor sends data (receive byte routine)
            state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_RX_BYTE; //Receive Byte
            byte_buffer[ind++] = 0x00; //filler

            //Master send ACK
            state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_ACK; //Receive Byte
            byte_buffer[ind++] = 0x00; //filler

            i++;
        }

        //Sensor sends data (receive byte routine)
        state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_RX_BYTE; //Receive Byte
        byte_buffer[ind++] = 0x00; //filler

        //Master send NACK
        state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_NACK; //Receive Byte
        byte_buffer[ind++] = 0x00; //filler

        //Master sends STOP
        state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_STOP; //Receive Byte
        byte_buffer[ind++] = 0x00; //filler

        //Required for IRQ to know when to stop
        state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_CMDEND; //Signifys end of transmission
        byte_buffer[ind++] = 0x00; //filler


        FIFOI2C_addQueue(device, byte_buffer, state_buffer, ind);

        return 0;
    }
    else
    {
        return -1;
    }
}

uint8 FIFOI2C_addQueue_writeDeviceRegisters(uint16 device, uint8 start_register, uint8 byte_buffer[], uint32 buffer_length)
{
    int i = 0, ind = 0;
    uint8 send_byte_buffer[FIFOI2C_TRANSMIT_BUFFER_SIZE];
    FIFOI2C_Device_Commands state_buffer[FIFOI2C_TRANSMIT_BUFFER_SIZE];

    if ((FIFOI2C_Devices_List[device].transmit_buffer_length + buffer_length) <= (FIFOI2C_TRANSMIT_BUFFER_SIZE - 1))
    {

    //Start
    state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_START; //Start it
    send_byte_buffer[ind++] = 0x13; //filler

    //Device Addr + write
    state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_TX_BYTE; //Transmit Byte
    send_byte_buffer[ind++] = (FIFOI2C_Devices_List[device].address << 1) | 0x00;

    //Send Start Register Address
    state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_TX_BYTE; //Transmit Byte
    send_byte_buffer[ind++] = start_register;

    while (i < (buffer_length - 1))
    {
        //Send value to write to register
        state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_TX_BYTE; //Transmit Byte
        send_byte_buffer[ind++] = byte_buffer[i];

        //Do I need a Recieved_ACK cmd?

        i++;
    }

    //Send value to write to register
    state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_TX_BYTE; //Transmit Byte
    send_byte_buffer[ind++] = byte_buffer[i];


    //Master sends STOP
    state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_STOP;
    send_byte_buffer[ind++] = 0x00; //filler


    //Required for IRQ to know when to stop
    state_buffer[ind] = FIFOI2C_DEVICE_COMMAND_CMDEND; //Signifys end of transmission
    send_byte_buffer[ind++] = 0x00; //filler


    FIFOI2C_addQueue(device, send_byte_buffer, state_buffer, ind);
    }
    else
    {
        return -1;
    }
}


void __ISR(_I2C_2_VECTOR, IPL4AUTO) __I2C2Interrupt(void)
{

    int i = 0;
    int ind = 0, ind2 = 0;
    FIFOI2C_RX_Byte rxb;

    if (INTGetFlag(INT_I2C2B)) //Bus Collision
    {
        INTClearFlag(INT_I2C2B);

    }
    if (INTGetFlag(INT_I2C2M)) //Master Intterupt
    {
        INTClearFlag(INT_I2C2M);

        //If we have received a byte, read it.
        if (FIFOI2C_nextInterruptReceive == 1)
        {
            //Setup indexes (easier to read)
            ind = FIFOI2C_Devices_List[FIFOI2C_currentDevice].transmit_buffer_current;
            ind2 = FIFOI2C_Devices_List[FIFOI2C_currentDevice].receive_buffer_length;

            //Read the received byte and the current command we are on.
            rxb.device_command = FIFOI2C_Devices_List[FIFOI2C_currentDevice].transmit_buffer[ind].device_command;
            rxb.rx_byte = I2C2RCV;

            //Write rxb to the receive buffer
            FIFOI2C_Devices_List[FIFOI2C_currentDevice].receive_buffer[ind2] = rxb;

            //increment indexes
            FIFOI2C_Devices_List[FIFOI2C_currentDevice].receive_buffer_length++;
            FIFOI2C_nextInterruptReceive = 0;
        }

        //Switch statement for the current_device's transmit byte's device_state
        ind = FIFOI2C_Devices_List[FIFOI2C_currentDevice].transmit_buffer_current;
        switch(FIFOI2C_Devices_List[FIFOI2C_currentDevice].transmit_buffer[ind].device_command)
        {
            case FIFOI2C_DEVICE_COMMAND_CMDERROR:
                ind = 5; //filler
                break;
            case FIFOI2C_DEVICE_COMMAND_START:
                I2C2CONbits.SEN = 1;    //start condition sequence
                break;
            case FIFOI2C_DEVICE_COMMAND_TX_BYTE:
                I2C2TRN = FIFOI2C_Devices_List[FIFOI2C_currentDevice].transmit_buffer[ind].tx_byte; //transmit byte
                break;
            case FIFOI2C_DEVICE_COMMAND_RESTART:
                I2C2CONbits.RSEN = 1;   //repeated start condition sequence
                break;
            case FIFOI2C_DEVICE_COMMAND_RX_BYTE:
                I2C2CONbits.RCEN = 1;   //Receive byte sequence
                //Flag for next intr to be for receiving said byte
                FIFOI2C_nextInterruptReceive = 1;
                break;
            case FIFOI2C_DEVICE_COMMAND_ACK:
                I2C2CONbits.ACKDT = 0;  //ACK
                I2C2CONbits.ACKEN = 1;  //Send ACK sequence
                break;
            case FIFOI2C_DEVICE_COMMAND_NACK:
                I2C2CONbits.ACKDT = 1;  //NACK
                I2C2CONbits.ACKEN = 1;  //Send NACK sequence
                break;
            case FIFOI2C_DEVICE_COMMAND_STOP:
                I2C2CONbits.PEN = 1;
                break;
            case FIFOI2C_DEVICE_COMMAND_CMDEND:
                //Loop through all devices and check if they have something (or more somethings) to send.

                //Check Current Device first
                if ((FIFOI2C_Devices_List[FIFOI2C_currentDevice].transmit_buffer_current + 1) >=
                        FIFOI2C_Devices_List[FIFOI2C_currentDevice].transmit_buffer_length) //Nothing to send
                {
                    //Turn it off device
                    FIFOI2C_isRunning = 0;

                    //Reset transmit indexes
                    FIFOI2C_Devices_List[FIFOI2C_currentDevice].transmit_buffer_current = 0;
                    FIFOI2C_Devices_List[FIFOI2C_currentDevice].transmit_buffer_length = 0;

                    //Check other devices now
                    while (i < FIFOI2C_DEVICES_COUNT)
                    {
                        //If not this device (since we already checked it)
                        if (i != FIFOI2C_currentDevice)
                        {
                            if(FIFOI2C_Devices_List[i].transmit_buffer_length >
                                    FIFOI2C_Devices_List[i].transmit_buffer_current) //If they have something to send
                            {
                                //Set device as the current device
                                FIFOI2C_currentDevice = i;
                                //Turn back on FIFOI2C
                                FIFOI2C_isRunning = 1;
                                //Kick-Start Operation
                                INTSetFlag(INT_I2C2M);
                            }
                        }
                        i++;
                    }

                    //Done so quite routine.
                    return;
                }
                else
                {
                    //Keep it running
                    INTSetFlag(INT_I2C2M);
                }

                break;

            default:
                break;
        }

        //Increment next byte to interpret
        FIFOI2C_Devices_List[FIFOI2C_currentDevice].transmit_buffer_current++;
    }

}

