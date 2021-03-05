//-----------------------------------------------------------------------------
// SSD clock module firmware for STM8S103F3P3.
// Main source file.
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
// Last updated: Dilshan Jayakody [14th Oct 2020]
//
// Update log:
// [11/10/2020] - Initial version - Dilshan Jayakody.
// [14/10/2020] - Fix LED blinks at startup - Dilshan Jayakody.
//-----------------------------------------------------------------------------

#ifndef DIGITAL_CLOCK_MODULE_H
#define DIGITAL_CLOCK_MODULE_H

#define LONG_BUTTON_PRESS_LIMIT 25
#define AUTO_COUNT_PRESS_LIMIT  10
#define BLINK_RATE  3

#define MENU_TIMEOUT_LIMIT      25

#define EDIT_MENU_REFRESH_RATE  10
#define MAIN_LOOP_REFRESH_RATE  4

#define ALARM_AUTO_RESET_LIMIT  0x5C0

#define SUCCESS 0xFF
#define FAIL    0x00

typedef enum
{
    TIME_NORMAL,
    TIME_SECONDS,
    TIME_EDIT_HOUR_MIN,
    TIME_EDIT_SECONDS
} TimeMode;

typedef enum
{
    COMP_HOUR,
    COMP_MINUTE,
    COMP_SECOND,
    COMP_NULL
} TimeComponent;

typedef enum
{
    BTN_NONE,
    BTN_UP,
    BTN_DOWN,    
} LongPressButtonMode;

typedef enum
{
    MENU_DEACTIVE,
    MENU_TIME_MODE,
    MENU_ALARM_MODE
} EditMode;

unsigned char alarmActive;
unsigned short alarmCounter;

void initSystem();
void initDisplayTimer();
void shutdownDisplay();

unsigned char timeEditMode();

void enableTimeoutCounter();
void disableTimeoutCounter();

extern inline void muteAlarm()
{
    // Shutdown external alarm output.
    PD_ODR &= 0xBF; 

    // Clear self-reset counter.
    alarmActive = FAIL;
    alarmCounter = 0;

    // Clear RTC alarm interrupt.
    clearRTCAlarm();
}

#endif