/*
  ESP8266_LED_64x16_Matrix.cpp - A 64x16 matrix driver for MQTT.
  Qi Sun
  https://github.com
*/

#include "ESP8266_LED_64x16_Matrix.h"
#include "Arduino.h"


ESP8266_LED_64x16_Matrix::ESP8266_LED_64x16_Matrix()
{
}

//screen mode is an integer code: 0:64x16 1:128x16;
void ESP8266_LED_64x16_Matrix::setDisplay(uint8_t matrixType, uint8_t panels)
{
	ESP8266_LED_64x16_Matrix::isrInstance = this;
	switch (matrixType)
	{
		case 0:
			columnNumber = 8 * panels;
			rowCount = 16;
			bufferSize = (columnNumber+1)*rowCount;
		break;
		default:
			columnNumber = 8 * panels;
			rowCount = 16;
			bufferSize = (columnNumber + 1)*rowCount;
			break;
	}

	buffer = new uint8_t[bufferSize*2];

	scrollPointer = 0;
	scanRow = 0;
	clear_buffer();

	timer1_attachInterrupt(interruptHandler);


	timer1_write(nextT);
	timer1_disable();


	delay(100);
}

void ESP8266_LED_64x16_Matrix::setPins(uint8_t pins[8])
{
	latchPin = pins[0];
	clockPin = pins[1];
	data_R1 = pins[2];
	//data_R2 = ;
	en_74138 = pins[3];
	la_74138 = pins[4];
	lb_74138 = pins[5];
	lc_74138 = pins[6];
	ld_74138 = pins[7];

	rowPin = (1 << la_74138) | (1 << lb_74138) | (1 << lc_74138) | (1 << ld_74138);

	pinMode(latchPin, OUTPUT);  pinMode(clockPin, OUTPUT);
	pinMode(data_R1, OUTPUT);   //pinMode(data_R2, OUTPUT);

	pinMode(en_74138, OUTPUT);
	pinMode(la_74138, OUTPUT);  pinMode(lb_74138, OUTPUT);
	pinMode(lc_74138, OUTPUT);  pinMode(ld_74138, OUTPUT);

}

void ESP8266_LED_64x16_Matrix::turnOn()
{
	//timer1_isr_init();
	timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
}

void ESP8266_LED_64x16_Matrix::turnOff()
{
	timer1_disable();
	digitalWrite(en_74138, HIGH);

	clear_buffer();
	scanRow = 0;
	scrollPointer = 0;

}


void ESP8266_LED_64x16_Matrix::clear_buffer()
{
	for (uint16_t i = 0; i <bufferSize; i++)
	{
		buffer[i] = 0x00;
	}
}


void ESP8266_LED_64x16_Matrix::drawChar(uint16_t pixel_x, uint16_t pixel_y, uint8_t n) {
	uint8_t fontrows= 16;
	uint16_t index;
	uint8_t charbytes[fontrows];
	index = (n-32)*fontrows; // go to the right code for this character																						 // addressing start at buffer and add y (rows) * (WIDTH is 64 so WIDTH/8) is 8 plus (x / 8) is 0 to 7
	for (uint8_t i = 0; i<fontrows; i++) {  // fill up the charbytes array with the right bits
		charbytes[i] = font8x16_basic[index + i];
	};
																				 // addressing start at buffer and add y (rows) * (WIDTH is 64 so WIDTH/8) is 8 plus (x / 8) is 0 to 7
	uint8_t *pDst = buffer + (pixel_y * (columnNumber + 1)) + pixel_x;


	uint8_t *pSrc = charbytes; // point at the first set of 8 pixels    
	for (uint8_t i = 0; i<fontrows; i++) {
		*pDst = *pSrc;     // populate the destination byte
		pDst += columnNumber + 1;         // go to next row on buffer
		pSrc++;            // go to next set of 8 pixels in character
	}

	//Serial.println(buffer[0]);
	//Serial.println("draw buffer111");
};


void ESP8266_LED_64x16_Matrix::moveLeft(uint8_t pixels, uint8_t rowstart, uint8_t rowstop) { // routine to move certain rows on the screen "pixels" pixels to the left
	uint8_t row, column;
	uint16_t index;
	for (column = 0; column<(columnNumber + 1); column++) {
		for (row = rowstart; row<rowstop; row++) {
			index = (row * (columnNumber + 1)) + column; /// right here!
			if (column == (columnNumber))
				buffer[index] = buffer[index] << pixels; // shuffle pixels left on last column and fill with a blank
			else {                // shuffle pixels left and add leftmost pixels from next column
				uint8_t incomingchar = buffer[index + 1];
				buffer[index] = buffer[index] << pixels;
				for (uint8_t x = 0; x<pixels; x++) { buffer[index] += ((incomingchar & (0x80 >> x)) >> (7 - x)) << (pixels - x - 1); };
			}
		}
	}
};

void ESP8266_LED_64x16_Matrix::shiftOutFast(byte data)
{
	byte i = 8;
	do {
		GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1 << clockPin);
		if (data & 0x80)
			GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 1 << data_R1);
		else
			GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1 << data_R1);
		GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 1 << clockPin);
		data <<= 1;
	} while (--i);
	return;
}

void ESP8266_LED_64x16_Matrix::scrollTextHorizontal(uint16_t delaytime)
{
	// display next character of message
	drawChar(columnNumber, 0, message[scrollPointer % (message.length())]);
	scrollPointer++;
	if (scrollPointer >= message.length())
	{
		scrollPointer = 0;
	}
	// move the text 1 pixel at a time
	for (uint8_t i = 0; i<8; i++) {
		delay(delaytime);
		moveLeft(1, 0, rowCount);

	};

}

void ESP8266_LED_64x16_Matrix::BreakTextInFrames(uint16_t delaytime)
{
	clear_buffer();
	for (uint8_t i = 0; i < columnNumber; i++)
	{
		drawChar(i, 0, message[scrollPointer]);
		scrollPointer++;
		if (scrollPointer >= message.length())
		{
			scrollPointer = 0;
			break;
		}
	}
	delay(delaytime);
}


//not tested , need to double the buffer size in the setScreen
void ESP8266_LED_64x16_Matrix::scrollTextVertical(uint16_t delaytime)
{
	for (uint16_t i = bufferSize; i <(2* bufferSize); i++)
	{
		buffer[i] = 0x00;
	}
	for (uint8_t i = 0; i < columnNumber; i++)
	{
		drawChar(i, rowCount, message[scrollPointer]);
		scrollPointer++;
		if (scrollPointer >= message.length())
		{
			scrollPointer = 0;
			break;
		}
	}

	for (uint8_t t = 0; t < rowCount; t++)
	{
		for (uint8_t i = 0; i< (rowCount * 2 - 1); i++)
		{
			memcpy(buffer + i * (columnNumber + 1), buffer + (i + 1)*(columnNumber + 1), columnNumber + 1);
		}
		delay(50);
	}
	delay(delaytime);

}



void  ESP8266_LED_64x16_Matrix::ISR_TIMER_SCAN()
{
	//noInterrupts();
	GPOS |= 1 << en_74138;
	//digitalWrite(en_74138, HIGH);     // Turn off display
									  // Shift out 8 columns

	WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR + 8, rowPin);
	uint32_t rowPinSet = ((scanRow >> 3) & 0x01) << ld_74138;
	rowPinSet = rowPinSet | (((scanRow >> 2) & 0x01) << lc_74138);
	rowPinSet = rowPinSet | (((scanRow >> 1) & 0x01) << lb_74138);
	rowPinSet = rowPinSet | ((scanRow & 0x01) << la_74138);
	WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR + 4, rowPinSet);


	for (uint8_t column = 0; column < columnNumber; column++) {
		uint8_t index = column + (scanRow *(columnNumber + 1));
		//shiftOut(data_R1, clockPin, MSBFIRST, buffer[index]);
		shiftOutFast(buffer[index]);
	}

	//digitalWrite(latchPin, LOW);
	GP16O &= ~1;
	//digitalWrite(latchPin, HIGH);
	GP16O |= 1;


	GPOC |= 1 << en_74138;
	//digitalWrite(en_74138, LOW);     // Turn on display
	scanRow++; 
	// Do the next pair of rows next time this routine is called
	if (scanRow == rowCount)
	{
		scanRow = 0;
		
	}
	timer1_write(nextT);
	//interrupts();
}

ESP8266_LED_64x16_Matrix * ESP8266_LED_64x16_Matrix::isrInstance;

void ESP8266_LED_64x16_Matrix::interruptHandler()
{
	isrInstance->ISR_TIMER_SCAN();
}
