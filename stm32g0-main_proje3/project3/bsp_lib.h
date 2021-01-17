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
#define second    	(1000)
#define track_cycle (80000000)

typedef enum{
	start,
	idlee,
	track_selected,
	record,
	plb,
	del,
	avab
}te_loop_status;

typedef enum{
	none,
	track1,
	track2,
	track3,
	track4
}te_track_selection;

typedef enum{
	standby,
	pause
}te_track_stat;

typedef enum{
	one,
	two,
	three,
	four,
	stop_pause,
	delete,
	track_status,
	playback,
	nop
}te_button_stat;

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

static uint32_t id[4] = {
	0x79,
	0x21,
	0x47,
	0x06
};

static uint32_t stat[3] = {
	0x08,
	0x63,
	0x33,
};

static uint32_t rec[3] = {
	0x2F,
	0x27,
	0x20,
};

static uint32_t play[4] = {
	0x0C,
	0x47,
	0x03
};

static uint32_t idle[4] = {
	0x4F,
	0x20,
	0x47,
	0x06
};

static uint32_t full[4] = {
	0x0E,
	0x63,
	0x47,
	0x47
};

void BSP_System_Init(void);
void display_start(void);

void enable_adc(void);
void disable_adc(void);

void display_status(void);
void display_record(int x);

void display_playback(int x);
void display_idle(void);

void display_full(void);
void button_handler(void);

void EXTI4_15_IRQHandler(void);
uint8_t read_I2C(uint8_t devAddr, uint8_t regAddr);

void write_I2C(uint8_t devAddr, uint8_t regAddr, uint8_t data);
void track_handler();

void state_handler(void);
void record_track(void);

void track_play(void);
void display_numb(int a);

void adc_getter(void);
void clear(void);
void delay(volatile uint32_t s);

#endif /* BSP_LIB_H_ */
