//Copyright @ Adys Tech
//Author : mvadu@adystech.com

#include <Arduino.h>
#include "Register.h"

namespace TrimmingRegisters
{
const uint8_t T1 = 0x88;
const uint8_t T2 = 0x8A;
const uint8_t T3 = 0x8C;

const uint8_t P1 = 0x8E;
const uint8_t P2 = 0x90;
const uint8_t P3 = 0x92;
const uint8_t P4 = 0x94;
const uint8_t P5 = 0x96;
const uint8_t P6 = 0x98;
const uint8_t P7 = 0x9A;
const uint8_t P8 = 0x9C;
const uint8_t P9 = 0x9E;

const uint8_t H1 = 0xA1;
const uint8_t H2 = 0xE1;
const uint8_t H3 = 0xE3;
const uint8_t H4_MSB = 0xE4;
const uint8_t H4_H5_Shared = 0xE5;
const uint8_t H5_MSB = 0xE6;
const uint8_t H6 = 0xE7;
};

class SensorCalibration : public Register
{
public:
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;

  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;

  uint8_t dig_H1;
  int16_t dig_H2;
  uint8_t dig_H3;
  int16_t dig_H4;
  int16_t dig_H5;
  uint8_t dig_H6;

  SensorCalibration(uint8_t deviceAddress) : Register(deviceAddress, TrimmingRegisters::T1, 32) {}
  bool Load();
};
