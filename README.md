# Clock module with seven segment display
![KT0001](https://img.shields.io/badge/ID-KT0001-orange)

![Front view of the clock module](https://raw.githubusercontent.com/srikit/ssd-clock-module/main/resources/images/ssd-clock-module-front-small.jpg)

This simple LED clock module uses a highly stable *DS3231* real-time clock for timekeeping. The RTC and alarm are control by the *STM8S103F3* microcontroller. This module also provides a slot to install a small *CR1220* type battery to back up the time and alarm configuration. 

The module can power up using 5V or 3V DC power source. The necessary power source can select using a jumper in the module.

This clock module is designed to handle a generally available 14.2mm (0.56 inch) 4-digit seven-segment displays. This module use 12-pin (6-pin × 2) socket to connect the seven-segment display to the PCB, and it allows to mount the display unit off from the PCB.

The "alarm terminal" of this module is available as an open-collector output, and it allows the user to connect an external circuitry to this module.

This is a [certified](https://certification.oshwa.org/lk000008.html) open-source hardware project. All the design files, [documentation](https://github.com/srikit/ssd-clock-module/wiki), and firmware source code are available to download at the [project source repository](https://github.com/srikit/ssd-clock-module). Content of this project is distributed under the terms of the following license:

* Hardware License: [CERN-OHL-W](https://opensource.org/CERN-OHL-W)
* Software License: [GNU GPL 3.0](https://github.com/srikit/ssd-clock-module/blob/main/LICENSE)
* Documentation License: [CC BY 4.0](https://creativecommons.org/licenses/by/4.0)
