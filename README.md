# ESP8266 libary to drive LED 64x16 matrix

This library provides an ESP8266 driver to run 64x16  LED matrix (red). It  supports ASCII characters only. It should work with one or multiple 64x16 panels. If you have red-green or 64x32 panels, the library code needs to be slightly modified. My LED matrxi has following pins: enable, clock, latch, data_r1, data r2, data_g1, data_g2, and 4 row pins labeled (a, b, c, d).  The "*_r2" pins are for controlling lower half of the 64x32 panels, and "g1" "g2" pins are for green panels. 

I directly connect data pins of the ESP8266 Mini D1 (3.3v) to the LED panel (5v).  

Latch pin ("LAT") must be connected to D0 (GPIO 16). GPIO 16 register is different from other GPIO.

To change the orientation of the display, modify the line in the library header file ESP8266_LED_64x16_Matrix.h "#define frameOrientation false"

![Circuit](https://raw.githubusercontent.com/qisun1/ESP8266_LED_64x16_Matrix/master/pictures/circuit_led64x16.png)



## Examples

In examples directory.

## Limitations
The current code does not yet implement pins for green color as well as data_r2 pin used for 64x32 matrix. Modification is needed. As most ESP8266 boards only have 11 usable data pins, to use the "g1" and "g2" pins, the "RX" "TX" pins on ESP8266 will need to be repurposed as GPIO pins.



## Compatible Hardware

The library works with ESP8266 based board, and one or multiple 64x16 LED panels.


## License

This code is released under the MIT License.
