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

int ava = 1;

int row;
int col;
int ticker = 0;
uint32_t adc_val;

te_loop_status loop = start;
te_track_selection track;
te_button_stat button;
te_track_stat plb_stat;



//configuration of pins
void BSP_System_Init(void){
	RCC->IOPENR |= (3U << 0);

	GPIOB->MODER &= ~(uint32_t)(0xFFFF);
	GPIOB->MODER |= ((uint32_t)0xD555);

	GPIOA->MODER &= ~(uint32_t)(0x3FCFF0F);
	GPIOA->MODER |= (uint32_t)(0x0035505);

	clear();

	GPIOA->PUPDR |= (2U << 9*2);
	GPIOA->PUPDR |= (2U << 10*2);
	GPIOA->PUPDR |= (2U << 11*2);
	GPIOA->PUPDR |= (2U << 12*2);

	EXTI->EXTICR[1] |= (0U << 9);

	EXTI->RTSR1 |= (15U << 9);

	EXTI->IMR1 |= (15U << 9);

	NVIC_SetPriority( EXTI4_15_IRQn  , 0);
	NVIC_EnableIRQ( EXTI4_15_IRQn );

	GPIOA->ODR |= (3U << 6);

	//adc clock
	RCC->APBENR2 |= (1U << 20);
	//voltage regulator
	ADC1->CR |= (1U << 28);

	delay(1000);
	//calibration
	ADC1->CR |= (1U << 31);

	for(;;){
		if( !((ADC1->CR >> 31)) )
			break;
	}

	//8 bit res
	ADC1->CFGR1 |= (1U << 4);
	//channel sel
	ADC1->CHSELR |= (1U << 11);
	ADC1->ISR |= (1U << 11);
	ADC1->CFGR1 |= (1U << 13);

	ADC1->SMPR |= (0U << 0);
	ADC1->SMPR |= (0U << 1);
	ADC1->SMPR |= (0U << 2);

	ADC1->CR |= (1U << 0);

	//START CONVERSION
	//ADC1->CR |= (1U << 2);

	//I2C

	GPIOB->MODER &= ~(uint32_t)(0xF0000);
	GPIOB->MODER |= (0xA0000);


	//drain
	GPIOB->OTYPER |= (2U << 8);

	//PB8
	GPIOB->AFR[1] &= ~(0xF << 0 );
	GPIOB->AFR[1] |= (6U << 0 );
	//PB9
	GPIOB->AFR[1] &= ~(0xF << 4 );
	GPIOB->AFR[1] |= (6U << 4 );

	//ENABLE I2C
	RCC->APBRSTR1 |= (1U << 21);

	//ERRI
	I2C1->CR1 |= (1U << 7);
	//NACK
	I2C1->CR1 |= (1U << 15);
	//ATUOEND
	I2C1->CR1 |= (1U << 25);
	//PRESC
	I2C1->TIMINGR |= (3 << 28);
	//SCLL
	I2C1->TIMINGR |= (0x13 << 0);
	//SCLH
	I2C1->TIMINGR |= (0xF << 8);
	//SDADEL
	I2C1->TIMINGR |= (0x2 << 16);
	//SCLDEL
	I2C1->TIMINGR |= (0x4 << 20);

	//PE
	I2C1->CR1 |= (1U << 0);

	NVIC_SetPriority( I2C1_IRQn  , 0);
	NVIC_EnableIRQ( I2C1_IRQn );

	//TIM 16 SET-UP
	//PB0 as AF mode
	GPIOA->MODER &= ~(3U << 2*8);
	GPIOA->MODER |= (2U << 2*8);

	GPIOA->AFR[1] &= ~((uint32_t)0xF << 4);
	GPIOA->AFR[1] |= (2U << 4);

	RCC->APBENR2 |= (1U << 11); // enable TIM1 clock

	TIM1->CR1 = 0U; // resetting control register
	TIM1->CR1 |= (1U << 7); // ARPE buffering
	TIM1->CNT = 0U; // timer counter reset

	TIM1->PSC = ((SystemCoreClock/10000) - 1); // setting prescaler to 1600-1
	TIM1->ARR = 1U; // set the autoreload register to 1 milliseconds

	TIM1->CR1 |= (1U << 0); // enable TIM1

	TIM1->CCMR1 |= (6U << 4);
	TIM1->CCMR1 |= (1U << 3);
	TIM1->CCER |= (1U << 0);
	TIM1->CCR1 |= (1U << 0);
	TIM1->BDTR |= (1U << 15);

}


void display_start(void){
	clear();
	GPIOB->ODR |= numb[1];
	GPIOA->ODR |= digit[0];
	delay(LEDDELAY);

	clear();
	GPIOB->ODR |= numb[7];
	GPIOA->ODR |= digit[1];
	delay(LEDDELAY);

	clear();
	GPIOB->ODR |= numb[0];
	GPIOA->ODR |= digit[2];
	delay(LEDDELAY);

	clear();
	GPIOB->ODR |= numb[5];
	GPIOA->ODR |= digit[3];
	delay(LEDDELAY);

	clear();
}

void enable_adc(void){ ADC1->CR |= (1U << 2); }
void disable_adc(void) { ADC1->CR |= (0U << 2); }

void display_status(void){
	for(int i = 0; i < 3; i++){
		clear();
		GPIOB->ODR |= stat[i];
		GPIOA->ODR |= digit[i];
		delay(LEDDELAY);
	}

	clear();
	GPIOB->ODR |= numb[ava];
	GPIOA->ODR |= digit[3];
	delay(LEDDELAY);
	clear();
}

void display_record(int x){
	for(int i = 0; i < 3; i++){
		clear();
		GPIOB->ODR |= rec[i];
		GPIOA->ODR |= digit[i];
		delay(LEDDELAY);
	}

	clear();
	GPIOB->ODR |= numb[x];
	GPIOA->ODR |= digit[3];
	delay(LEDDELAY);
	clear();
}

void display_playback(int x){
	for(int i = 0; i < 3; i++){
		clear();
		GPIOB->ODR |= play[i];
		GPIOA->ODR |= digit[i];
		delay(LEDDELAY);
	}

	clear();
	GPIOB->ODR |= numb[x];
	GPIOA->ODR |= digit[3];
	delay(LEDDELAY);
	clear();
}

void display_idle(void){
	for(int i = 0; i < 4; i++){
		clear();
		GPIOB->ODR |= idle[i];
		GPIOA->ODR |= digit[i];
		delay(LEDDELAY);
	}
	clear();
}

void display_full(void){
	for(int i = 0; i < 4; i++){
		clear();
		GPIOB->ODR |= full[i];
		GPIOA->ODR |= digit[i];
		delay(LEDDELAY);
	}
	clear();
}

void button_handler(void){
	if( row == 1 && col == 1 )
		button = one;

	else if(  row == 1 && col == 2 )
		button = two;

	else if(  row == 1 && col == 3 )
		button = three;

	else if(  row == 1 && col == 4 ){
		button = stop_pause;
	}


	else if(  row == 2 && col == 1 )
		button = four;

	else if(  row == 2 && col == 2 )
		button = track_status;

	else if(  row == 2 && col == 3 )
		button = playback;

	else if(  row == 2 && col == 4 )
		button = delete;

	else
		button = nop;

}

void EXTI4_15_IRQHandler(void){

	uint32_t c;
	c = (GPIOA->IDR >> 9);
	c &= (uint32_t)(0xF);

	if( c ){
		GPIOA->ODR ^= (1U << 6);
		if( (c >> 0) ){
			row = 1;
		}
		GPIOA->ODR ^= (1U << 6);

		GPIOA->ODR ^= (1U << 7);
		c = (GPIOA->IDR >> 9);
		c &= (uint32_t)(0xF);
		if( (c >> 0) ){
			row = 2;
		}
		GPIOA->ODR ^= (1U << 7);

		int plexer =  (int)(EXTI->RPR1 >> 9);

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
		button_handler();

		EXTI->RPR1 |= (15U << 9);

		ticker = 0;
		loop = track_selected;
		return;
	}

	row = 0;
	col = 0;
	button_handler();

	EXTI->RPR1 |= (15U << 9);
}

uint8_t read_I2C(uint8_t devAddr, uint8_t regAddr){

	I2C1->CR2 |= ((uint32_t)devAddr << 1 );
	I2C1->CR2 |= (1U << 16);
	I2C1->CR2 |= (1U << 13);

	while(I2C1->ISR & (1 << 1));

	I2C1->TXDR |= (uint32_t)regAddr;

	I2C1->CR2 |= 0;
	I2C1->CR2 |= ((uint32_t)devAddr << 1);
	I2C1->CR2 |= (1U << 10); //READ MODE
	I2C1->CR2 |= (1U << 16); //BYTES TO READ
	I2C1->CR2 |= (1U << 25);
	I2C1->CR2 |= (1U << 1); //GENERATE START

	while(!(I2C1->ISR & (1 << 1)));
	I2C1->TXDR |= (uint32_t)regAddr;

	uint8_t data = I2C1->RXDR;
	return data;

}

void write_I2C(uint8_t devAddr, uint8_t regAddr, uint8_t data){

	I2C1->CR2 |= ((uint32_t)devAddr << 1 );
		I2C1->CR2 |= (1U << 16);
		I2C1->CR2 |= (1U << 13);

		while(I2C1->ISR & (1 << 1));

		I2C1->TXDR |= (uint32_t)regAddr;

		I2C1->CR2 |= 0;
		I2C1->CR2 |= ((uint32_t)devAddr << 1);
		I2C1->CR2 |= (0U << 10); //WRITE MODE
		I2C1->CR2 |= (1U << 16); //BYTES TO WRITE
		I2C1->CR2 |= (1U << 25);
		I2C1->CR2 |= (1U << 1); //GENERATE START

		while(!(I2C1->ISR & (1 << 1)));
		I2C1->TXDR |= (uint32_t)regAddr;

		I2C1->TXDR |= (uint32_t)data;

}

void track_handler(){
	ticker++;
	if( ticker == second/100 ){
		ticker = 0;
		loop = idlee;
		return;
	}

	switch(button){
		case one:
			track = track1;
			for(int i = 0; i < second; i++)
				display_numb(1);
			break;
		case two:
			track = track2;
			for(int i = 0; i < second; i++)
				display_numb(2);
			break;
		case three:
			track = track3;
			for(int i = 0; i < second; i++)
				display_numb(3);
			break;
		case four:
			track = track4;
			for(int i = 0; i < second; i++)
				display_numb(4);
			break;
		case track_status:
			loop = avab;
			break;
		case delete:
			ava = 1;
			break;

		case playback:
			loop = plb;
			break;
		case stop_pause:
			loop = record;
			break;
		default:
			break;

	}


}

void state_handler(void){
	switch(loop){
		case start:
			for(int i = 0; i < second; i++)
				display_start();
			loop = idlee;
			break;

		case idlee:
			display_idle();
			break;
		case track_selected:
			track_handler();
			break;
		case avab:
			if( ava ){
				for(int i = 0; i < second; i++)
					display_status();
			}
			else{
				for(int i = 0; i < second; i++)
					display_full();
			}
			loop = idlee;
			break;
		case plb:
			track_play();
			break;

		case record:
			record_track();
			break;

		default:
			display_idle();
			break;
	}
}

void record_track(void){
	ava = 0;
	enable_adc();
	for(int i = 0; i < second*5; i++ ){
		//read and write the sample to EEPROM
		display_record(1);
	}
	disable_adc();
	loop = idlee;
}

void track_play(void){
	button == nop;
	//read from i2c
	//configure pwm duty cycle
	for(int i = 0; i < second*5; i++ ){
		display_playback(1);
		if(button == stop_pause){
			button = nop;
			while( button != stop_pause);
			button = nop;
		}
	}
	loop = idlee;
}

void display_numb(int a){
	clear();
	GPIOB->ODR |= numb[a];
	GPIOA->ODR |= digit[3];
	delay(LEDDELAY);
	clear();
}

void adc_getter(void){
	uint32_t dummy;

	if( ((ADC1->ISR >> 2 ) & 1)){
	   	adc_val = ADC1->DR;
	}
}

void clear(void){
	GPIOA->ODR &= (3U << 6);
	GPIOB->ODR &= 0U;
}

//delay
void delay(volatile uint32_t s) {
    for(; s>0; s--);
}
