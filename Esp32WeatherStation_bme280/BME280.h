//Copyright @ Adys Tech
//Author : mvadu@adystech.com

#include <Arduino.h>
#include "SensorCalibration.h"
#include "ConfigRegisters.h"
#include "OutputRegisters.h"

namespace CommandKeywords
{
const uint8_t Soft_Reset = 0xB6;
const uint8_t ValidChipID = 0x60;
}

class BME280
{
public:
  const uint8_t I2CAddress_SDO_GND = 0x76;
  const uint8_t I2CAddress_SDO_VDD = 0x77;

  BME280();
  //~BME280() = default;

  //initialize the sensor, with ESP32 MUX for sda/scl
  bool begin(int sda = -1, int scl = -1, uint32_t frequency = 100000);

  //Software reset routine
  void reset(void);

  //Primary read functions
  float getPressure(void);
  float getHumidity(void);
  float getTemperature(void);
  float getDewPoint();
  float getHeatIndex();

private:
  OutputRegisters _output;
  SensorCalibration _calibration;
  int32_t t_fine;

  float calculateHumidity();
  float calculateTemperature();
  float calculatePressure();
  float calculateDewPoint();
  float calculateHeatIndex();
  bool refresh();

  bool _reset;
  float _temp;
  float _humidity;
  float _dewpoint;
  float _heatindex;
  float _pressure;
  unsigned long _lastRefresh;
};
