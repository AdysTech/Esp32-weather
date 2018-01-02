//Copyright @ Adys Tech
//Author : mvadu@adystech.com

#include "OutputRegisters.h"
#include "ConfigRegisters.h"

bool OutputRegisters::Load()
{

    uint8_t data[MeasurementLength];

    for (char i = 0; i < MeasurementLength; i++)
    {
        data[i] = 0;
    }

    CtrlHumidityReg humidity(_deviceAddress);
    humidity.Load();

    CtrlMeasureReg measure(_deviceAddress);
    measure.Load();
    measure.RunMode = SensorMode::Forced;

    StatusReg stat(_deviceAddress);
    uint8_t measureTimeMax = (1.25 + (2.3 * measure.TempOverSample) + (2.3 * measure.PressOverSample + 0.575) + (2.3 * humidity.HumidityOverSample + 0.575));

    //initiate the measurement
    measure.Save();
    unsigned long start = millis();
    while (stat.Load() && (stat.get() == 0) && millis() - start < measureTimeMax)
        delay(1);
    //delay(10);
    while (stat.Load() && ((stat.get() & StatusReg_IMUpdate) == StatusReg_IMUpdate || (stat.get() & StatusReg_Measuring) == StatusReg_Measuring) && millis() - start < measureTimeMax)
        delay(5);
    Serial.printf("measureTimeMax: %d, measureTime: %d \n", measureTimeMax, millis() - start);

    //delay(300);

    // while (stat.Load() && ((stat.get() & StatusReg_Measuring) == StatusReg_Measuring)) delay(2);
    if (readRegion(data, MeasurementRegion, MeasurementLength) != MeasurementLength)
    {
        Serial.println("returned false");
        return false;
    }

    if ((data[0] == 0x80) && (data[3] == 0x80) && (data[6] == 0x80))
    {
        Serial.println("not measured");
        return false;
    }

    AdcPressure = -1;
    AdcTemperature = -1;
    AdcHumidity = -1;

    if (data[0] != 0x80)
    {
        AdcPressure = (int32_t)((((uint32_t)data[0]) << 12) | (((uint32_t)data[1]) << 4) | (((uint32_t)data[2] & 0xF0) >> 4));
    }
    if (data[3] != 0x80)
    {
        AdcTemperature = (int32_t)((((uint32_t)data[3]) << 12) | (((uint32_t)data[4]) << 4) | (((uint32_t)data[5] & 0xF0) >> 4));
    }
    if (data[6] != 0x80)
    {
        AdcHumidity = (int32_t)((((uint32_t)data[6]) << 8) + ((uint32_t)data[7]));
    }
    return true;
}
