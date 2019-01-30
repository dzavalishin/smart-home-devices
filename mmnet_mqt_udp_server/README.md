# Wall control board for smart home

Control/monitor four switches with buttons/leds.

To configure topic names for switches, see ```mqtt_map.c```:

```
struct mqtt_io  mqm[] =
{
    { "Light5A",        0 },
    { "Light5B",        1 },
    { "Light1A",        2 },
    { "Light1B",        3 },
};
```

String is a topic name, number is channel (button number).

## Hardware

MMNET01 Atmega128 board with ethernet.

## Pin map for MMNET01

### Port B

 *    0-3 - front panel LEDs
 *    4,6 - relays (not finished)
 *    5   - (used in mmnet?)
 *    7   - LCD backlight PWM

### Port D

 *    0-3 - Front panel buttons
 *    2,3 - 
 *    6-7 - Di (not finished)

### Port E

 *    0   - LCD E
 *    1   - LCD R/W
 *    2   - LCD R/S
 *    3   - 
 *    4   - Encoder button
 *    6,7 - Encoder rotation

### Port F

 *    0-7 - LCD data bus (0-3 in 4 bit mode as LCD DB4-7 )

### Port G

 *    3   - Activity LED
 *    4   - 1Wire (single, 2401 serial/MAC)

In current build LCD, encoder and 1Wire is not finished and not compiled in.

