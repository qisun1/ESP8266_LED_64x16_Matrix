#include <ESP8266_LED_64x16_Matrix.h>
// Connections to board
#define latchPin D0
#define clockPin D1
#define data_R1 D3
#define data_R2 D2
#define en_74138 D4
#define la_74138 D8
#define lb_74138 D7
#define lc_74138 D6
#define ld_74138 D5 


ESP8266_LED_64x16_Matrix LEDMATRIX;

void setup()
{
	//for some reason, my board requires 4 seconds delay when starting. otherwise there is a reset after 4 seconds. I do not know why
	delay(4000);

	Serial.begin(115200);

	//start the display for a mode
	//screen mode is an integer code: 0:64x16 1:128x16;
	uint8_t t[8] = { latchPin, clockPin, data_R1, en_74138, la_74138, lb_74138, lc_74138, ld_74138};
	LEDMATRIX.setPins(t);
	
	LEDMATRIX.setDisplay(0, 1);


	//LEDMATRIX.drawChar(0, 0, 'A', 1);

	LEDMATRIX.turnOn();

	LEDMATRIX.message = "ABCDEFGHIJKLMN";
	//LEDMATRIX.turnOn();
}

void loop()
{
	//horizontal scroll
	LEDMATRIX.scrollTextHorizontal(100);

	//vertical scroll
	//LEDMATRIX.scrollTextVertical(100);

	//static change screen
	//LEDMATRIX.BreakTextInFrames(100);

}
