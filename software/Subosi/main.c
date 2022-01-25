/*
 * main.c
 *
 * Created: 12/26/2021 5:30:38 PM
 *  Author: Samuel
 */ 

#include <xc.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#include "twislave.h"
#include "debounce.h"

// wake up pin
#define WKUP_PORT	PORTB
#define WKUP_DDR	DDRB
#define WKUP_PIN	(1<<1)

// led in from bluetooth module
#define LED_IN_PORT	PORTB
#define LED_IN_IN	PINB
#define LED_IN_PIN	(1<<0)

// on board led
#define LED_PORT	PORTB
#define LED_DDR		DDRB
#define LED_PIN		(1<<2)

#define LED_OFF()		LED_PORT |= LED_PIN
#define LED_ON()		LED_PORT &= ~LED_PIN


volatile uint8_t initialized = 0;

// 100 Hz ISR
ISR(TIMER0_COMPA_vect)
{
	debounce_update();
	
	if(initialized)
	{
		if(!(LED_IN_IN & LED_IN_PIN))
			LED_ON();
		else
			LED_OFF();
	}
	else
	{
		static uint8_t cnt = 0;
		cnt += 2;
		if (cnt < 128)
			LED_ON();
		else
			LED_OFF();
	}
}

void wakeup(void)
{
	WKUP_PORT |= WKUP_PIN;
	_delay_us(1);
	WKUP_PORT &= ~WKUP_PIN;
}

void write_buffer(uint8_t buffer[2][7])
{
	static uint8_t index = 0;
	
	for(int i = 0; i < 10; i++)
	{
		cli();
		memcpy((void*)txbuffer, buffer[index], sizeof(buffer[0]));
		sei();
		index = 1 - index;
		wakeup();
		_delay_ms(100);
		if(!(LED_IN_IN & LED_IN_PIN))
			break;
	}
}

int main(void)
{	
	// set outputs
	WKUP_DDR |= WKUP_PIN;
	LED_DDR |= LED_PIN;
	
	// enable pull ups
	LED_IN_PORT |= LED_IN_PIN;
	debouce_init();
	
	//init timer
	TCCR0A = 1 << CTC0 | 1 << CS00 | 1 << CS02;	// Clear at top; prescaler 64
	TIMSK0 = 1 << OCIE0A;						// enable overflow interrupt
	OCR0A = F_CPU / 1024 / 100 - 1;				// 100 Hz
	
	uint8_t all_on[2][7] = {
		{0x78, 0x14, 0x55, 0x90, 0x13, 0x17, 0x11},
		{0x79, 0x7C, 0x55, 0x90, 0x66, 0x74, 0x79},
	};
	
	uint8_t all_off[2][7] = {
		{0x7C, 0x13, 0x56, 0x90, 0x13, 0x17, 0x11},
		{0x7D, 0x7B, 0x56, 0x90, 0x66, 0x74, 0x79},
	};
	
	uint8_t group1_on[2][7] = {
		{0x58, 0x12, 0x55, 0x6E, 0x13, 0x17, 0x11},
		{0x5B, 0x7A, 0x55, 0x6E, 0x66, 0x74, 0x79},
	};
	
	uint8_t group1_off[2][7] = {
		{0x4C, 0x12, 0x56, 0x6E, 0x13, 0x17, 0x11},
		{0x4D, 0x79, 0x56, 0x6E, 0x66, 0x74, 0x79},
	};
	
	uint8_t group2_on[2][7] = {
		{0x58, 0x12, 0x55, 0x6E, 0x13, 0x17, 0x11},
		{0x5B, 0x7A, 0x55, 0x6E, 0x66, 0x74, 0x79},
	};
		
	uint8_t group2_off[2][7] = {
		{0x40, 0x10, 0x56, 0x6D, 0x13, 0x17, 0x11},
		{0x41, 0x78, 0x56, 0x6D, 0x66, 0x74, 0x79},
	};

	// init i2c slave
	uint8_t data[] = {0x5B, 0x06, 0x5B, 0x6F, 0x56, 0x63, 0x67, 0x23, 0x00, 0x0F, 0x23};
	memcpy((void*)init_value, data, sizeof(data));
	
	init_twi_slave(0x60);
	sei();
	
	
    while(1)
    {
		if(wakeup_flag)
		{
			_delay_ms(10);
			wakeup();
			wakeup_flag = 0;
			
			initialized = 1;
		}
		
        if(get_key_short(TERM_PIN1))
        {
			write_buffer(group1_off);
		}
		if(get_key_short(TERM_PIN2))
		{
			write_buffer(group1_on);
		}
		if(get_key_short(TERM_PIN3))
		{
			write_buffer(group2_off);
		}
		if(get_key_short(TERM_PIN4))
		{
			write_buffer(group2_on);
		}
		if(get_key_long(TERM_PIN1))
		{
			write_buffer(all_off);
		}
		if(get_key_long(TERM_PIN2))
		{
			write_buffer(all_on);
		}
    }
}