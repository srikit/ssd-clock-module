//-----------------------------------------------------------------------------
// STM8S common utility functions.
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
// Last updated: Dilshan Jayakody [12th Oct 2020]
//
// Update log:
// [07/10/2020] - Initial version - Dilshan Jayakody.
// [12/10/2020] - Add inline delay option - Dilshan Jayakody.
//-----------------------------------------------------------------------------

#ifndef STM8S103_COMMON_UTIL_H
#define STM8S103_COMMON_UTIL_H

#include "../include/stm8.h"

#ifndef F_CPU
#warning "F_CPU not defined, using 2MHz as default CPU clock frequency"
#define F_CPU 2000000UL
#endif

static inline unsigned char bcdToDec(unsigned char bcd)
{
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

static inline unsigned char decToBCD(unsigned char dec)
{
    return ((dec / 10) << 4) + (dec % 10);
}

#ifdef INLINE_DELAY

static inline void delay_ms(unsigned int ms)
{
    unsigned int countdown;

    for (countdown = 0; countdown < ((F_CPU / 18 / 1000UL) * ms); countdown++)
    {
        nop();
    }
}

static inline void delay_cycle(unsigned short cycle)
{
    while(cycle)
    {
        cycle--;
    }
}

#else

void delay_ms(unsigned int ms);
void delay_cycle(unsigned short cycle);

#endif

#endif /* STM8S103_COMMON_UTIL_H */