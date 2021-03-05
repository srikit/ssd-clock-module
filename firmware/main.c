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
// [12/10/2020] - Add RTC alarm support - Dilshan Jayakody.
// [14/10/2020] - Fix LED blinks at startup - Dilshan Jayakody.
//-----------------------------------------------------------------------------

#include "include/stm8.h"
#include "include/stm8-util.h"

#include "serialssd.h"
#include "ds3231rtc.h"

#include "main.h"

TimeData sysTime;
TimeMode mode;
TimeComponent timeComponent;
EditMode functionMode;

unsigned char digit1, digit2;
unsigned char refreshCounter;
unsigned char blinkCounter, blinkState;
unsigned char lastInput;
unsigned char idleCount;
unsigned char idleTime;
unsigned char idleUpdate;
unsigned char longPressCounter;
unsigned char alarmState;

void PORTC_update() __interrupt(PORTC_IRQ)
{
    // RTC alarm interrupt handler.
    if((alarmState) && ((PC_IDR & 0x40) == 0x00))
    {
        // Alarm activated.
        PD_ODR |= 0x40;

        // Start alarm self-reset counter.
        alarmCounter = ALARM_AUTO_RESET_LIMIT;
        alarmActive = SUCCESS;        
    }
}

void TIM2_update() __interrupt(TIMER2_TRIGGER_IRQ)
{       
    // Check for edit mode is in seconds component.
    if((mode == TIME_SECONDS) || ((mode == TIME_EDIT_HOUR_MIN) && (timeComponent == COMP_SECOND)))
    {
        // Shutdown first two segments in seconds edit mode.
        PD_ODR &= 0xE1;
        setDigitValue(0x00);
        PD_ODR |= 0x02;
        delay_cycle(100);

        PD_ODR &= 0xE1;
        setDigitValue(0x00);
        PD_ODR |= 0x04;
        delay_cycle(100);

        // Get current seconds value and blink rate.
        if((blinkState > 0) || (mode == TIME_SECONDS))
        {
            digit1 = sysTime.seconds / 10;
            digit2 = sysTime.seconds % 10;
        }
        else
        {
            digit1 = 0xFF;
            digit2 = 0xFF;
        }
           
        // Display seconds value.
        PD_ODR &= 0xE1;
        setDigitValue(getDigitValue(digit1));
        PD_ODR |= 0x08;
        delay_cycle(100);

        PD_ODR &= 0xE1;
        setDigitValue(getDigitValue(digit2));
        PD_ODR |= 0x10;
        delay_cycle(100);

        // Shutdown all segments.
        PD_ODR &= 0xE1;

        // Clear timer 2 interrupt flag.
        TIM2_SR1 &= ~TIM2_SR1_UIF;

        // Exit from ISR.
        return;
    }
    
    digit1 = sysTime.hour / 10;
    digit2 = sysTime.hour % 10;

    // If first digit is 0, then turn it off.
    if(digit1 == 0)
    {
        digit1 = 0xFF;
    }
    
    // Blink hour component in edit mode.
    if((mode == TIME_EDIT_HOUR_MIN) && (timeComponent == COMP_HOUR) && (blinkState > 0))
    {
        digit1 = 0xFF;
        digit2 = 0xFF;
    }

    // Set value of the first hour digit.    
    PD_ODR &= 0xE1;
    setDigitValue(getDigitValue(digit1));
    PD_ODR |= 0x02;
    delay_cycle(90);

    // Set value of the second hour digit.
    PD_ODR &= 0xE1;
    setDigitValue(getDigitValue(digit2) | ((sysTime.seconds % 2 == 0) ? SSD_DOT_ON : SSD_DOT_OFF));
    PD_ODR |= 0x04;
    delay_cycle(85);

    digit1 = sysTime.minutes / 10;
    digit2 = sysTime.minutes % 10;

    // Blink minute component in edit mode.
    if((mode == TIME_EDIT_HOUR_MIN) && (timeComponent == COMP_MINUTE) && (blinkState > 0))
    {
        digit1 = 0xFF;
        digit2 = 0xFF;
    }

    // Set value of the first minute digit.
    PD_ODR &= 0xE1;
    setDigitValue(getDigitValue(digit1));
    PD_ODR |= 0x08;
    delay_cycle(100);

    // Set value of the second minute digit.
    PD_ODR &= 0xE1;
    setDigitValue(getDigitValue(digit2));
    PD_ODR |= 0x10;
    delay_cycle(100);

    // Shutdown all segments.
    PD_ODR &= 0xE1;

    // Clear timer 2 interrupt flag.
    TIM2_SR1 &= ~TIM2_SR1_UIF;
}

void TIM4_update() __interrupt(TIMER4_TRIGGET_IRQ)
{
    // Idle time counter.
    if((++idleCount) > 50)
    {
        idleCount = 0;
        idleTime++;
    }

    // Enable timer interrupt.
    TIM4_SR &= ~TIM4_SR1_UIF;
    TIM4_CNTR = 0x00;
}

void main()
{           
    cli();
        
    // Initialize all the required registers of the MCU.
    initSystem();
    delay_ms(5);

    // Initialize I2C channel to link with RTC.
    initRTC();
    delay_ms(150);

    // Initialize system time structure by getting data from RTC.
    getRTCData(DS3231_TIME_ADDR, &sysTime);
    
    // Start display driver.
    initDisplayTimer();

    sei();

    // Main service loop.
    while(1)
    {
        // refresh time with RTC.
        getRTCData(DS3231_TIME_ADDR, &sysTime);

        // Detect long button press.
        if((PA_IDR & 0x0E) != 0x0E)
        {
            longPressCounter++;

            if(longPressCounter >= LONG_BUTTON_PRESS_LIMIT) 
            {
                // Long button press timeout is reached!
                if((PA_IDR & 0x02) == 0x00)
                {
                    functionMode = MENU_TIME_MODE;

                    // Activate time edit mode.                    
                    if(timeEditMode())
                    {
                        // Send updated time information to RTC.
                        setRTCData(DS3231_TIME_ADDR, &sysTime); 

                        // Restore time on SSD.
                        initDisplayTimer();
                    }

                    functionMode = MENU_DEACTIVE;
                }
                else if((PA_IDR & 0x04) == 0x00)
                {
                    // Activate alarm edit mode.
                    getRTCData(DS3231_ALARM1_ADDR, &sysTime);
                    
                    functionMode = MENU_ALARM_MODE;
                    
                    // Activate time edit mode.
                    if(timeEditMode())
                    {
                        // Send alarm 1 configuration to the RTC.
                        setRTCData(DS3231_ALARM1_ADDR, &sysTime);

                        // Get system time from RTC and display.
                        getRTCData(DS3231_TIME_ADDR, &sysTime);
                        initDisplayTimer();
                    }

                    functionMode = MENU_DEACTIVE;
                    PD_ODR = alarmState ? (PD_ODR | 0x20) : (PD_ODR & 0xDF);
                }

                longPressCounter = 0;
                lastInput = PA_IDR;
            }          
        }
        else
        {
            // Clear long press flag.
            longPressCounter = 0;
        }

        if(((lastInput & 0x02) == 0x00) && ((PA_IDR & 0x02) == 0x02))
        {
            // Toggle between hour/minute and seconds mode.
            mode = (mode == TIME_SECONDS) ? TIME_NORMAL : TIME_SECONDS;
        }
        
        if(((lastInput & 0x04) == 0x00) && ((PA_IDR & 0x04) == 0x04))
        {
            // Activate or deactivate the alarm.
            alarmState = ~alarmState;            
            PD_ODR = alarmState ? (PD_ODR | 0x20) : (PD_ODR & 0xDF);
            setAlarmStatus(alarmState);

            // Clear self-reset counter.
            if(!alarmState)
            {
                alarmActive = FAIL;
                alarmCounter = 0;
            }
        }

        if(((lastInput & 0x08) == 0x00) && ((PA_IDR & 0x08) == 0x08))
        {
            // Silient the alarm output.
            muteAlarm();
        }

        // Check for alarm self-reset timeout.
        if(alarmActive)
        {
            if((--alarmCounter) < 1)
            {
                // Mute current alarm!
                muteAlarm();
            }
        }

        lastInput = PA_IDR;

        // Idle counter.
        refreshCounter = MAIN_LOOP_REFRESH_RATE;
        while(refreshCounter > 0)
        {
            wfi();
            refreshCounter--;            
        }
    }
}

unsigned char timeEditMode()
{
    unsigned char menuState = 1;
    unsigned char longPressMode = 0;

    LongPressButtonMode longPressBtn = BTN_NONE;    
    mode = TIME_EDIT_HOUR_MIN;
    timeComponent = COMP_HOUR;

    blinkCounter = 0;
    blinkState = 0;
    longPressCounter = 0;
    idleUpdate = 0;

    // Always activate hour and minute seperator in the edit mode.
    sysTime.seconds = 0;
    
    // Wait until user releases the buttons.
    while((PA_IDR & 0x0E) != 0x0E)
    {
        blinkCounter++;

        if(blinkCounter > BLINK_RATE)
        {
            blinkCounter = 0;
            if((++blinkState) > 1)
            {
                blinkState = 0;                
            }
        }

        // Idle counter.
        refreshCounter = EDIT_MENU_REFRESH_RATE;
        while(refreshCounter > 0)
        {
            wfi();
            refreshCounter--;
        }
    }

    lastInput = 0x0E;
    enableTimeoutCounter();

    // Main edit mode service loop.
    while(menuState)
    {
        if(idleTime > MENU_TIMEOUT_LIMIT)
        {
            // Menu timeout occured, return to clock mode.
            getRTCData(DS3231_TIME_ADDR, &sysTime);

            mode = TIME_NORMAL;
            disableTimeoutCounter();
            return FAIL;
        }
        
        if((PA_IDR & 0x0E) != 0x0E)
        {
            // Try to detect long button press.            
            longPressCounter++;
            if(longPressCounter >= AUTO_COUNT_PRESS_LIMIT)
            {
                // Long button press detected.
                if(((PA_IDR & 0x02) == 0x00) && (idleUpdate))
                {
                    // Raise long button press flag for UP.
                    longPressMode = 1;
                    idleUpdate = 0;
                    longPressBtn = BTN_UP;
                }
                else if(((PA_IDR & 0x04) == 0x00) && (idleUpdate))
                {
                    // Raise long button press flag for DOWN.
                    longPressMode = 1;
                    idleUpdate = 0;
                    longPressBtn = BTN_DOWN;
                }

                longPressCounter = AUTO_COUNT_PRESS_LIMIT;
            }
        }
        else if(longPressMode)
        {
            // Exit long press mode and try to avoid button press events.
            lastInput = PA_IDR;
            longPressMode = 0;
            idleUpdate = 0;
            longPressCounter = 0;
            longPressBtn = BTN_NONE;
        }
        
        if((((lastInput & 0x02) == 0x00) && ((PA_IDR & 0x02) == 0x02)) || (longPressBtn == BTN_UP))
        {                        
            // Increase the current time component.
            idleTime = 0; 
            longPressBtn = BTN_NONE;

            switch (timeComponent)
            {
            case COMP_HOUR:
                sysTime.hour = (sysTime.hour == 23) ? 0 : (sysTime.hour + 1);
                break;
            case COMP_MINUTE:
                sysTime.minutes = (sysTime.minutes == 59) ? 0 : (sysTime.minutes + 1);
                break;
            case COMP_SECOND:
                sysTime.seconds = (sysTime.seconds == 59) ? 0 : (sysTime.seconds + 1);
                break;
            }                       
        }

        if((((lastInput & 0x04) == 0x00) && ((PA_IDR & 0x04) == 0x04)) || (longPressBtn == BTN_DOWN))
        {
            // Decrease the current time component.
            idleTime = 0; 
            longPressBtn = BTN_NONE;

            switch (timeComponent)
            {
            case COMP_HOUR:
                sysTime.hour = (sysTime.hour == 0) ? 23 : (sysTime.hour - 1);
                break;
            case COMP_MINUTE:
                sysTime.minutes = (sysTime.minutes == 0) ? 59 : (sysTime.minutes - 1);
                break;
            case COMP_SECOND:
                sysTime.seconds = (sysTime.seconds == 0) ? 59 : (sysTime.seconds - 1);
                break;
            }                      
        }

        if(((lastInput & 0x08) == 0x00) && ((PA_IDR & 0x08) == 0x08))
        {
            // Move to next available mode.
            idleTime = 0; 

            switch (timeComponent)
            {
            case COMP_HOUR:
                // Switch to minute edit mode.
                timeComponent = COMP_MINUTE;
                break;
            case COMP_MINUTE:
                // Switch to seconds edit mode.
                timeComponent = COMP_SECOND;
                break;
            case COMP_SECOND:
                // Initialize exit routine from the menu.
                idleTime = 0; 
                shutdownDisplay();                
                timeComponent = COMP_NULL;                
                menuState = 0;
                break;
            }
        }
        
        // Detect edit mode and current blink states.
        blinkCounter++;

        if(blinkCounter > BLINK_RATE)
        {
            blinkCounter = 0;
            if((++blinkState) > 1)
            {
                blinkState = 0;
                idleUpdate = 1;
            }
        }

        // Check for alarm self-reset timeout.
        if(alarmActive)
        {
            if((--alarmCounter) < 1)
            {
                // Mute current alarm!
                muteAlarm();
            }
        }

        // In alarm edit mode, blink the alarm LED.
        if(functionMode == MENU_ALARM_MODE)
        {
            PD_ODR = (blinkState > 0) ? (PD_ODR | 0x20) : (PD_ODR & 0xDF);
        }

        lastInput = PA_IDR;
        
        // Idle counter.
        refreshCounter = EDIT_MENU_REFRESH_RATE;
        while(refreshCounter > 0)
        {
            wfi();
            refreshCounter--;
        }        
    }

    disableTimeoutCounter();
    
    // Edit operation is successful and restore clock mode to normal.
    mode = TIME_NORMAL;   
    return SUCCESS;
}

void initSystem()
{
    PC_ODR = 0x00;
    PD_ODR = 0x00;
    
    // Init port C for following devices / interfaces:
    // PC3 : SSD serial data output. [SER]
    // PC4 : SSD serial load latch. [RCLK]
    // PC5 : SSD serial clock. [SRCLK]
    // PC6 : DS3231 interrupt input.
    PC_DDR = 0x38;
    PC_CR1 = 0x78;
    PC_CR2 = 0x40;     

    // Init port D for following devices / interfaces:
    // PD1 : Segment driver 1.
    // PD2 : Segment driver 2.
    // PD3 : Segment driver 3.
    // PD4 : Segment driver 4.
    // PD5 : Alarm LED.
    // PD6 : Alarm output.
    PD_DDR = 0x7E;
    PD_CR1 = 0x7E;
    PD_CR2 = 0x00;
    PD_ODR = 0x00;

    // Init port A for following devices / interfaces:
    // PA1 : SET / UP input
    // PA2 : SECONDS / DOWN input
    // PA3 : MUTE / NEXT input
    PA_DDR = 0x00;
    PA_CR1 = 0x0E;
    PA_CR2 = 0x00;

    // Enable port C interrupts on falling edge.
    EXTI_CR1 = 0x20;

    // Set global variables into known values.
    sysTime.hour = 12;
    sysTime.minutes = 0;
    sysTime.seconds = 0;

    blinkCounter = 0;
    blinkState = 0;
    refreshCounter = 0;
    lastInput = 0x0E;
    longPressCounter = 0;
    alarmState = 0;
    alarmCounter = 0;

    alarmActive = FAIL;
    mode = TIME_NORMAL;
    functionMode = MENU_DEACTIVE;
}

void initDisplayTimer()
{
    // Set timer 2 prescaler to 256.
    TIM2_PSCR = 8;

    // Set ARRH to get ms interrupt.
    TIM2_ARRH = 0x00;
    TIM2_ARRL = 0x27;

    // Enable timer 2 interrupt.
    TIM2_IER |= TIM2_IER_UIE;
    TIM2_CR1 |= TIM2_CR1_CEN;
}

void shutdownDisplay()
{
    TIM2_IER &= ~(TIM2_IER_UIE);
    TIM2_CR1 &= ~(TIM2_CR1_CEN);
}

void enableTimeoutCounter()
{
    idleCount = 0;
    idleTime = 0;
    
    TIM4_PSCR = 5;

    // Enable timer and related interrupts.
    TIM4_IER |= TIM4_IER_UIE;
    TIM4_SR &= ~TIM4_SR1_UIF;
    TIM4_CR1 |= TIM4_CR1_CEN;
}

void disableTimeoutCounter()
{
    idleCount = 0;
    idleTime = 0;
    
    // Disable timer and all related interrupts.
    TIM4_CR1 &= ~(TIM4_CR1_CEN);
    TIM4_SR &= ~TIM4_SR1_UIF;
    TIM4_IER &= ~TIM4_IER_UIE;
}