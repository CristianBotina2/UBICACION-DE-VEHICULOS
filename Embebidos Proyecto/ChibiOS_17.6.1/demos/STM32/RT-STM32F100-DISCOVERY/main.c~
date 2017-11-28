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

//int32_t n = 0;
static void i2notify( io_queue_t *qp) {
	(void)qp;
	
	chprintf( (BaseChannel *)&SD3, "callback %x\n\r", PAL_LOW );
	
	
	/*
	int32_t n = sdRequestDataI(&SD2);
	
	if( n == MSG_TIMEOUT){
		n = "pailas esto";
	}
	
	chprintf( (BaseChannel *)&SD3, "callback %x\n\r", n );*/
}

/*Función callback, llamada al cumplirse el tiempo de espera del GPT*/
static void gpt1cb(GPTDriver *gptp) {
  (void)gptp;
  
  palClearPad(GPIOA, TRIGG);
};

static int contador = 0;
static int lectura = 0; 
static float distancia = 0.0; //variable que tendrá la distancia de detección del sensor ultrasonido
/*Función callback, llamada al detectarse flanco (subida en este caso), para conseguir el ancho del pulso */
static void icuwidthcb(ICUDriver *icup) {
	
	//osalSysLockFromISR;
  icucnt_t anchoPulso = icuGetWidthX(icup); //llama a funcion para conseguir el ancho del pulso, y retorna en ticks (es un uint16_t)
  contador = anchoPulso;
  
  if(anchoPulso < 30 || anchoPulso > 2400){ //30 corresponde a 5cm y 2400 a 400 cm
  	distancia=0;
  }else{
		distancia = (SPEED_OF_SOUND * anchoPulso * M_TO_CM) / (100000 * 2); //100000 pues es la frec. colocada en ICUConfig icu2cfg
		lectura = 1;
		chprintf( (BaseChannel *)&SD3, "distancia %f cm\n\r", distancia);
  }
  //osalSysUnlockFromISR;
};

/*configuración de GPTConfig nombrado como gpt1cfg, necesario para el gptStart() */
static const GPTConfig gpt1cfg = {
  500000,    /* 500kHz timer clock.*/
  gpt1cb,    /* Timer callback.*/
  0
};

/*configuración de ICUConfig nombrado como gpt1cfg, necesario para el gptStart() */
static const ICUConfig icu3cfg = {
  ICU_INPUT_ACTIVE_HIGH,   // modo de funcionamiento, comienza a contar en flanco subida
  100000,    //100 kHz, cada tick es de 10us.
  icuwidthcb,
  NULL,
  NULL,
  ICU_CHANNEL_2,
  0
};

static const SerialConfig sd2cfg = {9600};
/*--------------     -----------       -----------*/
//static BaseSequentialStream * chp = (BaseSequentialStream*)&SD3;

/*
 * Application entry point.
 */
 static BaseChannel * chan = (BaseChannel *)&SD3;
 static BaseChannel * chan2 = (BaseChannel *)&SD2;

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
  char data[24] = "hallo mundo \n \r";
  char info[128] ="info: \n";
  chThdSleepMilliseconds(2500);
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL); //corre tarea 1 (blink1)
  //chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO+1, Thread2, NULL);
	
	//osalSysLock();
	 //parece que no se pudiese colocar nada despues de esta linea (excepto el true)
	//osalSysUnlock();
	
	//sdObjectInit(&SD2, i2notify, NULL );
	//sdStart(&SD2, &sd2cfg);
	//uint8_t f = sdRead(&SD2, &info, 80);
	uint8_t n = 0;
	//n = sdReadTimeout(&SD2, &info, 128, 20000);
 
	while (true) {
   
		/* Inicio de GPT (General Purpose Timer), para medir el pulso a enviar al ultrasonido HC-SR04 */
		gptStart(&GPTD1, &gpt1cfg); //argumento GPTD1 (TIM1) como apuntador. igualmente, se envía apuntador de la configuración (más 
		/* Inicio de ICU (Input Capture Driver), para medir el tiempo que tarda en alto el pulso enviado por el echo -  ultrasonido HC-SR04. Basado en ejemplo en PlayEmbeded.com */
		icuStart(&ICUD3, &icu3cfg); //argumento ICUD3 (TIM3, el TIM2 ya es usado por chibios) como apuntador. igualmente, se envía apuntador de la configuración (más arriba)
		
		palSetPad(GPIOA, 0);
		
		palClearPad(GPIOA, TRIGG);
		
		chprintf( (BaseChannel *)&SD3, "began %x\n\r", PAL_LOW);
		/*
		 * Creates the example threads.
		 */

		palSetPad(GPIOA, TRIGG);
		gptStartOneShot(&GPTD1, 10);  // dT = 500,000 / 10 = 50000 Hz , 20us
		
		distancia = 0.0; //para reiniciar el valor de distancia
		icuStartCapture(&ICUD3); //comienza la captura o espera a un flanco de subida
		icuEnableNotifications(&ICUD3);
		chThdSleepMilliseconds(100); //al menos 60 ms acepta el US para ser leido de nuevo, se le da algo mas de holgura de tiempo
		chprintf( (BaseChannel *)&SD3, "shot ICU %x\n\r", PAL_LOW);
	
		gptStop(&GPTD1);
		/*GPIOA, 0 es de prueba (debug)*/
		palClearPad(GPIOA, 0);
		
		icuStopCapture(&ICUD3);
		icuStop(&ICUD3);
		
		//chThdSleepMilliseconds(500);
		
		//distancia = (SPEED_OF_SOUND * contador * M_TO_CM) / (100000 * 2);
		if(lectura){
			//distancia = 22.0;
			chprintf( (BaseChannel *)&SD3, "if %x\n\r", PAL_LOW);
		}else{
			distancia = 38.0;
			chprintf( (BaseChannel *)&SD3, "els %x\n\r", PAL_LOW);
		}
		
//		n = oqIsEmptyI(&SD2);
		//n = iqReadTimeout(&SD2);
		
		//osalSysLock();
		//n = streamRead(&SD2, &info, 150); //funciona sin timeout
		//n = chnRead(&SD2, &info, 150); //funciona sin timeout
		sdStart(&SD2, &sd2cfg);
		n = chnReadTimeout 	(&SD2, &info, 128, TIME_INFINITE); //funciona con timeout, lee 100 primeros bytes, pero no del canal sino del inicio de transmisión, esta función es de tipo blocking
		
		//n = chnReadTimeout 	(&SD2, &info, 100, TIME_INMEDIATE); lee lo que esta en el channel
		//osalSysUnlock();
		//n = sdAsynchronousRead(&SD2, &info, 82);
		
		/*		
		int32_t n = 0;
		int32_t f = 81;
		
		n = chnGetTimeout((BaseChannel *)&SD2, 20000);
		
		chprintf( (BaseChannel *)&SD3, "ya paso cb %c\n\r", f);
		
		if( n == STM_TIMEOUT){
			n = 123456;
		}
		chprintf( (BaseChannel *)&SD3, "n %c\n\r", n);
		*/
		//uint8_t n = sdReadTimeout(&SD2, &info, 128, 20000); //TIME_INFINITE, TIME_IMMEDIATE
		//uint8_t f = sdRead(&SD2, &info, n);
		
		/*int32_t n = 0;
		
		n = sdRequestDataI(&SD2);
		
		if( n == MSG_TIMEOUT){
			n = "pailas esto";
		}*/
		
		/*
		flagsmask_t flags;
		EventListener s2EventListener;
		chEvtRegisterMask((EventSource *)chnGetEventSource(&SD2), &s2EventListener,EVENT_MASK(1));


		int i=0;
		while (TRUE)
		{

		  chEvtWaitOneTimeout(EVENT_MASK(1),MS2ST(10));

		  flags = chEvtGetAndClearFlags(&s2EventListener);

		  if (flags & CHN_INPUT_AVAILABLE) {
		       // /* Data available read here.
		      //sdReadTimeout(&SD4, message, sizeof(message), TIME_IMMEDIATE);

		  }

		  if (flags & CHN_OUTPUT_EMPTY) {
		        ///* Data sent, you may transmit from here.
		  }
		  if (flags & (SD_PARITY_ERROR | SD_FRAMING_ERROR | SD_OVERRUN_ERROR |
		                   SD_NOISE_ERROR | SD_BREAK_DETECTED)) {
		        ///* Some receive error happened.
		  }
		    //uartStartSend(&UARTD3,1,"b");

		  chThdSleepMilliseconds(5);
		} 
		*/
		
		sdStop(&SD2);
				
  	chprintf( (BaseChannel *)&SD3, "Count %d\n\r", contador);
//  	chSysLock();
		//osalSysLock();
  	chprintf( (BaseChannel *)&SD3, "tamano %d\n\r", n);//cast a puntero de tipo BaseChannel
  	//osalSysUnlock();
  	chprintf( (BaseChannel *)&SD3, "info %s\n\r", &info  );
		chprintf( (BaseChannel *)&SD3, "valor: %f cm\n\r", distancia);
		chprintf( (BaseChannel *)&SD3, "isOk %d\n\r", lectura);
		chprintf( (BaseChannel *)&SD3, "------ ------ ------- %x\n\r", true);
		
		chThdSleepMilliseconds(500);    
  }
  
}
