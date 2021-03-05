//-----------------------------------------------------------------------------
// SSD clock module firmware for STM8S103F3P3.
// DS3231 based real time clock driver.
//
// Copyright (C) 2020 SriKIT contributors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// Last updated: Dilshan Jayakody [13th Oct 2020]
//
// Update log:
// [13/10/2020] - Initial version - Dilshan Jayakody.
//-----------------------------------------------------------------------------

#include "include/stm8.h"
#include "include/stm8-util.h"
#include "include/stm8-i2c.h"

#include "ds3231rtc.h"

void initRTC()
{
    i2cInit();
}

void getRTCData(unsigned char addr, TimeData *buffer)
{
    // Initialize read operation.
    i2cStart();
    i2cWriteAddr(DS3231_I2C_WRITE_ADDR);

    // Start reading from specified address.
    i2cWrite(addr);
    i2cStop();

    // Get time information from the I2C link.
    i2cStart();
    i2cWriteAddr(DS3231_I2C_READ_ADDR);

    buffer->seconds = bcdToDec(i2cRead(1));
    buffer->minutes = bcdToDec(i2cRead(1));
    buffer->hour = bcdToDec(i2cRead(0));
    i2cStop();
}

void setRTCData(unsigned char addr, TimeData *buffer)
{
    // Initialize write operation.
    i2cStart();
    i2cWriteAddr(DS3231_I2C_WRITE_ADDR);

    // Start reading from specified address.
    i2cWrite(addr);

    i2cWrite(decToBCD(buffer->seconds));
    i2cWrite(decToBCD(buffer->minutes));
    i2cWrite(decToBCD(buffer->hour));

    // Update A1M4 to trigget alarm on  hours, minutes, and seconds match.
    if(addr == DS3231_ALARM1_ADDR)
    {
        i2cWrite(0x81);
    }

    i2cStop();
}

void clearRTCAlarm()
{
    unsigned char statusData;
    
    // Get RTC alarm interrupt flags.
    i2cStart();
    i2cWriteAddr(DS3231_I2C_WRITE_ADDR);
    i2cWrite(DS3231_STATUS_ADDR);
    i2cStop();

    i2cStart();
    i2cWriteAddr(DS3231_I2C_READ_ADDR);
    statusData = i2cRead(0);
    i2cStop();

    // Clear alarm interrupt flags.
    statusData &= 0xFC;

    // Submit updated status data into RTC.
    i2cStart();
    i2cWriteAddr(DS3231_I2C_WRITE_ADDR);
    i2cWrite(DS3231_STATUS_ADDR);
    i2cWrite(statusData);
    i2cStop();
}

void setAlarmStatus(unsigned char alarmState)
{
    unsigned char controlData, statusData;
    
    // Get current RTC control settings and flags.
    i2cStart();
    i2cWriteAddr(DS3231_I2C_WRITE_ADDR);

    // Start reading from control address.
    i2cWrite(DS3231_CONTROL_ADDR);
    i2cStop();

    // Get data from the I2C link.
    i2cStart();
    i2cWriteAddr(DS3231_I2C_READ_ADDR);
    controlData = i2cRead(1);
    statusData = i2cRead(0);
    i2cStop();

    // Update specified alarm setting in control byte.
    controlData &= 0xFA;
    
    if(alarmState)
    {
        controlData |= 0x05;
    }
    else
    {
        // Shutdown alarm output.
        PD_ODR &= 0xBF;        
        controlData |= 0x04;        
    }
    
    // Clear all alarm interrupt flags.
    statusData &= 0xFC;

    // Submit updated control / status bytes into RTC.
    i2cStart();
    i2cWriteAddr(DS3231_I2C_WRITE_ADDR);
    i2cWrite(DS3231_CONTROL_ADDR);
    i2cWrite(controlData);
    i2cWrite(statusData);
    i2cStop();
}