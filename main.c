// Standard includes
#include <stdio.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "hw_apps_rcm.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "gpio.h"
#include "utils.h"

// Common interface includes
#include "gpio_if.h"
#include "button_if.h"
#include "pinmux.h"
#include "common.h"
#include "osi.h"
#include "uart_if.h"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
volatile uint32_t val;
#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
#define OSI_STACK_SIZE 8192
// OsiTaskHandle g_PushButtonTask; 
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS                         
//*****************************************************************************

//*****************************************************************************
// FreeRTOS User Hook Functions enabled in FreeRTOSConfig.h
//*****************************************************************************

//*****************************************************************************
//
//! \brief Application defined hook (or callback) function - assert
//!
//! \param[in]  pcFile - Pointer to the File Name
//! \param[in]  ulLine - Line Number
//!
//! \return none
//!
//*****************************************************************************
void
vAssertCalled( const char *pcFile, unsigned long ulLine )
{
    //Handle Assert here
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined idle task hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
vApplicationIdleHook( void)
{
    //Handle Idle Hook for Profiling, Power Management etc
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    //Handle Memory Allocation Errors
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook( OsiTaskHandle *pxTask,
                                   signed char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}

static void BoardInit(void) {
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs) || defined(gcc)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

void LedInit() {
	GPIO_IF_LedConfigure(LED1|LED2|LED3);
	GPIO_IF_LedOff(MCU_ALL_LED_IND);
}

void SW2_handler() {
	val++;
}

void SW3_handler() {
	val--;
}

void ButtonInit() {
	Button_IF_Init(SW2_handler,SW3_handler);
	Button_IF_EnableInterrupt(SW2);
	Button_IF_EnableInterrupt(SW3);
}

void main_loop() {

	ButtonInit();

	while(1) {
	
		if (val&1) {
			GPIO_IF_LedOn(MCU_RED_LED_GPIO);
		} else {
			GPIO_IF_LedOff(MCU_RED_LED_GPIO);
		}
		if (val &(1<<1)) {
			GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
		} else {
			GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
		}
		if (val & (1<<2)) {
			GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
		} else {
			GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
		}
	}
}


int main() {
	long lRetVal = -1;
	val=0;
	BoardInit();
	PinMuxConfig();
	LedInit();

    lRetVal = osi_TaskCreate(main_loop, 
    						 (signed char*)"main_loop", 
                                OSI_STACK_SIZE, 
                                NULL, SPAWN_TASK_PRIORITY, NULL );
    if(lRetVal < 0)
    {
    ERR_PRINT(lRetVal);
    LOOP_FOREVER();
    }

    osi_start();
	return 0;
}