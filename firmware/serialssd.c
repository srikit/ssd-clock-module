//-----------------------------------------------------------------------------
// SSD clock module firmware for STM8S103F3P3.
// 74HC595 based seven segment display driver.
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
// Last updated: Dilshan Jayakody [11th Oct 2020]
//
// Update log:
// [11/10/2020] - Initial version - Dilshan Jayakody.
//-----------------------------------------------------------------------------

#include "include/stm8.h"
#include "include/stm8-util.h"

#include "serialssd.h"

// PORTC.3 -> Serial data.
// PORTC.4 -> Latch data.
// PORTC.5 -> Serial clock.

void setDigitValue(unsigned char ssdValue)
{
    unsigned char pos = 7;

    while(pos != 0xFF)
    {
        // Set next available bit into data terminal and clear SRCLK, CLK.
        PC_ODR &= 0xC7;        
        PC_ODR |= (ssdValue & (1 << pos)) ? 0x08 : 0x00;
        delay_cycle(10);

        // Load data into SSD shift-register.
        PC_ODR |= 0x20;
        delay_cycle(20);

        pos--;
    }

    // Load ssdValue into output register.
    PC_ODR &= 0xC7;
    PC_ODR |= 0x10;    
}

unsigned char getDigitValue(unsigned char num)
{
    switch (num)
    {
    case 0:
        return 0x3F;    // Character : 0
    case 1:
        return 0x06;    // Character : 1
    case 2:
        return 0x5B;    // Character : 2
    case 3:
        return 0x4F;    // Character : 3
    case 4:
        return 0x66;    // Character : 4
    case 5:
        return 0x6D;    // Character : 5
    case 6:
        return 0x7D;    // Character : 6
    case 7:
        return 0x07;    // Character : 7
    case 8:
        return 0x7F;    // Character : 8
    case 9:
        return 0x6F;    // Character : 9
    default:
        return 0x00;    // Clear segment.
    }
}