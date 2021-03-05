# Clock module with seven segment display

This simple LED clock module uses a highly stable *DS3231* real-time clock for timekeeping. The RTC and alarm are control by the *STM8S103* microcontroller. This module also provides a slot to install a small *CR1220* type battery to back up the time and alarm configuration. 

The module can power up using 5V or 3V DC power source. The necessary power source can select using a jumper in the module.

This clock module is designed to handle a generally available 14.2mm (0.56 inch) 4-digit seven-segment displays. This module use 12-pin (6-pin × 2) socket to connect the seven-segment display to the PCB, and it allows to mount the display unit off from the PCB.

The "alarm terminal" of this module is available as an open-collector output, and it allows the user to connect an external circuitry to this module.
