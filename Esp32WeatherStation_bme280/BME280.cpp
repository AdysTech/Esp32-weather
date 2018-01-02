//Copyright @ Adys Tech
//Author : mvadu@adystech.com

#include <Wire.h>
#include "BME280.h"
#include <math.h>

BME280::BME280(void) : _output(I2CAddress_SDO_GND), _calibration(I2CAddress_SDO_GND)
{
    _reset = NAN;
    _temp = NAN;
    _humidity = NAN;
    _dewpoint = NAN;
    _heatindex = NAN;
    _pressure = NAN;
    _lastRefresh = 0xFFFFFFFF;
}


///initializes the Bosch BME280 sensor for I2C with given ESP32 IO pins used for SDA and SCL.
bool BME280::begin(int sda, int scl, uint32_t frequency)
{
    Wire.begin(sda, scl, frequency);
    // check if sensor, i.e. the chip ID is correct
    IdReg id(I2CAddress_SDO_GND);

    if (id.Load() != true || (id.get() != CommandKeywords::ValidChipID))
    {
        Serial.printf("ID %d, expected %d \n", id.get(), CommandKeywords::ValidChipID);
        return false;
    }

    // reset the device using soft-reset, this makes sure the IIR is off, etc.
    ResetReg reset(I2CAddress_SDO_GND);
    reset.set(CommandKeywords::Soft_Reset);
    reset.Save();

    // wait for chip to wake up.
    delay(50);

    _calibration.Load();

    //first make the sensor to sleep
    CtrlHumidityReg humidity(I2CAddress_SDO_GND);
    humidity.HumidityOverSample = OverSampling::X1;
    humidity.Save();

    ConfigReg config(I2CAddress_SDO_GND);
    config.Load();
    config.IIR_Filter = FilterCoefficients::IIR_Off;
    config.SleepTime = StandbyDuration::ms_0_5;
    config.Save();

    CtrlMeasureReg measure(I2CAddress_SDO_GND);
    measure.TempOverSample = OverSampling::X1;
    measure.PressOverSample = OverSampling::X1;
    measure.RunMode = SensorMode::Sleep;
    measure.Save();
    delay(10);

    //take the initial reading
    refresh();

    return true;
}

bool BME280::refresh()
{
    unsigned long currentMillis = millis();
    //detect timer roll over
    if (isnan(_humidity) || isnan(_temp) || isnan(_pressure) || currentMillis < _lastRefresh || currentMillis - _lastRefresh > 1000)
    {
        if (!_output.Load())
            return false;

        _temp = calculateTemperature();
        _humidity = calculateHumidity();
        _pressure = calculatePressure();

        //dump all registers in output and config region
        // uint8_t addr = 0xF2;
        // Wire.beginTransmission(I2CAddress_SDO_GND);
        // Wire.write(addr);
        // Wire.endTransmission();
        // // request bytes from slave device
        // Wire.requestFrom(I2CAddress_SDO_GND, 13);
        // while ((Wire.available()) && (addr < 0xFF)) // slave may send less than requested
        // {
        //     Serial.print("Address: "); Serial.print(addr,HEX);
        //     Serial.print(" Value: "); Serial.println(Wire.read(),BIN); // receive a byte
        //     addr++;
        // }

        bool stat = !(isnan(_humidity) || isnan(_temp) || isnan(_pressure));
        if (stat)
        {
            _lastRefresh = currentMillis;
            _dewpoint = calculateDewPoint();
            _heatindex = calculateHeatIndex();
        }

        return stat;
    }
    return true;
}

float BME280::getTemperature()
{
    if (isnan(_temp))
    {
        refresh();
    }
    return _temp;
}

float BME280::calculateTemperature()
{
    if (_output.AdcTemperature == -1)
        return NAN;

    //By datasheet, calibrate
    int64_t var1, var2;
    var1 = ((((_output.AdcTemperature >> 3) - ((int32_t)_calibration.dig_T1 << 1))) * ((int32_t)_calibration.dig_T2)) >> 11;
    var2 = (((((_output.AdcTemperature >> 4) - ((int32_t)_calibration.dig_T1)) * ((_output.AdcTemperature >> 4) - ((int32_t)_calibration.dig_T1))) >> 12) *
            ((int32_t)_calibration.dig_T3)) >>
           14;

    t_fine = var1 + var2;
    float output = (t_fine * 5 + 128) >> 8;
    output = output / 100;

    return output;
}


float BME280::getPressure()
{
    if (isnan(_pressure))
    {
        refresh();
    }
    return _pressure;
}

// Returns pressure in hPA
float BME280::calculatePressure()
{
    if (_output.AdcPressure == -1)
        return NAN;

    int64_t var1, var2, p_acc;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_calibration.dig_P6;
    var2 = var2 + ((var1 * (int64_t)_calibration.dig_P5) << 17);
    var2 = var2 + (((int64_t)_calibration.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)_calibration.dig_P3) >> 8) + ((var1 * (int64_t)_calibration.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_calibration.dig_P1) >> 33;

    if (var1 == 0)
        return NAN; //div by 0

    p_acc = 1048576 - _output.AdcPressure;
    p_acc = (((p_acc << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)_calibration.dig_P9) * (p_acc >> 13) * (p_acc >> 13)) >> 25;
    var2 = (((int64_t)_calibration.dig_P8) * p_acc) >> 19;
    p_acc = ((p_acc + var1 + var2) >> 8) + (((int64_t)_calibration.dig_P7) << 4);

    return ((float)p_acc / 256.0) / 100.0;
}


float BME280::getHumidity()
{
    if (isnan(_humidity))
    {
        refresh();
    }
    return _humidity;
}

float BME280::calculateHumidity()
{
    if (_output.AdcHumidity == -1)
        return NAN;

    int32_t var1;
    var1 = (t_fine - ((int32_t)76800));
    var1 = (((((_output.AdcHumidity << 14) - (((int32_t)_calibration.dig_H4) << 20) - (((int32_t)_calibration.dig_H5) * var1)) +
              ((int32_t)16384)) >>
             15) *
            (((((((var1 * ((int32_t)_calibration.dig_H6)) >> 10) * (((var1 * ((int32_t)_calibration.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                  ((int32_t)_calibration.dig_H2) +
              8192) >>
             14));
    var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)_calibration.dig_H1)) >> 4));
    var1 = (var1 < 0 ? 0 : var1);
    var1 = (var1 > 419430400 ? 419430400 : var1);
    return (float)(var1 >> 12) / 1024.0;
}



float BME280::getDewPoint()
{
    if (isnan(_dewpoint))
    {
        refresh();
    }
    return _dewpoint;
}

float BME280::calculateDewPoint()
{

    float dewPoint = 243.04 * (log(getHumidity() / 100.0) + ((17.625 * getTemperature()) / (243.04 + getTemperature()))) / (17.625 - log(getHumidity() / 100.0) - ((17.625 * getTemperature()) / (243.04 + getTemperature())));
    return (dewPoint);
}

float BME280::getHeatIndex()
{
    if (isnan(_heatindex))
    {
        refresh();
    }
    return _heatindex;
}

//ref: http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
float BME280::calculateHeatIndex()
{
    //convert temp to fahrehneit
    float T = 9.0 / 5.0 * getTemperature() + 32;

    //this simple formula is computed first, if it's 80 degrees F or higher, the full regression equation along with any adjustment is applied.
    float heatIndex = 0.5 * (T + 61.0 + ((T - 68.0) * 1.2) + (getHumidity() * 0.094));

    if (heatIndex > 80)
    {
        float heatIndex = -42.379 + 2.04901523 * T + 10.14333127 * getHumidity() - .22475541 * T * getHumidity() - .00683783 * T * T - .05481717 * getHumidity() * getHumidity() + .00122874 * T * T * getHumidity() + .00085282 * T * getHumidity() * getHumidity() - .00000199 * T * T * getHumidity() * getHumidity();
        if (getHumidity() < 13 && T > 80 && T < 112)
        {
            heatIndex -= ((13 - getHumidity()) / 4) * sqrt((17 - (T > 95.0 ? T - 95.0 : 95.0 - T)) / 17);
        }
        else if (getHumidity() > 85 && T > 80 && T < 87)
        {
            heatIndex += ((getHumidity() - 85) / 10) * ((87 - T) / 5);
        }
    }
    //convert back to celsius
    return (5.0 / 9.0 * (heatIndex - 32));
}
