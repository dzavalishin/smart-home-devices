# Software for AVR based smarthome hw #

## one\_wire\_temp ##

1-Wire to modbus protocol convertor, up to 32 temperature sensors (DS18B20 or alike)

  * automatic ModBus RTU/Ascii selector (DCOM to come later)
  * up to 8 separate hardware 1-wire buses
  * maps 1-wire ROM codes to persistent sensor ID
  * 1602 LCD, local configuration UI

additionally:

  * 4 AI

This device discovers and reads temperature from up to 32 Dallas 1-wire temperature sensors on up to 8 separate hardware buses. Temperature readouts can be accessed with ModBus protocol over an RS485 bus.

1-wire bus does not work well if bus topology is star. That's why it is good to have 8 separate buses here.