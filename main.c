/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co. KG                 *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 2014 - 2017  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start
*/

#include <stdio.h>
#include <stdlib.h>

#include "Drivers\STM32F7xx_HAL_Driver\Inc\stm32f7xx_hal.h"

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/
void main(void) {
  int i;

  for (i = 0; i < 100; i++) {
    printf("Hello World %d!\n", i);
  }
  do {
    i++;
  } while (1);
}

/*************************** End of file ****************************/
