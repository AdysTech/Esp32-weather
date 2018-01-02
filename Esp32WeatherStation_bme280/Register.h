//Copyright @ Adys Tech
//Author : mvadu@adystech.com

#include <Arduino.h>
#ifndef REG_H
#define REG_H

class Register
{

  public:
    Register(const uint8_t deviceAddress, const uint8_t regAddress, const uint8_t len = 1);

    //readRegister reads one register
    virtual bool Load();

    //Writes a byte
    virtual bool Save();

    virtual uint8_t get();
    virtual void set(uint8_t val);
    uint8_t StartAddress;
    uint8_t Length;

  protected:
    //ReadRegion read a chunk of memory into an array.
    uint8_t readRegion(uint8_t *arr, uint8_t addr, uint8_t len);

    //readRegister reads one register
    uint8_t readRegister(uint8_t address);

    //Reads two regs, LSByte then MSByte order, and returns uint16_t
    int16_t readRegisterInt16(uint8_t address);

    uint8_t _deviceAddress;
    uint8_t _value;
};

#endif