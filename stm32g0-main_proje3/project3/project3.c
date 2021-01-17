/*
 * project3.c
 *
 * author: Aziz Can Akkaya
 * number: 171024005
 *
 */

#include "stm32g0xx.h"
#include "bsp_lib.h"

int main(void) {

	BSP_System_Init();


    while(1) {
    	state_handler();
    }

    return 0;
}


