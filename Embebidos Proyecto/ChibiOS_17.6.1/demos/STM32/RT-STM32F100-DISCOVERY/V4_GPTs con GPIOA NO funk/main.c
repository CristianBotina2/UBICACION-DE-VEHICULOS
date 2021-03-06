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

//#include "prueba.h"


#define M_TO_CM                     100.0f //f significa float
#define SPEED_OF_SOUND              343.2f

#define ECHO												7
#define TRIGG												6

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


/*Función callback, llamada al cumplirse el tiempo de espera del GPT*/
static void gpt1cb(GPTDriver *gptp) {
  (void)gptp;
  
  palClearPad(GPIOA, TRIGG);
};

static int contador = 0;
static bool lectura = FALSE; 
static float distancia = 0.0; //variable que tendrá la distancia de detección del sensor ultrasonido
/*Función callback, llamada al cumplirse el tiempo de espera del GPT3, y que cuenta el tiempo que está en alto el pin ECHO, generando además, la distancia de detección */
static void gpt3cb(GPTDriver *gptp) {
	(void)gptp;
	contador = contador + 1;
};

/*configuración de GPTConfig nombrado como gpt1cfg, necesario para el gptStart() */
static const GPTConfig gpt1cfg = {
  500000,    /* 500kHz timer clock.*/
  gpt1cb,    /* Timer callback.*/
  0
};

/*configuración de GPTConfig nombrado como gpt3cfg, necesario para el gptStart() */
static const GPTConfig gpt3cfg = {
  500000,    /* 500kHz timer clock.*/
  gpt3cb,    /* Timer callback.*/
  0
};

/*--------------     -----------       -----------*/
//static BaseSequentialStream * chp = (BaseSequentialStream*)&SD3;

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
   
  /* Inicio de GPT (General Purpose Timer), para medir el pulso a enviar al ultrasonido HC-SR04 */
  gptStart(&GPTD1, &gpt1cfg); //argumento GPTD1 (TIM1) como apuntador. igualmente, se envía apuntador de la configuración (más 
  
  gptStart(&GPTD3, &gpt3cfg); //argumento GPTD3 (TIM3) como apuntador. igualmente, se envía apuntador de la configuración (más arriba)
  
  palSetPad(GPIOA, 0);
  
  palClearPad(GPIOA, TRIGG);
  chThdSleepMilliseconds(3500);
  chprintf( (BaseChannel *)&SD3, "began %x\n\r", PAL_LOW);
  /*
   * Creates the example threads.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL); //corre tarea 1 (blink1)
  //chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO+1, Thread2, NULL);
	
  palSetPad(GPIOA, TRIGG);
  gptStartOneShot(&GPTD1, 10);  // dT = 500,000 / 10 = 50000 Hz , 20us
  chprintf( (BaseChannel *)&SD3, "shot %x\n\r", PAL_LOW);
	
	gptStop(&GPTD1);
	/*GPIOA, 0 es de prueba (debug)*/
  palClearPad(GPIOA, 0);
  while(palReadPad(GPIOA, ECHO) == PAL_LOW){
		//aqui en este While no es necesario hacer algo, solo esperar a que salga.
	};
	//chprintf( (BaseChannel *)&SD3, "N %s\n\r", PAL_LOW);
	//gptStartContinuous(&GPTD1, 10)
	while(palReadPad(GPIOA, ECHO) == PAL_HIGH){
		gptStartContinuous(&GPTD3, 10);  // dT = 500,000 / 10 = 50000 Hz , 20us
	};
	
	if(contador >= 1900){ // 38ms/20us = 1900 o más
		lectura = FALSE;
		distancia = 38; //si distancia toma este valor, la medida no es fiable, o no hay obstáculo
	} else {
		int anchoPulso = contador * 0.000020; //en segundos
  	distancia = (SPEED_OF_SOUND * anchoPulso* M_TO_CM) / (2); //Cálculo de la distancia
  };
  
  gptStop(&GPTD3);// al colocar esta línea, ya ni siquiera entra el While de debajo. Puede ser que al parar el GPT3, este corriendo el callback y no se pueda detener correctamente; es decir, la Maquina Estados no estaría en GPT_CONT...S
  
  while (true) {
  	chprintf( (BaseChannel *)&SD3, "Count %d\n\r", contador);
  	chprintf( (BaseChannel *)&SD3, "test %x\n\r"); //cast a puntero de tipo BaseChannel
		chprintf( (BaseChannel *)&SD3, "valor: %d\n\r", distancia);
		chprintf( (BaseChannel *)&SD3, "paso oneShot %x\n\r");
		chThdSleepMilliseconds(500);    
  }
  
}
