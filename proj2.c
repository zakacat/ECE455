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
#include <stdlib.h> //Added for random function
#include <inttypes.h>
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"



/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 3

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

//This function receives all of the information necessary to create a
//new dd_taskstruct(excluding the release time and completion time.
static void release_dd_task_func();

//This function receives the ID of the DD-Task which has completed its execution.
//The ID is packaged as a message and sent to a queue for the DDS to receive
static void complete_dd_task_func();

//This function sends a message to a queue requesting the Active Task List from the DDS.
//Once a response is received from the DDS, the function returns the list.
static void get_active_dd_task_list();

//This function sends a message to a queue requesting the Completed Task List from the DDS.
//Once a response is received from the DDS, the function returns the list
static void get_completed_dd_task_list();

//This function sends a message to a queue requesting the Overdue Task List from the DDS.
//Once a response is received from the DDS, the function returns the list.
static void get_overdue_dd_task_list();

static void Pot_Reader_Task( void *pvParameters );
static void Traffic_Light_Controller_Task( void *pvParameters );
static void Hardware_Clock_Task( void *pvParameters );
static void Car_Generation_Task( void *pvParameters );


xQueueHandle xQueue_handle = 0;
xQueueHandle xQueue_handle2 = 0;

typedef enum {PERIODIC,APERIODIC} task_type ;//Should they all be in real time?

typedef struct dd_task { //A dd_task which will likely be food
	TaskHandle_t t_handle;
	task_type type;
	uint32_t task_id; //Can be in real time
	uint32_t release_time; //Should be in real time
	uint32_t absolute_deadline; //Should be in real time
	uint32_t completion_time; //Should be in real time
};

typedef struct dd_task_list{ // Probably this will be represented by the chef's cooking queue
	struct dd_task task;
	struct dd_task_list *next_task;
};


/*-----------------------------------------------------------*/

int main(void)
{


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
	vQueueAddToRegistry( xQueue_handle, "Potentiometer" );

	vQueueAddToRegistry( xQueue_handle2, "TrafficLight" );

	xTaskCreate( Pot_Reader_Task, "Pot_Reader", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Traffic_Light_Controller_Task, "Traffic_Light_Controller", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Hardware_Clock_Task, "Hardware_Clock", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Car_Generation_Task, "Car_Generation", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

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

	//1 means not reset
	GPIO_SetBits(GPIOC, GPIO_Pin_8);
	//Turn off the first SPC clock
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

static void Pot_Reader_Task( void *pvParameters )
{

	while(1)
	{

		printf("Ftask 1 !\n");

		vTaskDelay(2000);

	}
}

/*-----------------------------------------------------------*/

static void Traffic_Light_Controller_Task( void *pvParameters )
{



	while(1)
	{

		printf("Ftask 2 !\n");
		vTaskDelay(2000);

	}
}


/*-----------------------------------------------------------*/

static void Hardware_Clock_Task( void *pvParameters )
{

	while(1)
	{
		printf("Ftask 3 !\n");
		vTaskDelay(2000);


	}
}

/*-----------------------------------------------------------*/

static void Car_Generation_Task( void *pvParameters )
{


	while(1)
	{
		printf("Ftask 4 !\n");
		vTaskDelay(2000);
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

