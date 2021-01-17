/*
 * bsp_lib.c
 *
 * author: Aziz Can Akkaya
 * number: 171024005
 *
 */
#include "stm32g0xx.h"
#include "stdint.h"
#include "math.h"
#include "bsp_lib.h"

te_disp_stat edispstat = stand;		//display status
te_button button = def;				//button output status
te_input inp = stop;				//input status
te_numb_stat numb_stat = pos;		//number type
te_op op = nll;						//op type
int counter = 0;					//systick

//operation handler for result
void op_handler(void){
	switch( op ){
		case add:
			res += input;
			op = nll;
			break;

		case sub:
			res -= input;
			op = nll;
			break;

		case mult:
			res *= input;
			op = nll;
			break;

		case div:
			res /= input;
			op = nll;
			break;

		default:
			break;
	}

	res = 0;
}

//Button handler
void char_finder(void){
	if( col == 1 && row == 1 )
		button = one;
	else if( col == 1 && row == 2 )
		button = four;
	else if( col == 1 && row == 3 )
		button = seven;
	else if( col == 1 && row == 4 )
		button = f;
	else if( col == 2 && row == 1 )
		button = two;
	else if( col == 2 && row == 2 )
		button = five;
	else if( col == 2 && row == 3 )
		button = eight;
	else if( col == 2 && row == 4 )
		button = zero;
	else if( col == 3 && row == 1 )
		button = three;
	else if( col == 3 && row == 2 )
		button = six;
	else if( col == 3 && row == 3 )
		button = nine;
	else if( col == 3 && row == 1 )
		button = e;
	else if( col == 4 && row == 1 )
		button = a;
	else if( col == 4 && row == 2 )
		button = b;
	else if( col == 4 && row == 3 )
		button = c;
	else if( col == 4 && row == 4 )
		button = d;
	else
		button = def;

}

//Interrupt handler determines interrupt comes from which row
void EXTI4_15_IRQHandler(void){

	GPIOA->ODR &= 0U;
	uint32_t c;

	GPIOA->ODR ^= (1U << 6);
	c = (GPIOA->IDR >> 10);
	c &= (uint32_t)(0x27);
	if( (c >> 0) ){
		row = 1;
	}
	GPIOA->ODR ^= (1U << 6);

	GPIOA->ODR ^= (1U << 7);
	c = (GPIOA->IDR >> 10);
	c &= (uint32_t)(0x27);
	if( (c >> 0) ){
		row = 2;
	}
	GPIOA->ODR ^= (1U << 7);

	GPIOA->ODR ^= (1U << 8);
	c = (GPIOA->IDR >> 10);
	c &= (uint32_t)(0x27);
	if( (c >> 0) ){
		row = 3;
	}
	GPIOA->ODR ^= (1U << 8);

	GPIOA->ODR ^= (1U << 9);
	c = (GPIOA->IDR >> 10);
	c &= (uint32_t)(0x27);
	if( (c >> 0) ){
		row = 4;
	}
	GPIOA->ODR ^= (1U << 9);
	GPIOA->ODR |= 0x3C0;

	int plexer =  (int)(EXTI->RPR1 >> 10);

	if( plexer == 1 ){
		col = 1;
	}
	else if( plexer == 2 ){
		col = 2;
	}
	else if( plexer == 4 ){
		col = 3;
	}
	else{
		col = 4;
	}

	char_finder();

	if( button == b && inp == stop ){
		numb_stat = neg;
		inp = start;
	}
	else if( button != def )
		inp = start;

	if( button == f ){
		op_handler();
		inp = end;
		counter = 0;
	}

	EXTI->RPR1 |= (7U << 10);
	EXTI->RPR1 |= (1U << 15);
}

//state handler for unlimited loop
void state_handler(void){
	if( inp == stop ){
		display_numb(1705);
	}
	else if( inp == start ){
		counter++;

		if(counter == 10*second)
			inp = stop;

		switch(button){
			case one:
				input *= 10;
				input += 1;
				break;

			case two:
				input *= 10;
				input += 2;
				break;

			case three:
				input *= 10;
				input += 3;
				break;

			case four:
				input *= 10;
				input += 4;
				break;

			case five:
				input *= 10;
				input += 5;
				break;

			case six:
				input *= 10;
				input += 6;
				break;

			case seven:
				input *= 10;
				input += 7;
				break;

			case eight:
				input *= 10;
				input += 8;
				break;

			case nine:
				input *= 10;
				input += 9;
				break;

			case zero:
				input *= 10;
				input += 0;
				break;

			case a:
				op = add;
				break;

			case b:
				op = sub;
				break;

			case c:
				op = mult;
				break;

			case d:
				op = mult;
				break;

			default:
				break;
		}

		if( input >= 0 && numb_stat == neg )
			input *= (-1);

		inp = standby;

		display_numb(input);
	}
	else if( inp == standby ){
		if( input > 9999 || input < -999 ){
			display_overflow();
			counter++;
			if( counter == second ){
				counter = 0;
				input = 0;
			}
		}
		else
			display_numb(input);
	}
	else if( inp == end ){
		display_numb(res);
		counter++;
		if(counter == second ){
			input = 0;
			inp = standby;
			counter = 0;
		}
	}

}

//configuration of pins
void GPIOX_INIT(void){
	RCC->IOPENR |= (3U << 0);

	GPIOB->MODER &= ~(uint32_t)((uint32_t)0xFFFFF << 0);
	GPIOB->MODER |= ((uint32_t)0x05555 << 0);

	GPIOA->MODER &= ~(uint32_t)((uint32_t)0xC3FFFF0F << 0);

	GPIOA->PUPDR |= (2U << 10*2);
	GPIOA->PUPDR |= (2U << 11*2);
	GPIOA->PUPDR |= (2U << 12*2);
	GPIOA->PUPDR |= (2U << 15*2);

	EXTI->EXTICR[1] |= (0U << 10);
	EXTI->EXTICR[1] |= (0U << 15);

	EXTI->RTSR1 |= (7U << 10);
	EXTI->RTSR1 |= (1U << 15);

	EXTI->IMR1 |= (7U << 10);
	EXTI->IMR1 |= (7U << 15);

	NVIC_SetPriority( EXTI4_15_IRQn  , 0);
	NVIC_EnableIRQ( EXTI4_15_IRQn );

	GPIOA->MODER |= ((uint32_t)0x55505 << 0);
	GPIOA->ODR &= 0xFFFFFFC0;
	GPIOA->ODR |= 0x3C0;

}

//clear ssd pins
void clear(void){
	GPIOA->ODR &= (15U << 6);
	GPIOB->ODR &= 0U;
}

//display numb
void display_numb(float number){

	if( number > 9999 || number < -999 ){
		display_overflow();
		return;
	}
	else if( number < 0 ){
		edispstat = write;
		display_digit(-1, 0);
		number *= -1;
		edispstat = stand;
	}
	else if( !number ){
		edispstat = write;
		index = 3;
		display_digit(number, 0);
		index = 0;
		edispstat = stand;
	}

	volatile int thousand = number / 1000;
	if( thousand ){
		number -= (thousand*1000);
		edispstat = write;
	}
	display_digit(thousand, 0);

	volatile int hundreds = number / 100;
	if( hundreds ){
		number -= (hundreds*100);
		edispstat = write;
	}
	display_digit(hundreds, 0);

	volatile int tens = number / 10;
	if( tens ){
		number -= (tens*10);
		edispstat = write;
	}
	display_digit(tens, 0);

	volatile int ones = number;
	if( ones ){
		number -= ones;
		edispstat = write;
	}


	volatile int pten = number*10;
	number = number*10 - pten;

	volatile int phunderd = number*10;
	number = number*10 - phunderd;

	volatile int pthousand = number*10;
	number =  number*10 - pthousand;

	if( pten || phunderd || pthousand ){
		edispstat = write;
		display_digit(ones, 1);
		display_digit(pten, 0);
		display_digit(phunderd, 0);
		display_digit(pthousand, 0);
	}
	else
		display_digit(ones, 0);

	index = 0;
	edispstat = stand;
}

void display_overflow(void) {
	clear();

	for(int i = 0; i < 4; i++ ) {
		GPIOB->ODR |= overflow[i];
		GPIOA->ODR |= digit[i];
		delay(LEDDELAY);
		clear();
	}
}

//digit display according to index
void display_digit(int number, int dp){
	if( index < 4 && index >= 0  ){
		if( edispstat == write ){
			clear();

			if( number < 0 ){
				GPIOB->ODR |= minus;
			}
			else
				GPIOB->ODR |= numb[number];

			if( !dp )
				GPIOB->ODR |= (1U << 7);

			GPIOA->ODR |= digit[index];

			delay(LEDDELAY);

			index++;
		}
	}
}

//delay
void delay(volatile uint32_t s) {
    for(; s>0; s--);
}
