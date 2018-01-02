//Copyright @ Adys Tech
//Author : mvadu@adystech.com

#include <Arduino.h>
#include "Register.h"

namespace ConfigRegisters
{
const uint8_t ChipId = 0xD0;
const uint8_t Soft_Reset = 0xE0;
const uint8_t Ctrl_Humidity = 0xF2;
const uint8_t Status = 0xF3;
const uint8_t Ctrl_Measure = 0xF4;
const uint8_t Config = 0xF5;
};

enum SensorMode : uint8_t
{
    Sleep = 0b00,
    Forced = 0b01,
    Normal = 0b11
};

enum OverSampling : uint8_t
{
    Skip = 0b000,
    X1 = 0b001,
    X2 = 0b010,
    X4 = 0b011,
    X8 = 0b100,
    X16 = 0b101
};

enum FilterCoefficients : uint8_t
{
    IIR_Off = 0b000,
    IIR_X2 = 0b001,
    IIR_X4 = 0b010,
    IIR_X8 = 0b011,
    IIR_X16 = 0b100
};

// standby durations in ms
enum StandbyDuration : uint8_t
{
    ms_0_5 = 0b000,  //0, 0.5ms
    ms_62_5 = 0b001, //1, 62.5ms
    ms_125 = 0b010,  //2, 125ms
    ms_250 = 0b011,  //3, 250ms
    ms_500 = 0b100,  //4, 500ms
    ms_1000 = 0b101, //5, 1000ms
    ms_10 = 0b110,   //6, 10ms
    ms_20 = 0b111    //7, 20ms
};

class IdReg : public Register
{
  public:
    IdReg(uint8_t deviceAddress) : Register(deviceAddress, ConfigRegisters::ChipId, 1) {}
};

class ResetReg : public Register
{
  public:
    ResetReg(uint8_t deviceAddress) : Register(deviceAddress, ConfigRegisters::Soft_Reset, 1) {}
};

class StatusReg : public Register
{
  public:
    StatusReg(uint8_t deviceAddress) : Register(deviceAddress, ConfigRegisters::Status, 1) {}
};

class CtrlHumidityReg : public Register
{
  public:
    CtrlHumidityReg(uint8_t deviceAddress) : Register(deviceAddress, ConfigRegisters::Ctrl_Humidity, 1) {}
    OverSampling HumidityOverSample = OverSampling::Skip;
    uint8_t get()
    {
        return HumidityOverSample & 0b111;
    }
    bool Load()
    {
        if (Register::Load())
        {
            HumidityOverSample = (OverSampling)(_value & 0b00000111);
            return true;
        }
        return false;
    }
};

class CtrlMeasureReg : public Register
{
  public:
    CtrlMeasureReg(uint8_t deviceAddress) : Register(deviceAddress, ConfigRegisters::Ctrl_Measure, 1) {}
    SensorMode RunMode = SensorMode::Sleep;
    OverSampling PressOverSample = OverSampling::Skip;
    OverSampling TempOverSample = OverSampling::Skip;
    uint8_t get()
    {
        return (TempOverSample << 5) | (PressOverSample << 2) | RunMode;
    }
    bool Load()
    {
        if (Register::Load())
        {
            TempOverSample = (OverSampling)((_value & 0b11100000) >> 5);
            PressOverSample = (OverSampling)((_value & 0b00011100) >> 2);
            RunMode = (SensorMode)(_value & 0b00000011);
            return true;
        }
        return false;
    }
};

class ConfigReg : public Register
{
  public:
    ConfigReg(uint8_t deviceAddress) : Register(deviceAddress, ConfigRegisters::Config, 1) {}
    StandbyDuration SleepTime = StandbyDuration::ms_0_5;
    FilterCoefficients IIR_Filter = FilterCoefficients::IIR_Off;
    uint8_t Communication = 0;
    uint8_t get()
    {
        return (SleepTime << 5) | (IIR_Filter << 2) | Communication;
    }
    bool Load()
    {
        if (Register::Load())
        {
            SleepTime = (StandbyDuration)((_value & 0b11100000) >> 5);
            IIR_Filter = (FilterCoefficients)((_value & 0b00011100) >> 2);
            Communication = (_value & 0b00000011);
            return true;
        }
        return false;
    }
};