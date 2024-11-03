#ifndef _I2C_H
#define _I2C_H

#include <stdint.h>

typedef struct i2c_device i2c_device;

i2c_device *i2c_open(int sda, int scl);
void i2c_start(const i2c_device *device);
void i2c_stop(const i2c_device *device);
int i2c_get_ack(const i2c_device *device);
void i2c_send_byte(const i2c_device *device, uint8_t byte);
void i2c_close(i2c_device *device);

#endif
