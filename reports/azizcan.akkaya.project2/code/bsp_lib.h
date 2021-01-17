/*
 * bsp_lib.h
 *
 * author: Aziz Can Akkaya
 * number: 171024005
 *
 */

#ifndef BSP_LIB_H_
#define BSP_LIB_H_

#define minus 		(0xBF)
#define LEDDELAY	1000
#define second    	(2000)

static uint32_t overflow[4] = {
		0xC0,
		0xE3,
		0x8E,
		0xC7,
};

static uint32_t digit[4] = {
		0x01,
		0x02,
		0x10,
		0x20,
};

static uint32_t numb[10] = {
		0x40,
		0x79,
		0x24,
		0x30,
		0x19,
		0x12,
		0x02,
		0x78,
		0x00,
		0x10
};

typedef enum{
	one,
	two,
	three,
	four,
	five,
	six,
	seven,
	eight,
	nine,
	zero,
	a,
	b,
	c,
	d,
	e,
	f,
	def
}te_button;

typedef enum{
	stand,
	write
}te_disp_stat;

typedef enum{
	stop,
	start,
	standby,
	end
}te_input;

typedef enum{
	pos,
	neg,
}te_numb_stat;

typedef enum{
	nll,
	add,
	sub,
	mult,
	div
}te_op;

int index;
int row, col;

float res, input;

void op_handler(void);
void row_finder(void);
void display_digit(int number, int dp);
void display_numb(float number);
void clear(void);
void display_overflow(void);
void GPIOX_INIT(void);
void delay(volatile uint32_t);
void state_handler(void);

void EXTI4_15_IRQHandler(void);

#endif /* BSP_LIB_H_ */
