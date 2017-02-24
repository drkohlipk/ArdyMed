/*
	ELECHOUSE_CC1101.cpp - CC1101 module library
	Copyright (c) 2010 Michael.
 Author: Michael, <www.elechouse.com>
 Version: November 12, 2010
 
	This library is designed to use CC1101/CC1100 module on Arduino platform.
	CC1101/CC1100 module is an useful wireless module.Using the functions of the
	library, you can easily send and receive data by the CC1101/CC1100 module.
	Just have fun!
	For the details, please refer to the datasheet of CC1100/CC1101.
 */
#include "ELECHOUSE_CC1101.h"
#include <Arduino.h>

/****************************************************************/
#define 	WRITE_BURST     	0x40						//write burst
#define 	READ_SINGLE     	0x80						//read single
#define 	READ_BURST      	0xC0						//read burst
#define 	BYTES_IN_RXFIFO     0x7F  						//byte number in RXfifo

/****************************************************************/
byte PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};

/****************************************************************
 *FUNCTION NAME:SpiInit
 *FUNCTION     :spi communication initialization
 *INPUT        :none
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::SpiInit(void)
{
    // initialize the SPI pins
    pinMode(SCK_PIN, OUTPUT);
    pinMode(MOSI_PIN, OUTPUT);
    pinMode(MISO_PIN, INPUT);
    pinMode(SS_PIN, OUTPUT);
    
    // enable SPI Master, MSB, SPI mode 0, FOSC/4
    SpiMode(0);
}
/****************************************************************
 *FUNCTION NAME:SpiMode
 *FUNCTION     :set spi mode
 *INPUT        :        config               mode
 (0<<CPOL) | (0 << CPHA)		 0
 (0<<CPOL) | (1 << CPHA)		 1
 (1<<CPOL) | (0 << CPHA)		 2
 (1<<CPOL) | (1 << CPHA)		 3
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::SpiMode(byte config)
{
    byte tmp;
    
    // enable SPI master with configuration byte specified
    SPCR = 0;
    SPCR = (config & 0x7F) | (1<<SPE) | (1<<MSTR);
    tmp = SPSR;
    tmp = SPDR;
}

/****************************************************************
 *FUNCTION NAME:SpiTransfer
 *FUNCTION     :spi transfer
 *INPUT        :value: data to send
 *OUTPUT       :data to receive
 ****************************************************************/
byte ELECHOUSE_CC1101::SpiTransfer(byte value)
{
    SPDR = value;
    while (!(SPSR & (1<<SPIF))) ;
    return SPDR;
}

/****************************************************************
 *FUNCTION NAME: GDO_Set()
 *FUNCTION     : set GDO0,GDO2 pin
 *INPUT        : none
 *OUTPUT       : none
 ****************************************************************/
void ELECHOUSE_CC1101::GDO_Set (void)
{
    pinMode(GDO0, INPUT_PULLUP);
    pinMode(GDO2, INPUT_PULLUP);
}

/****************************************************************
 *FUNCTION NAME:Reset
 *FUNCTION     :CC1101 reset //details refer datasheet of CC1101/CC1100//
 *INPUT        :none
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::Reset (void)
{
    digitalWrite(SS_PIN, LOW);
    delay(1);
    digitalWrite(SS_PIN, HIGH);
    delay(1);
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
//    Serial.println(digitalRead(MISO_PIN));
    SpiTransfer(CC1101_SRES);
//    while(digitalRead(MISO_PIN));
    digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
 *FUNCTION NAME:SpiWriteReg
 *FUNCTION     :CC1101 write data to register
 *INPUT        :addr: register address; value: register value
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::SpiWriteReg(byte addr, byte value)
{
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
    SpiTransfer(addr);
    SpiTransfer(value);
    digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
 *FUNCTION NAME:SpiWriteBurstReg
 *FUNCTION     :CC1101 write burst data to register
 *INPUT        :addr: register address; buffer:register value array; num:number to write
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::SpiWriteBurstReg(byte addr, byte *buffer, byte num)
{
    byte i, temp;
    
    temp = addr | WRITE_BURST;
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
    SpiTransfer(temp);
    for (i = 0; i < num; i++)
    {
        SpiTransfer(buffer[i]);
    }
    digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
 *FUNCTION NAME:SpiStrobe
 *FUNCTION     :CC1101 Strobe
 *INPUT        :strobe: command; //refer define in CC1101.h//
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::SpiStrobe(byte strobe)
{
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
    SpiTransfer(strobe);
    digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
 *FUNCTION NAME:SpiReadReg
 *FUNCTION     :CC1101 read data from register
 *INPUT        :addr: register address
 *OUTPUT       :register value
 ****************************************************************/
byte ELECHOUSE_CC1101::SpiReadReg(byte addr)
{
    byte temp, value;
    
    temp = addr|READ_SINGLE;
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
    SpiTransfer(temp);
    value=SpiTransfer(0);
    digitalWrite(SS_PIN, HIGH);
    
    return value;
}

/****************************************************************
 *FUNCTION NAME:SpiReadBurstReg
 *FUNCTION     :CC1101 read burst data from register
 *INPUT        :addr: register address; buffer:array to store register value; num: number to read
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::SpiReadBurstReg(byte addr, byte *buffer, byte num)
{
    byte i,temp;
    
    temp = addr | READ_BURST;
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
    SpiTransfer(temp);
    for(i=0;i<num;i++)
    {
        buffer[i]=SpiTransfer(0);
    }
    digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
 *FUNCTION NAME:SpiReadStatus
 *FUNCTION     :CC1101 read status register
 *INPUT        :addr: register address
 *OUTPUT       :status value
 ****************************************************************/
byte ELECHOUSE_CC1101::SpiReadStatus(byte addr)
{
    byte value,temp;
    
    temp = addr | READ_BURST;
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
    SpiTransfer(temp);
    value=SpiTransfer(0);
    digitalWrite(SS_PIN, HIGH);
    
    return value;
}

/****************************************************************
 *FUNCTION NAME:RegConfigSettings
 *FUNCTION     :CC1101 register config //details refer datasheet of CC1101/CC1100//
 *INPUT        :none
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::RegConfigSettings(int num)
{
    SpiStrobe(CC1101_SRES); //Reset
    delay(5);
    SpiStrobe(CC1101_SFRX); //Flush the RX FIFO
    //These are common settings
    SpiWriteReg(CC1101_FSCTRL1, 0x06);
    SpiWriteReg(CC1101_FSCTRL0, 0x00);
    SpiWriteReg(CC1101_FREQ2, 0x0F);
    SpiWriteReg(CC1101_FREQ1, 0x77);
    SpiWriteReg(CC1101_FREQ0, 0x8D);
    SpiWriteReg(CC1101_CHANNR, 0x00);
    SpiWriteReg(CC1101_DEVIATN, 0x15);
    SpiWriteReg(CC1101_FSCAL3, 0xE9);
    SpiWriteReg(CC1101_FSCAL2, 0x2A);
    SpiWriteReg(CC1101_FSCAL1, 0x00);
    SpiWriteReg(CC1101_FSCAL0, 0x1F);
    SpiWriteReg(CC1101_FSTEST, 0x59);
    SpiWriteReg(CC1101_PKTLEN, 0x50);
    SpiWriteReg(CC1101_ADDR, 0x00);
    SpiWriteReg(CC1101_MDMCFG1, 0x02);
    SpiWriteReg(CC1101_PKTCTRL1, 0x00);
    SpiWriteReg(CC1101_FIFOTHR, 0x07);
    SpiWriteReg(CC1101_MDMCFG3, 0x4A);
    SpiWriteReg(CC1101_MDMCFG2, 0x33); //B4
    SpiWriteReg(CC1101_MDMCFG0, 0x18); //F8
    SpiWriteReg(CC1101_MCSM2 , 0x07);
    SpiWriteReg(CC1101_MCSM1, 0x30);
    SpiWriteReg(CC1101_MCSM0 , 0x18);
    SpiWriteReg(CC1101_FOCCFG, 0x16);
    SpiWriteReg(CC1101_BSCFG, 0x6C);
    SpiWriteReg(CC1101_FREND1, 0x56); // Wide BW = B6, Narrow 56
    SpiWriteReg(CC1101_FREND0, 0x11);
    SpiWriteReg(CC1101_TEST2, 0x81); // Wide BW = 88, Narrow 81
    SpiWriteReg(CC1101_TEST1, 0x35); // Wide BW = 31, Narrow 35
    SpiWriteReg(CC1101_AGCCTRL2, 0xA4); //A4
    SpiWriteReg(CC1101_AGCCTRL1, 0x50); //50
    SpiWriteReg(CC1101_AGCCTRL0, 0x92); //92
    SpiWriteReg(CC1101_MDMCFG4, 0x58); //58 Bandwidth
    if (num == 1)
    {
        Serial.println("Setting Dexcom FIFO");
        SpiWriteReg(CC1101_PKTCTRL0, 0x00);
        SpiWriteReg(CC1101_IOCFG2, 0x09); //Carrier Sense
        SpiWriteReg(CC1101_IOCFG0, 0x00); // RX FIFO
        SpiWriteReg(CC1101_SYNC1, 0xAD);
        SpiWriteReg(CC1101_SYNC0, 0xEB);
        SpiWriteReg(CC1101_MDMCFG4, 0xC8);
        SpiWriteReg(CC1101_MDMCFG3, 0x4A);
        SpiWriteReg(CC1101_MDMCFG2, 0x34);
        SpiWriteReg(CC1101_MDMCFG1, 0x02);
        SpiWriteReg(CC1101_MDMCFG0, 0xF8);
        SpiStrobe(CC1101_SRX);
    }
    else if (num == 2)
    {
        Serial.println("Setting Direct GP0=Clock GP2=Data");
        SpiWriteReg(CC1101_PKTCTRL0, 0x12);
        SpiWriteReg(CC1101_IOCFG2, 0x0C);
        SpiWriteReg(CC1101_IOCFG1, 0x2E); //0C = Data, 0B = Clock
        SpiWriteReg(CC1101_IOCFG0, 0x0B);
        //Other from Doc
        SpiWriteReg(CC1101_MDMCFG4, 0xC8);
        SpiWriteReg(CC1101_MDMCFG3, 0x4A);
        SpiWriteReg(CC1101_MDMCFG2, 0x30);
        SpiWriteReg(CC1101_MDMCFG1, 0x42);
        SpiWriteReg(CC1101_MDMCFG0, 0xF8);
        SpiStrobe(CC1101_SRX);
    }
    else if (num == 3)
    {
        Serial.println("Async GP0");
        SpiWriteReg(CC1101_PKTCTRL0, 0x30);
        SpiWriteReg(CC1101_IOCFG0, 0x0D);
        //Other from Doc
        SpiWriteReg(CC1101_MDMCFG4, 0xC8);
        SpiWriteReg(CC1101_MDMCFG3, 0x4A);
        SpiWriteReg(CC1101_MDMCFG2, 0x30);
        SpiWriteReg(CC1101_MDMCFG1, 0x42);
        SpiWriteReg(CC1101_MDMCFG0, 0xF8);
        SpiStrobe(CC1101_SRX);
    }
    else if (num == 4)
    {
        Serial.println("Setting Direct Hard GP0=Clock GP2=Data");
        SpiWriteReg(CC1101_PKTCTRL0, 0x12);
        SpiWriteReg(CC1101_IOCFG2, 0x17);
        SpiWriteReg(CC1101_IOCFG1, 0x2E); //0C = Data, 0B = Clock
        SpiWriteReg(CC1101_IOCFG0, 0x1D);
        //Other from Doc
        SpiWriteReg(CC1101_MDMCFG4, 0xC8);
        SpiWriteReg(CC1101_MDMCFG3, 0x4A);
        SpiWriteReg(CC1101_MDMCFG2, 0x30);
        SpiWriteReg(CC1101_MDMCFG1, 0x42);
        SpiWriteReg(CC1101_MDMCFG0, 0xF8);
        SpiStrobe(CC1101_SRX);
    }
    else if (num == 5)
    {
        Serial.println("Transmit test");
        SpiWriteReg(CC1101_PKTCTRL0, 0x00);
        SpiWriteReg(CC1101_SYNC1, 0xFF);
        SpiWriteReg(CC1101_SYNC0, 0xFF);
        SpiWriteReg(CC1101_MDMCFG4, 0xC8);
        SpiWriteReg(CC1101_MDMCFG3, 0x4A);
        SpiWriteReg(CC1101_MDMCFG2, 0x35); // 35 = 15/16 +CS; 34 = CS;
        SpiWriteReg(CC1101_MDMCFG1, 0x02); // Preamble 0=2 bytes;1=3 bytes
        SpiWriteReg(CC1101_MDMCFG0, 0xF8);
        SpiWriteReg(CC1101_IOCFG2, 0x06); //Sync
        SpiWriteReg(CC1101_IOCFG0, 0x00); // RX FIFO
        byte i;
        byte TX_buffer[4] = {0};
        for (i = 0; i < 4; i++)
        {
            TX_buffer[i] = 0xAA;
        }
        Serial.println("Transmit Start");
        SendData(TX_buffer, 4);
        Serial.println("Transmit Complete");
    }
    else if (num == 6)
    {
        Serial.println("OneTouch Direct GP0=Clock GP2=Data");
        SpiWriteReg(CC1101_PKTCTRL0, 0x12);
        SpiWriteReg(CC1101_IOCFG2, 0x0C);
        SpiWriteReg(CC1101_IOCFG1, 0x2E); //0C = Data, 0B = Clock
        SpiWriteReg(CC1101_IOCFG0, 0x0B);
        //Switch to 916 mhz
        SpiWriteReg(CC1101_FREQ2, 0x23);
        SpiWriteReg(CC1101_FREQ1, 0x40);
        SpiWriteReg(CC1101_FREQ0, 0x00);
        //Other from Doc
        SpiWriteReg(CC1101_MDMCFG4, 0xC8);
        SpiWriteReg(CC1101_MDMCFG3, 0x4A);
        SpiWriteReg(CC1101_MDMCFG2, 0x30);
        SpiWriteReg(CC1101_MDMCFG1, 0x42);
        SpiWriteReg(CC1101_MDMCFG0, 0xF8);
        SpiStrobe(CC1101_SRX);
    }
    else
    {
        Serial.println("No Config");
    }
}

/****************************************************************
 *FUNCTION NAME:SendData
 *FUNCTION     :use CC1101 send data
 *INPUT        :txBuffer: data array to send; size: number of data to send, no more than 61
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::SendData(byte *txBuffer,byte size)
{
    SpiWriteReg(CC1101_TXFIFO,size);
    SpiWriteBurstReg(CC1101_TXFIFO,txBuffer,size);			//write data to send
    SpiStrobe(CC1101_STX);									//start send
    while (!digitalRead(GDO0));								// Wait for GDO0 to be set -> sync transmitted
    while (digitalRead(GDO0));								// Wait for GDO0 to be cleared -> end of packet
    SpiStrobe(CC1101_SFTX);									//flush TXfifo
}

/****************************************************************
 *FUNCTION NAME:SetReceive
 *FUNCTION     :set CC1101 to receive state
 *INPUT        :none
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::SetReceive(void)
{
    SpiStrobe(CC1101_SRX);
}

/****************************************************************
 *FUNCTION NAME:CheckReceiveFlag
 *FUNCTION     :check receive data or not
 *INPUT        :none
 *OUTPUT       :flag: 0 no data; 1 receive data
 ****************************************************************/
byte ELECHOUSE_CC1101::CheckReceiveFlag(void)
{
    if(digitalRead(GDO0))			//receive data
    {
        while (digitalRead(GDO0));
        return 1;
    }
    else							// no data
    {
        return 0;
    }
}


/****************************************************************
 *FUNCTION NAME:ReceiveData
 *FUNCTION     :read data received from RXfifo
 *INPUT        :rxBuffer: buffer to store data
 *OUTPUT       :size of data received
 ****************************************************************/
byte ELECHOUSE_CC1101::ReceiveData(byte *rxBuffer)
{
    byte size;
    byte status[2];
    
    if(SpiReadStatus(CC1101_RXBYTES) & BYTES_IN_RXFIFO)
    {
        size=SpiReadReg(CC1101_RXFIFO);
        SpiReadBurstReg(CC1101_RXFIFO,rxBuffer,size);
        SpiReadBurstReg(CC1101_RXFIFO,status,2);
        SpiStrobe(CC1101_SFRX);
        return size;
    }
    else
    {
        SpiStrobe(CC1101_SFRX);
        return 0;
    }
    
}

ELECHOUSE_CC1101 ELECHOUSE_cc1101;


/****************************************************************
 *FUNCTION NAME:Init
 *FUNCTION     :CC1101 initialization
 *INPUT        :number correlating to device selection
 *OUTPUT       :none
 ****************************************************************/
void ELECHOUSE_CC1101::Init(int n)
{
    SpiInit();										//spi initialization
    GDO_Set();										//GDO set
    digitalWrite(SS_PIN, HIGH);
    digitalWrite(SCK_PIN, HIGH);
    digitalWrite(MOSI_PIN, LOW);
    Reset();										//CC1101 reset
    RegConfigSettings(n);							//CC1101 register config
    SpiWriteBurstReg(CC1101_PATABLE,PaTabel,8);		//CC1101 PATABLE config
}
