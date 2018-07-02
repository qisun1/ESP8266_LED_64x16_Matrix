# ESP8266 libary to drive LED 64x16 or 64x32 (RED/GREEN) matrix

This library provides an ESP8266 driver to run LED matrix, supporting ASCII encoding. I am still testing it. It should work with one or multiple 64x16 LED matrix (red). If you have red-green matrix, the code needs to be slightly modified if you plan to use the green color. My LED matrxi has following pins: enable, clock, latch, data_r1, data r2(for 64x32 matrix), data_g1, data_g2 (for green color), and 4 row pins labeled (a, b, c, d). 

## Examples

In examples directory. I am adding more display modes.

## Limitations
The current code does not work with green color, and 64x32 matrix yet. Slight modification is needed. 



## Compatible Hardware

The library works with ESP8266 based board:


## License

This code is released under the MIT License.
