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

#ifndef DS3231_RTC_H
#define DS3231_RTC_H

#define DS3231_I2C_READ_ADDR    0xD1
#define DS3231_I2C_WRITE_ADDR   0xD0

#define DS3231_TIME_ADDR        0x00
#define DS3231_CONTROL_ADDR     0x0E
#define DS3231_STATUS_ADDR      0x0F
#define DS3231_ALARM1_ADDR      0x07

typedef struct TimeData
{
    unsigned char hour;
    unsigned char minutes;
    unsigned char seconds;
} TimeData;

void initRTC();

void getRTCData(unsigned char addr, TimeData *buffer);
void setRTCData(unsigned char addr, TimeData *buffer);

void clearRTCAlarm();
void setAlarmStatus(unsigned char alarmState);

#endif /* DS3231_RTC_H */