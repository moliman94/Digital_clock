/*
 * timer.c
 *
 * Created: 17-Apr-20 1:25:41 AM
 *  Author: Mohamed Soliman
 */ 

#include "timer.h"

#define PRESCALER_1024

void timer_0_normal_init()
{
	SETBIT(TCCR0, FOC0);
	TCCR0 &= ~((1<<WGM01)|(1<<WGM00));   //select normal mode
	SETBIT(TIMSK, TOIE0);   //enable interrupt on overflow
	SETBIT(SREG, 7);
}

void timer_0_ctc_init()
{
	SETBIT(TCCR0, FOC0);
	//select CTC mode
	TCCR0 |= (1<<WGM01);
	TCCR0 &= ~(1<<WGM00);
	OCR0 = 200;
	SETBIT(TIMSK, OCIE0);   //enable interrupt on compare match
	SETBIT(SREG, 7);
}

void timer_0_start()
{
	TCCR0 &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));
	
#ifdef NO_PRESCALER
	TCCR0 |= 1<<CS00;
#endif

#ifdef PRESCALER_8
	TCCR0 |= 1<<CS01;
#endif

#ifdef PRESCALER_64
	TCCR0 |= (1<<CS01)|(1<<CS00);
#endif

#ifdef PRESCALER_256
	TCCR0 |= 1<<CS02;
#endif

#ifdef PRESCALER_1024
	TCCR0 |= (1<<CS02)|(1<<CS00);   //prescaler 1024
#endif
}

void timer_0_stop()
{
	TCCR0 &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));
}

void timer_2_normal_init()
{
	SETBIT(TCCR2, FOC2);
	TCCR2 &= ~((1<<WGM21)|(1<<WGM20));   //select normal mode
	SETBIT(TIMSK, TOIE2);   //enable interrupt on overflow
	SETBIT(SREG, 7);
}

void timer_2_start()
{
	TCCR0 &= ~((1<<CS22)|(1<<CS21)|(1<<CS20));
	
	#ifdef NO_PRESCALER
	TCCR2 |= 1<<CS20;
	#endif

	#ifdef PRESCALER_8
	TCCR2 |= 1<<CS21;
	#endif

	#ifdef PRESCALER_64
	TCCR2 |= (1<<CS22);
	#endif

	#ifdef PRESCALER_256
	TCCR2 |= (1<<CS22)|(1<<CS21);
	#endif

	#ifdef PRESCALER_1024
	TCCR2 |= (1<<CS22)|(1<<CS21)|(1<<CS20);   //prescaler 1024
	#endif
}

void timer_2_stop()
{
	TCCR2 &= ~((1<<CS22)|(1<<CS21)|(1<<CS20));
}