/************************************************************************/
/*                                                                      */
/*                      Debouncing 8 Keys                               */
/*                      Sampling 4 Times                                */
/*                      With Repeat Function                            */
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                      danni@specs.de                                  */
/*                                                                      */
/************************************************************************/

#pragma once

#define KEY_DDR         DDRD
#define KEY_PORT        PORTD
#define KEY_PIN         PIND

#define TERM_PIN1	(1<<4)
#define TERM_PIN2	(1<<5)
#define TERM_PIN3	(1<<6)
#define TERM_PIN4	(1<<7)

#define ALL_KEYS        (TERM_PIN1|TERM_PIN2|TERM_PIN3|TERM_PIN4)

#define REPEAT_MASK     (TERM_PIN1 | TERM_PIN2)  // repeat: key1, key2
#define REPEAT_START    50                        // after 500ms
#define REPEAT_NEXT     20                        // every 200ms


void debounce_update(void);		// call every 10 ms

///////////////////////////////////////////////////////////////////
//
// check if a key has been pressed. Each pressed key is reported
// only once
//
uint8_t get_key_press(uint8_t key_mask);

///////////////////////////////////////////////////////////////////
//
// check if a key has been pressed long enough such that the
// key repeat functionality kicks in. After a small setup delay
// the key is reported being pressed in subsequent calls
// to this function. This simulates the user repeatedly
// pressing and releasing the key.
//
uint8_t get_key_rpt(uint8_t key_mask);

///////////////////////////////////////////////////////////////////
//
// check if a key is pressed right now
//
uint8_t get_key_state(uint8_t key_mask);

///////////////////////////////////////////////////////////////////
//
uint8_t get_key_short(uint8_t key_mask);

///////////////////////////////////////////////////////////////////
//
uint8_t get_key_long(uint8_t key_mask);

void debouce_init(void);