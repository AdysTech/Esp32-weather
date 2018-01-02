//Copyright @ Adys Tech
//Author : mvadu@adystech.com

#include "Register.h"
#include <Wire.h>

Register::Register(uint8_t deviceAddress, uint8_t regAddress, uint8_t len)
{
    StartAddress = regAddress;
    Length = len;
    _deviceAddress = deviceAddress;
    _value = 0;
}

bool Register::Save(void)
{
    Wire.beginTransmission(_deviceAddress);
    Wire.write(StartAddress);
    uint8_t val = this->get();
    // Serial.print("Write: ");Serial.print(StartAddress,HEX);Serial.print(" Val:");Serial.println(val,BIN);
    Wire.write(val);
    Wire.endTransmission();
    return true;
}

bool Register::Load()
{
    if (readRegion(&_value, StartAddress, Length) == Length)
        return true;
    return false;
}

uint8_t Register::readRegister(uint8_t address)
{
    uint8_t val = 0;
    if (readRegion(&val, address, 1) == 1)
        return val;
    return 0;
}

int16_t Register::readRegisterInt16(uint8_t address)
{
    uint8_t myBuffer[2];
    if (readRegion(myBuffer, address, 2) == 2)
    {
        int16_t output = (int16_t)myBuffer[0] | int16_t(myBuffer[1] << 8);
        return output;
    }
    return 0;
}

uint8_t Register::readRegion(uint8_t *arrPtr, uint8_t addr, uint8_t len)
{
    uint8_t i = 0;
    uint8_t val = 0;
    Wire.beginTransmission(_deviceAddress);
    Wire.write(addr);
    Wire.endTransmission();
    // request bytes from slave device
    Wire.requestFrom(_deviceAddress, len);
    while ((Wire.available()) && (i < len)) // slave may send less than requested
    {
        *arrPtr = Wire.read(); // receive a byte
        arrPtr++;
        i++;
    }
    return i;
}

uint8_t Register::get()
{
    return _value;
}

void Register::set(uint8_t val)
{
    _value = val;
}
