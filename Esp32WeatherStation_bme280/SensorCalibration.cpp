//Copyright @ Adys Tech
//Author : mvadu@adystech.com

#include "SensorCalibration.h"

bool SensorCalibration::Load()
{
    dig_T1 = readRegisterInt16(TrimmingRegisters::T1);
    dig_T2 = readRegisterInt16(TrimmingRegisters::T2);
    dig_T3 = readRegisterInt16(TrimmingRegisters::T3);

    dig_P1 = readRegisterInt16(TrimmingRegisters::P1);
    dig_P2 = readRegisterInt16(TrimmingRegisters::P2);
    dig_P3 = readRegisterInt16(TrimmingRegisters::P3);
    dig_P4 = readRegisterInt16(TrimmingRegisters::P4);
    dig_P5 = readRegisterInt16(TrimmingRegisters::P5);
    dig_P6 = readRegisterInt16(TrimmingRegisters::P6);
    dig_P7 = readRegisterInt16(TrimmingRegisters::P7);
    dig_P8 = readRegisterInt16(TrimmingRegisters::P8);
    dig_P9 = readRegisterInt16(TrimmingRegisters::P9);

    dig_H1 = readRegister(TrimmingRegisters::H1);
    dig_H2 = readRegisterInt16(TrimmingRegisters::H2);
    dig_H3 = readRegister(TrimmingRegisters::H3);
    uint8_t shared = readRegister(TrimmingRegisters::H4_H5_Shared);
    dig_H4 = (readRegister(TrimmingRegisters::H4_MSB) << 4) + (shared & 0x0F);
    dig_H5 = (readRegister(TrimmingRegisters::H5_MSB) << 4) + ((shared >> 4) & 0x0F);
    dig_H6 = readRegister(TrimmingRegisters::H6);
}
