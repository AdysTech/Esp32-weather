//Copyright @ Adys Tech
//Author : mvadu@adystech.com

#include <Arduino.h>
#include "Register.h"

const uint8_t MeasurementRegion = 0xF7;
const uint8_t MeasurementLength = 8;
const uint8_t StatusReg_Measuring = 0b00001000;
const uint8_t StatusReg_IMUpdate = 0b00000001;

class OutputRegisters : public Register
{
public:
  OutputRegisters(uint8_t deviceAddress) : Register(deviceAddress, MeasurementRegion, MeasurementLength) {}
  int32_t AdcPressure, AdcTemperature, AdcHumidity;
  bool Load();
};
