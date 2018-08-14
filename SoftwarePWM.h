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
 * This library is for producing a software PWM output for digital pins.
 * Uses the Timer0 in CTC mode with a 64 prescaler to create a 980hz (1ms~) PWM signal.
 * I built this library as part of a birthday present for my niece, to create a flashing 
 */


#ifndef SOFTWAREPWM_H_
#define SOFTWAREPWM_H_

#include <stdlib.h>
#include <avr/interrupt.h>

// PIN Numbers for the Arduino Uno, Arduino Pro Mini
#if defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
	#define PIN_0 			_BV(0)
	#define PIN_1 			_BV(1)
	#define PIN_2 			_BV(2)
	#define PIN_3 			_BV(3)
	#define PIN_4 			_BV(4)
	#define PIN_5 			_BV(5)

// PIN Numbers for the Arduino Mega, Arduino Pro Micro
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega2560__)
	#define PIN_0 			_BV(4)
	#define PIN_1 			_BV(5)
	#define PIN_2 			_BV(6)
	#define PIN_3 			_BV(2)
	#define PIN_4 			_BV(3)
	#define PIN_5 			_BV(1)

#endif /* PIN Numbers */

#define PWM_MAX				6U					// Max number of PWM outputs

#define ENABLE_PWM			TIMSK1 |= 0x4;		// Fast Enable OCIE1B
#define DISABLE_PWM			TIMSK1 &= 0xFB;		// Fast Disable OCIE1B

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PWM_node {
	uint8_t length;
	uint8_t pins;
	struct PWM_node *tail;
} PWM_node_t;

typedef union {
	PWM_node_t *ptr;
	uint16_t i;
} PWM_ptr_t;

typedef struct {
	uint8_t length;
	uint8_t pin;
} PWM_data_t;

void SoftwarePWM_init();
void SoftwarePWM_clear();
void SoftwarePWM_create();
void SoftwarePWM_setPWM(uint8_t index, uint8_t length);
void SoftwarePWM_setPWMs(uint8_t *index, uint8_t *length, uint8_t size);
void SoftwarePWM_setAll(uint8_t length);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* SOFTWAREPWM_H_ */