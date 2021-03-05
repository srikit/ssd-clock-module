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

#ifndef SERIAL_SEVEN_SEGMENT_DISPLAY_H
#define SERIAL_SEVEN_SEGMENT_DISPLAY_H

#define SSD_DOT_ON  0x80
#define SSD_DOT_OFF 0x00

void setDigitValue(unsigned char ssdValue);
unsigned char getDigitValue(unsigned char num);

#endif /* SERIAL_SEVEN_SEGMENT_DISPLAY_H */