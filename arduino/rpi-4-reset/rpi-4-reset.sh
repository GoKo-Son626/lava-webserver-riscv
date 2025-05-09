#!/bin/bash

/lava-shared/arduino-cli config init
/lava-shared/arduino-cli core update-index
/lava-shared/arduino-cli core install arduino:avr
/lava-shared/arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old /lava-shared/rpi-4-reset/rpi-4-reset.ino
/lava-shared/arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano:cpu=atmega328old /lava-shared/rpi-4-reset/rpi-4-reset.ino
