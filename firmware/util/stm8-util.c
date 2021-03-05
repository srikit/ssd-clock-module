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

#include "../include/stm8-util.h"

#ifndef INLINE_DELAY

void delay_ms(unsigned int ms)
{
    unsigned int countdown;

    for (countdown = 0; countdown < ((F_CPU / 18 / 1000UL) * ms); countdown++)
    {
        nop();
    }
}

void delay_cycle(unsigned short cycle)
{
    while(cycle)
    {
        cycle--;
    }
}

#endif