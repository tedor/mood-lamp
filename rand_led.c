/*
 * rand_led.c
 */ 

#define F_CPU 10000000UL

#define PORT_LED_RED OCR0A
#define PORT_LED_GREEN OCR1A
#define PORT_LED_BLUE OCR1B

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

volatile int timer_count = 0;

ISR(TIMER0_OVF_vect) 
{	
	if(++timer_count > 255) {
		timer_count = 0;
	}
}

void delay_ms(unsigned int delay) 
{
	while (delay--)
	{
		_delay_ms(1);
	}
}

void delay_us(unsigned int delay) 
{
	while (delay--)
	{
		_delay_us(1);		
	}
}

int main(void)
{
	// Init timer 0
	TCCR0A |= (1<<COM0A1) | (1<<COM0B1) | (1<<WGM01) | (1<<WGM00);
	TCCR0B |= (1<<CS01);
	TIMSK |= (1<<TOIE0);
	
	// Init timer 1
	TCCR1A |= (1<<COM1A1) | (1<<COM1B1) | (1<<WGM12) | (1<<WGM10);
	TCCR1B |= (1<<CS11);
	
	// Set starting pwr values
	PORT_LED_RED = 85;
	PORT_LED_GREEN = 85;
	PORT_LED_BLUE = 85;
	
	// Init ports as output
	DDRB |= (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB5);
	
	// Allow interrupts
	sei();
	
	timer_count = 100;
	
    while(1)
    {
		delay_ms(timer_count*10);		
		unsigned int led_red_now = timer_count;
		delay_ms(timer_count);
		unsigned int led_green_now = 0;
		unsigned int led_blue_now = 0;
		if(timer_count > 128) {
			led_green_now = timer_count + led_red_now;
			if(led_green_now > (255 - led_red_now)) led_green_now = 255 - led_red_now;
			led_blue_now = 255 - (led_red_now + led_green_now);
		} else {
			led_blue_now = timer_count + led_red_now;
			if(led_blue_now > (255 - led_red_now)) led_blue_now = 255 - led_red_now;
			led_green_now = 255 - (led_red_now + led_blue_now);
		}	
		
		unsigned int tmp = 0;
		
		if(timer_count >=0 && timer_count < 85) {
			tmp = led_red_now;
			led_red_now = led_green_now;
			led_green_now = tmp;
		} else if (timer_count >= 85 && timer_count < 170) {
			tmp = led_red_now;
			led_red_now = led_blue_now;
			led_blue_now = tmp;
		}						
						
		unsigned int i = 255;
		
		PORTB ^= (1 << PB5);
				
		while (i--)
		{			
			if(led_red_now < PORT_LED_RED) {
				PORT_LED_RED--;
			} else if(led_red_now > PORT_LED_RED) {
				PORT_LED_RED++;
			}

			if(led_green_now < PORT_LED_GREEN) {
				PORT_LED_GREEN--;
			} else if(led_green_now > PORT_LED_GREEN) {
				PORT_LED_GREEN++;
			}

			if(led_blue_now < PORT_LED_BLUE) {
				PORT_LED_BLUE--;
			} else if(led_blue_now > PORT_LED_BLUE) {
				PORT_LED_BLUE++;
			}
			delay_us(800);
		}		
    }
}