#include "max30100.h"
#include "mgm240pb32vnn.h"
#include "sl_i2cspm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Set up I2C, (SDA,SCL)

#define MAX30100_I2C_BUS_ADDRESS 0x00

I2C_TypeDef *i2c;
static int8_t i2c_bus_read(uint8_t devAddr, uint8_t regAddr, uint8_t *regData, uint8_t count);
static bool read_register(uint8_t addr, uint8_t *regData);
static int8_t i2c_bus_write(uint8_t devAddr, uint8_t regAddr, uint8_t *regData, uint8_t count);




static int8_t i2c_bus_read(uint8_t devAddr, uint8_t regAddr, uint8_t *regData, uint8_t count)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = devAddr << 1;
  seq.flags = I2C_FLAG_WRITE_READ;

  seq.buf[0].len  = 1;
  seq.buf[0].data = &regAddr;
  seq.buf[1].len  = count;
  seq.buf[1].data = regData;

  ret = I2CSPM_Transfer(i2c, &seq);

  if (ret != i2cTransferDone) {
    return -1;
  }

  return 0;
}

static bool read_register(uint8_t addr, uint8_t *regData)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = MAX30100_I2C_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE_READ;

  seq.buf[0].len  = 1;
  seq.buf[0].data = &addr;
  seq.buf[1].len  = 1;
  seq.buf[1].data = regData;

  ret = I2CSPM_Transfer(i2c, &seq);
  if (ret != i2cTransferDone) {
    return 0;;
  }

  return 1;
}



static int8_t i2c_bus_write(uint8_t devAddr, uint8_t regAddr, uint8_t *regData, uint8_t count)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = devAddr << 1;
  seq.flags = I2C_FLAG_WRITE_WRITE;

  seq.buf[0].len  = 1;
  seq.buf[0].data = &regAddr;
  seq.buf[1].len  = count;
  seq.buf[1].data = regData;

  ret = I2CSPM_Transfer(i2c, &seq);
  if (ret != i2cTransferDone) {
    return -1;
  }

  return 0;
}





uint16_t IR = 0;      // Last IR reflectance datapoint
uint16_t RED = 0;     // Last Red reflectance datapoint

//Wire read and write protocols
int i2c_write (uint8_t i2c_addr, uint8_t register_addr, uint8_t* buffer, uint8_t Nbyte )
{
    int8_t ret;
    ret = i2c_bus_write(i2c_addr, register_addr, buffer, Nbyte);
    return ret;
}

int i2c_read (uint8_t i2c_addr, uint8_t register_addr, uint8_t* buffer, uint8_t Nbyte )
{
    int ret;

    /* Send device address, with no STOP condition */
    ret = i2c_bus_write(i2c_addr, register_addr, buffer, Nbyte);
    if(!ret) {
        /* Read data, with STOP condition  */
        i2c_bus_read((i2c_addr|0x01), register_addr, buffer, Nbyte);
    }
    return ret;
}
//
   
void setLEDs(pulseWidth pw, ledCurrent red, ledCurrent ir){
  char reg[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1);
  reg[0] = reg[0] & 0xFC; // Set LED_PW to 00
  reg[0] = reg[0] | pw;
  i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1);     // Mask LED_PW
  reg[0] = (red<<4) | ir;
  i2c_write(MAX30100_ADDRESS, MAX30100_LED_CONFIG, &reg[0], 1); // write LED configs
}

void setSPO2(sampleRate sr){
  char reg[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1);
  reg[0] = reg[0] & 0xE3; // Set SPO2_SR to 000
  reg[0] = reg[0] | (sr<<2);
  i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1); // Mask SPO2_SR
  i2c_read(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);
  reg[0] = reg[0] & 0xf8; // Set Mode to 000
  reg[0] = reg[0] | 0x03;
  i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1); // Mask MODE
}

int getNumSamp(void){
    char wrPtr[1];
    char rdPtr[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_WR_PTR, &wrPtr[0], 1);
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_RD_PTR, &rdPtr[0], 1);
    return (abs( 16 + wrPtr[0] - rdPtr[0] ) % 16);
}

void readSensor(void){
  char temp[4] = {0};  // Temporary buffer for read values
  i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_DATA, &temp[0], 4);  // Read four times from the FIFO
  IR = (temp[0]<<8) | temp[1];    // Combine values to get the actual number
  RED = (temp[2]<<8) | temp[3];   // Combine values to get the actual number
}

void shutdown(void){
  char reg[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);  // Get the current register
  reg[0] = reg[0] | 0x80;
  i2c_write(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);   // mask the SHDN bit
}

void reset(void){
  char reg[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);  // Get the current register
  reg[0] = reg[0] | 0x40;
  i2c_write(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);   // mask the RESET bit
}

void startup(void){
  char reg[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);  // Get the current register
  reg[0] = reg[0] & 0x7F;
  i2c_write(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);   // mask the SHDN bit
}

uint8_t getRevID(void){
  char buffer[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_REV_ID, &buffer[0], 1);
  return buffer[0];
}

uint8_t getPartID(void){
  char buffer[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_PART_ID, &buffer[0], 1);
  return buffer[0];
}

void begin(pulseWidth pw, ledCurrent ir, sampleRate sr){
  char buffer[1];
  buffer[0] = 0x02;
  i2c_write(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &buffer[0], 1); // Heart rate only
  buffer[0] = ir;
  i2c_write(MAX30100_ADDRESS, MAX30100_LED_CONFIG, &buffer[0], 1);
  buffer[0] = (sr<<2)|pw;
  i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &buffer[0], 1);
}

