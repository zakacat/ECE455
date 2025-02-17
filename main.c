/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
#include <stdlib.h>
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"



/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 100

#define amber  	0
#define green  	1
#define red  	2
#define blue  	3

#define amber_led	LED3
#define green_led	LED4
#define red_led		LED5
#define blue_led	LED6


/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );

/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */
//Set up the GPIO pins for the LEDs
static void Setup_GPIO_Traffic();

static void Setup_GPIO_Pot();
//Serial to Parallel Converter
static void Setup_SPC();
//Set up the ADC peripheral for potentiometer
static void Setup_ADC();

static void Manager_Task( void *pvParameters );
static void Blue_LED_Controller_Task( void *pvParameters );
static void Green_LED_Controller_Task( void *pvParameters );
static void Red_LED_Controller_Task( void *pvParameters );
static void Amber_LED_Controller_Task( void *pvParameters );

//Our tasks to be defined

//This task will be reading the potentiometer
//static void Traffic_Flow_Adjustment_Task( void *pvParameters );
//This is for the traffic
//static void Traffic_Generator_Task( void *pvParameters );
//This is for the traffic light
//static void Traffic_Light_State_Task( void *pvParameters );
//I think that this task may be redundant
//static void System_Display_Task( void *pvParameters );

xQueueHandle xQueue_handle = 0;
xQueueHandle xQueue_handle2 = 0;


/*-----------------------------------------------------------*/

int main(void)
{

	/* Initialize LEDs */
    STM_EVAL_LEDInit(amber_led);
	STM_EVAL_LEDInit(green_led);
	STM_EVAL_LEDInit(red_led);
	STM_EVAL_LEDInit(blue_led);

	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */
	prvSetupHardware();


	/* Create the queue used by the queue send and queue receive tasks.
	http://www.freertos.org/a00116.html */
	xQueue_handle = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
							sizeof( uint16_t ) );	/* The size of each item the queue holds. */

	xQueue_handle2 = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
								sizeof( char ) );	/* The size of each item the queue holds. */

	/* Add to the registry, for the benefit of kernel aware debugging. */
	vQueueAddToRegistry( xQueue_handle, "MainQueue" );

	vQueueAddToRegistry( xQueue_handle2, "TrafficLight" );

	xTaskCreate( Manager_Task, "Manager", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate( Blue_LED_Controller_Task, "Blue_LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Red_LED_Controller_Task, "Red_LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Green_LED_Controller_Task, "Green_LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Amber_LED_Controller_Task, "Amber_LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);


	Setup_GPIO_Traffic();

	Setup_GPIO_Pot();

	Setup_ADC();

	Setup_SPC();
	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}

//Set up the GPIO pins for the LEDs
static void Setup_GPIO_Traffic(){

	printf("Setup_GPIO_Traffic enter\n");

	//Enable the RCC peripheral clock port C GPIOs
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);

	//Create intialization struct
	GPIO_InitTypeDef GPIO_InitStruct;

	//Configure Pin 0 = RED Light, PIN 1 = YELLER, PIN 2 = GREEN
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 ;
	//Set Mode to output
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	//Set to Push-Pull
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//Set speed to slowest speed to avoid noise
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	//Set to no pull
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	//Initialize the port
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	printf("Setup_GPIO_Traffic exit\n");

}

static void Setup_GPIO_Pot(){

	printf("Setup_GPIO_Pot enter\n");
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);

	//Create intialization struct
	GPIO_InitTypeDef GPIO_InitStruct;
	//Isolate Pin C3
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	//Set Mode to analog in
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	//Set to no pull
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	//Initialize the port
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	printf("Setup_GPIO_Pot exit\n");

}
//Serial to Parallel Converter
static void Setup_SPC(){

	printf("Setup_SPC enter\n");
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);

	//Create intialization struct
	GPIO_InitTypeDef GPIO_InitStruct;

	//Isolate Pin C8 = shift reset, C7 = shift clock, C6 = shift data, C4 = 2nd shift clock
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7| GPIO_Pin_6 | GPIO_Pin_4;
	//Set Mode to output
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	//Set to Push-Pull
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//Set speed to slowest speed to avoid noise
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	//Set to no pull
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;



	//Initialize the port
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_SetBits(GPIOC, GPIO_Pin_8);
	//GPIO_SetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	printf("Setup_SPC exit\n");

}

//Set up the ADC peripheral for potentiometer
static void Setup_ADC(){

	printf("Setup_ADC enter\n");

	//Enable the RCC ADC 1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_ADC1EN, ENABLE);

	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;

	//I don't think that we care about the data alignment as long as it is consistent
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;

	ADC_InitStruct.ADC_ExternalTrigConv = DISABLE;

	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;

	ADC_InitStruct.ADC_NbrOfConversion = 1;
	//I don't think we care
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;

	ADC_InitStruct.ADC_ScanConvMode = DISABLE;

	ADC_Init(ADC1, &ADC_InitStruct);

	//Set the ADC command
	ADC_Cmd(ADC1, ENABLE);

	//Config the channel
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_84Cycles);




	printf("Setup_ADC exit\n");
}


/*-----------------------------------------------------------*/

static void Manager_Task( void *pvParameters )
{
	//uint16_t tx_data = amber;


	while(1)
	{

		ADC_SoftwareStartConv(ADC1);

		uint16_t pot = 0 ;

		if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)){


			pot = ADC_GetConversionValue(ADC1);

			printf("This is the pot value:  %u \n", pot);

		}

		if(!xQueueReceive(xQueue_handle, &pot, 0)){
			if( xQueueSend(xQueue_handle,&pot,1000))
			{
				printf("Manager: %u ON!\n", pot);
				vTaskDelay(200);
			}
			else
			{
				printf("Manager Failed!\n");
			}
		}


	}
}

/*-----------------------------------------------------------*/

static void Blue_LED_Controller_Task( void *pvParameters )
{
	uint16_t pot2;

	uint16_t yellow = 3000; // All lights start with the same duration
	uint16_t red2 = 3000;
	uint16_t green2 = 3000;

	char curr = 'g'; // Light starts on green

	GPIO_ResetBits(GPIOC, GPIO_Pin_8);
	vTaskDelay(50);
	GPIO_SetBits(GPIOC, GPIO_Pin_8);


	while(1)
	{

		//check the pot
		if(xQueueReceive(xQueue_handle, &pot2, 500))
		{

			printf("Blue task.\n");

			if(pot2 < 1100){ // if pot is less than 1100, then red is longer, and green is shorter
				red2 = 2000;
				green2 = 4000;
			}
			else if (pot2 > 2200){ // If pot is more than 2200, then red is shorter, and green is longer

				red2 = 4000;
				green2 = 2000;

			}
			else //else, then red and green and are the same as yellow
			{
				red2 = 3000;
				green2 = 3000;
			}
		}

		if(curr == 'g'){//if light is green
			GPIO_ResetBits(GPIOC, GPIO_Pin_0);
			GPIO_ResetBits(GPIOC, GPIO_Pin_1);

			GPIO_SetBits(GPIOC, GPIO_Pin_2);

			curr = 'r';

			//printf("green duration %u .\n" , green);
			//printf("pot %u  .\n" , pot2);

			vTaskDelay(green2);

		}
		else if (curr == 'r'){// or if red
			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
			GPIO_ResetBits(GPIOC, GPIO_Pin_1);

			GPIO_SetBits(GPIOC, GPIO_Pin_0);

			curr = 'y';

			vTaskDelay(red2);

		}
		else { // must be yellow
			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
			GPIO_ResetBits(GPIOC, GPIO_Pin_0);

			GPIO_SetBits(GPIOC, GPIO_Pin_1);

			curr = 'g';

			vTaskDelay(yellow);

		}

		if( xQueueSend(xQueue_handle2,&curr,1000))
		{
			printf("traffic sent: %c ON!\n", curr);
			vTaskDelay(200);
		}
		else
		{
			printf("traffic Failed!\n");
		}


	}
}


/*-----------------------------------------------------------*/

static void Green_LED_Controller_Task( void *pvParameters )
{
//	uint16_t rx_data;
	while(1)
	{

			GPIO_ResetBits(GPIOC, GPIO_Pin_4);
			vTaskDelay(400);
			GPIO_SetBits(GPIOC, GPIO_Pin_4);
			vTaskDelay(400);


	}
}

/*-----------------------------------------------------------*/

static void Red_LED_Controller_Task( void *pvParameters )
{

	char curr2;

	int num = 10; //Random max

	int val;

	int arr[8] = {0,0,0,0,0,0,0,0};

	int carArrive = 0; //Whether a car will arrive next second

	uint16_t pot2;

	int wait; //how many cars to wait

	int count = 0;


	while(1)
	{
		//Here we will want to read the traffic light value then adjust the led array accordingly.

		//check the pot
		if(xQueueReceive(xQueue_handle2, &curr2, 500))
		{
			printf("traffic received: %c ON!\n", curr2);

		}

		//check the pot
		if(xQueueReceive(xQueue_handle, &pot2, 0)){

			if(pot2 < 1100){ // if pot is less than 1100, then red is longer, and green is shorter

				wait = 0;
			}
			else if (pot2 > 2200){ // If pot is more than 2200, then red is shorter, and green is longer

				wait = 5;

			}
			else //else, then red and green and are the same as yellow
			{
				wait = 3;
			}

		}

		count ++;

		val = rand()%num;

		printf("VALUE: %d ON!\n", val);
		//fastest car generation setting.
		if(val > 2 && count > wait){
			carArrive = 1;
			count = 0;
		}
		else{
			carArrive = 0;
		}

		//SHIFT LOGIC

		if(curr2 == 'g'){
		//If the light is green.
			for(int i = 7; i >= 0; i--){

				if(i <= 6 && i > 0){

					if(arr[i] == 1){
						arr[i+1] = 1;
					}

					arr[i] = arr[i-1];
				}
				else if (i > 6){

					arr[i] = arr[i-1];
				}

				else if(i == 0){
					arr[i] = carArrive;
				}
				//vTaskDelay(500);
			}
		}
		else{
		//If the light is red or yellow.
			for(int i = 7; i >= 0; i--){

				if(i <= 6 && i > 0){

					if(arr[i] == 1){
						//do nothing
					}
					else{
						arr[i] = arr[i-1];
						arr[i-1] = 0;
					}
				}
				else if (i > 6){

					if(arr[i] == 1){
						//do nothing
					}
					else{
						arr[i] = arr[i-1];
					}

				}

				else if(i == 0){

					if(arr[1] + arr[2] + arr[3] + arr[4] + arr[5] + arr[6] + arr[7]  == 7){
						if(arr[0] == 1){
							//donothing
						}
						else{
							arr[0] = carArrive;
						}
					}
					else{
						arr[0] = carArrive;
					}

				}
				//vTaskDelay(500);
			}
		}



		for(int i = 7; i >= 0; i--){

			if(arr[i] == 1){
				GPIO_SetBits(GPIOC, GPIO_Pin_6);
			}
			else{
				GPIO_ResetBits(GPIOC, GPIO_Pin_6);
			}

				//vTaskDelay(5);
				GPIO_SetBits(GPIOC, GPIO_Pin_7);
				//vTaskDelay(5);
				GPIO_ResetBits(GPIOC, GPIO_Pin_7);

			printf("arr value:  %d \n", arr[i]);

		}
		vTaskDelay(500);
	}
}


/*-----------------------------------------------------------*/

static void Amber_LED_Controller_Task( void *pvParameters )
{

//	uint16_t rx_data;
	while(1)
	{
//		if(xQueueReceive(xQueue_handle, &rx_data, 500))
//		{
//			if(rx_data == amber)
//			{
		vTaskDelay(250);
//				STM_EVAL_LEDOff(amber_led);
//				printf("Amber Off.\n");
//			}
//			else
//			{
//				if( xQueueSend(xQueue_handle,&rx_data,1000))
//					{
//						printf("AmberTask GRP (%u).\n", rx_data); // Got wrong Package
//						vTaskDelay(500);
//					}
//			}
//		}
	}
}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

