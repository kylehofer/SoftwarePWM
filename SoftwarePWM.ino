/*
 * Copyright (c) 2018 Kyle Hofer
 * Email: kylehofer@neurak.com.au
 * Web: https://neurak.com.au/
 *
 * This file is part of SoftwarePWM.
 *
 * SoftwarePWM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SoftwarePWM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This sketch contains a set of demo functions for testing the effectiveness of the SoftwarePWM library
 * My demo implementation uses a set of 6 LED's wired with BC549 Transistors as outputs for the PWM
 */

#include "SoftwarePWM.h"

void setup() {
	SoftwarePWM_init();
}

/*
 * Flashes the LED's on and off
 */
void flash() {
	SoftwarePWM_setAll(255);

	delay(125);

	SoftwarePWM_setAll(0);

	delay(75);

	SoftwarePWM_setAll(255);

	delay(125);

	SoftwarePWM_setAll(0);


	delay(75);

	SoftwarePWM_setAll(255);

	delay(45);

	SoftwarePWM_setAll(0);

	delay(75);
}

/*
 * Lights up the LED's in sequence
 */
void inOrderLightUp() {
	for (int i = 0; i < 6; i++){
		for (int b = 1; b < 50; b++) {
			SoftwarePWM_setPWM(i, b * 5);
		}
	}
}

/*
 * Dims the LED's in sequence
 */
void inOrderLightUpReverse() {
	for (int i = 5; i >= 0; i--){
		for (int b = 49; b >= 0; b--) {
			SoftwarePWM_setPWM(i, b * 5);
		}
	}
}

/*
 * A moving light travels through the LED's
 */
void moving() {
	uint8_t index = 0;
	int value = 255;

	for (int i = 0; i < 42; i++) {
		SoftwarePWM_setPWM(index, value);

		delay(55);
		SoftwarePWM_setPWM(index, 0);
		index++;
		if (index >= PWM_MAX)
			index = 0;
	}
	
}

/*
 * A moving light with a trail travels through the LED's
 */
void tail() {

	int tailLength = 4;
	int tailBrightness = 255;
	int tailDuration = 42;

	uint8_t tail[tailLength];
	uint8_t tailValues[tailLength];

	tailValues[0] = tailBrightness;

	tail[0] = 0;
	tail[1] = 5;
	tail[2] = 4;
	tail[3] = 3;

	tailValues[0] = 255;
	tailValues[1] = 60;
	tailValues[2] = 15;
	tailValues[3] = 0;

	for (int i = 0; i < tailDuration; i++) {

		SoftwarePWM_setPWMs(tail, tailValues, tailLength);

		for (int t = 0; t < tailLength; t++)	{
			tail[t]++;
			if (tail[t] >= PWM_MAX)
				tail[t] = 0;
		}
		delay(125);
	}

}

/*
 * A speed test to check the processor can keep up with the speed of the PWM.
 * The last LED of the series is lit on the dimmest setting, then LED's 0 through 5
 * are lit up in sequence as fast as possible. LED 5 can then be monitored for flickering.
 */
void speedTest() {
	SoftwarePWM_setPWM(5, 1);


	for (int i = 0; i < 5; i++){
		for (int b = 0; b < 127; b++) {
			SoftwarePWM_setPWM(i, b << 1);
		}
	}
	for (int i = 4; i >= 0; i--){
		for (int b = 127; b >= 0; b--) {
			SoftwarePWM_setPWM(i, b << 1);
		}
	}	
}

void loop() {
	speedTest();
	SoftwarePWM_setAll(0); // Clear LED's
	tail();
	SoftwarePWM_setAll(0); // Clear LED's
	inOrderLightUp();
	inOrderLightUpReverse();
	flash();
	moving();		
}