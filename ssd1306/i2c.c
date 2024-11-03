#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

#include "i2c.h"

struct i2c_device {
  int sda;
  int scl;
};

i2c_device *i2c_open(int sda, int scl) {
  static uint8_t wiringPiSetupCalled = 0;
  if (!wiringPiSetupCalled) {
    wiringPiSetup();
    wiringPiSetupCalled = 1;
  }

  i2c_device *device = malloc(sizeof(i2c_device));
  *device = (i2c_device){sda, scl};
  pinMode(sda, OUTPUT);
  pinMode(scl, OUTPUT);
  pullUpDnControl(sda, PUD_UP);
  pullUpDnControl(scl, PUD_UP);
  return device;
}

void i2c_start(const i2c_device *device) {
  digitalWrite(device->sda, HIGH);
  digitalWrite(device->scl, HIGH);
  delayMicroseconds(1);
  digitalWrite(device->sda, LOW);
  delayMicroseconds(1);
  digitalWrite(device->scl, LOW);
}

void i2c_stop(const i2c_device *device) {
  digitalWrite(device->sda, LOW);
  digitalWrite(device->scl, HIGH);
  delayMicroseconds(1);
  digitalWrite(device->sda, HIGH);
  delayMicroseconds(1);
}

int i2c_get_ack(const i2c_device *device) {
  digitalWrite(device->sda, HIGH);
  delayMicroseconds(1);
  digitalWrite(device->scl, HIGH);
  pinMode(device->sda, INPUT);
  delayMicroseconds(1);
  int ack = digitalRead(device->sda) == LOW;
  pinMode(device->sda, OUTPUT);
  digitalWrite(device->scl, LOW);
  delayMicroseconds(1);
  return ack;
}

void i2c_send_byte(const i2c_device *device, uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(device->sda, byte & 0x80 ? HIGH : LOW);
    byte <<= 1;
    delayMicroseconds(1);
    digitalWrite(device->scl, HIGH);
    delayMicroseconds(1);
    digitalWrite(device->scl, LOW);
    delayMicroseconds(1);
  }
}

void i2c_close(i2c_device *device) { free(device); }
