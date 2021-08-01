
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SOFT_I2C_H
#define	SOFT_I2C_H
 
    void i2cInit(void);
    unsigned char i2cWriteByte(unsigned char send_start, unsigned char send_stop, unsigned char byte);
    unsigned char i2cReadByte(unsigned char nack, unsigned char send_stop);
 
#endif	/* SOFT_I2C_H */

