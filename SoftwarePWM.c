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

#include "SoftwarePWM.h"

/*
 * Software PWM Globals
 */

PWM_data_t PWM_DATA[PWM_MAX];
PWM_node_t *PWM_ITEM;
PWM_ptr_t PWM_HEAD, PWM_NEXT;
uint8_t PIN_ALL, PWM_ACTIVE_PINS, PWM_COUNT;
volatile uint8_t PWM_HAS_NEXT;


/*
 * Progresses through the list of PWM values and disables
 * pin outs for active pins
 */
ISR(TIMER1_COMPB_vect) {
	PORTB &= PWM_ITEM->pins;		// Disables Pin outs
	PWM_ITEM = PWM_ITEM->tail;		// Progresses through the list
	if (!PWM_ITEM)					// Loops back to the head of the list
		PWM_ITEM = PWM_HEAD.ptr;
	OCR1B = PWM_ITEM->length;		// Sets the value of the next interrupt
}

/*
 * Enable pin outs for active pins
 * Swaps to a new set of commands if they're available
 */
ISR(TIMER1_COMPA_vect) {	
	if (PWM_HAS_NEXT) {					// Next Flag Check
		// XOR swap algorithm
		PWM_HEAD.i ^= PWM_NEXT.i;
		PWM_NEXT.i ^= PWM_HEAD.i;
		PWM_ITEM = (PWM_node_t *) (PWM_HEAD.i ^= PWM_NEXT.i);

		OCR1B = PWM_HEAD.ptr->length;	// Sets the value of the next interrupt

		PWM_HAS_NEXT = 0;				// Disable Next Flag
	}	
	PORTB = PWM_ACTIVE_PINS;			// Enable pin outs for active pins
}

/*
 * Clears up the memory used by a previous set of commands
 */
void SoftwarePWM_clear() {
	PWM_node_t *item, *next;

	while (PWM_HAS_NEXT);	// Holds future changes in commands until ready

	item = PWM_NEXT.ptr;

	while (item) {
		next = item->tail;
		free(item);			// Freeing up memory
		item = next;
	}

	PWM_NEXT.ptr = 0;		// Clearing pointer
}

/*
 * Creates the next set of PWM commands
 * Inserts commands starting from the head to the tail
 */
void SoftwarePWM_create() {
	PWM_node_t *item, *prev;
	uint8_t active, length, pin;

	active = 0;

	for (uint8_t i = 0; i < PWM_MAX; ++i)	{

		length = PWM_DATA[i].length;			// Storing values to reduce the amount of dereferencing

		if (length > 0) {
			pin = PWM_DATA[i].pin;				// Storing values to reduce the amount of dereferencing
			item = PWM_NEXT.ptr;
			prev = 0;

			while (item && length > item->length) {		// Finding the position for the next command
				prev = item;
				item = item->tail;
			}

			if (!item || length < item->length) {		// Insert a new command

				PWM_node_t *insert = malloc (sizeof (PWM_node_t));	// Allocating memory for a new command
				insert->length = length;
				insert->pins = PIN_ALL ^ pin;
				insert->tail = item;

				if (prev)
					prev->tail = insert;		// Add command to tail			
				else
					PWM_NEXT.ptr = insert;		// Set command to head	
					
			}
			else if (length == item->length)	// Enable pin out to a command
				item->pins ^= pin;
			active |= pin;						// Enable active pins
		}
	}
	PWM_ACTIVE_PINS = active;					// Set Active commands
	PWM_HAS_NEXT = 1;							// Enable next commands flag

	if (active) {								// Active pins
		ENABLE_PWM;								// Enable PWM timer							
	}
	else {
		DISABLE_PWM;							// Disable PWM timer
	}
}


/*
 * Sets a single PWM value to a pin
 */
void SoftwarePWM_setPWM(uint8_t index, uint8_t length) { 
	SoftwarePWM_clear();
	PWM_DATA[index].length = length;
	SoftwarePWM_create();
}

/*
 * Sets multiple PWM values to pins
 */
void SoftwarePWM_setPWMs(uint8_t *index, uint8_t *length, uint8_t size) { 
	SoftwarePWM_clear();

	for (uint8_t i = 0; i < size; i++)
		PWM_DATA[index[i]].length = length[i];

	SoftwarePWM_create();
}

/*
 * Sets a PWM value to all pins
 */
void SoftwarePWM_setAll(uint8_t length) {
	SoftwarePWM_clear();

	for (uint8_t i = 0; i < PWM_MAX; i++)	
		PWM_DATA[i].length = length;

	SoftwarePWM_create();
}


/*
 * Initializes Software PWM
 */
void SoftwarePWM_init() {
	// Storing pin values for easy referencing
	PWM_DATA[0].pin = PIN_0;
	PWM_DATA[1].pin = PIN_1;
	PWM_DATA[2].pin = PIN_2;
	PWM_DATA[3].pin = PIN_3;
	PWM_DATA[4].pin = PIN_4;
	PWM_DATA[5].pin = PIN_5;

	PIN_ALL = (PIN_0 | PIN_1 | PIN_2 | PIN_3 | PIN_4 | PIN_5);	// Global to reference all pins at once

	DDRB = PIN_ALL;								// Sets all pins as outputs

	PWM_HAS_NEXT = 0;

	/*
	 * Setting up the timer for PWM cycles
	 */
	cli();										// Disable interrupts
	TCCR1A = 0;
	TCCR1B = 0;
	TIFR1 |= _BV(OCF1A) | _BV(OCF1B);			// Clearing flagged interrupts
	//TCCR1B = _BV(CS12) | _BV(WGM12);			// 256 prescaler CTC Mode
	TCCR1B = _BV(CS10) | _BV(CS11) | _BV(WGM12);// 64 prescaler CTC Mode
	//TCCR1B = _BV(CS11) | _BV(WGM12);			// 8 prescaler CTC Mode
	TIMSK1 |= _BV(OCIE1A);						// Output compare register A Enable
	OCR1A = 0x100;								// Setting the CTC compare register to 256
	sei();
	
}