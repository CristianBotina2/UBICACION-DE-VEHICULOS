/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "ch_test.h"
#include "prueba.h"

/*
 * Blinker thread #1.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;

  chRegSetThreadName("blinker");
  while (true) {
    palSetPad(GPIOD,2); //
    chThdSleepMilliseconds(1000);
    palClearPad(GPIOD, 2); 
    chThdSleepMilliseconds(1000);
  }
}

/*
 * Blinker thread #2.
 */
/*static THD_WORKING_AREA(waThread2, 128);
static THD_FUNCTION(Thread2, arg) {

  (void)arg;

  chRegSetThreadName("blinker");
  while (true) {
    palSetPad(GPIOC, GPIOC_LED3);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOC, GPIOC_LED3);
    chThdSleepMilliseconds(500);
  }
}*/

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit(); //con este llamado, tambien llama a palInit(), que controla los GPIO's
  chSysInit(); //inicia el RT
  boardInit(); //agregado
  int a = TestADCReg;

  /*
   * Activates the serial driver 1 using the driver default configuration.
   * PA9(TX) and PA10(RX) are routed to USART1.
   */
	//  sdStart(&SD1, NULL);
	sdStart(&SD3, NULL); //serial Driver 3 activated. en este caso es PC10  y PC11(del STM32)
	// En mcuconf, se cambia la variable #define STM32_SERIAL_USE_USART3 de FALSE a TRUE
	//En halconf.h, se cambia la velocidad de transmisión UART (por defecto es 38000) a 115200
	// En board.h (ver ruta en MakeFile), se cambia los pines PC10 y PC11
	//#define VAL_GPIOCCRH            4B ->pin 11 y 10 ( *   4 - Digital input.,  *   B - Alternate Push Pull output 50MHz.)
	//En board.c (ver Makefile ruta) se remapean los pines de PC a USART3	
  /*
     USART3 re-mapped to PC10-PC11

  AFIO -> MAPR |= AFIO_MAPR_USART3_REMAP_PARTIALREMAP;
  http://stm32.kosyak.info/doc/globals_0x61.html
  */

  /*
   * Creates the example threads.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL); //corre tarea 1 (blink1)
  //chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO+1, Thread2, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched.
   */
  while (true) {
    /*if (palReadPad(GPIOA, GPIOA_BUTTON))
      test_execute((BaseSequentialStream *)&SD1);
    chThdSleepMilliseconds(500);*/
    chprintf( (BaseChannel *)&SD3, "test %x\n\r"); //cast a puntero de tipo BaseChannel
    //chprintf((BaseChannel *)&a, "prueba");
    //1era prueba: test 21AC
    
  }
}
