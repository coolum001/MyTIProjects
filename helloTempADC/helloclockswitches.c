//*****************************************************************************
//
// helloclock.c - Simple hello world example.
//
// Modification of code from Texas Instruments Incorporated.  
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Hello World (hello)</h1>
//!
//! A very simple ``hello world'' example.  It simply displays ``Hello World!''
//! on the UART and is a starting point for more complicated applications.
//!
//! UART0, connected to the Virtual Serial Port and running at
//! 115,200, 8-N-1, is used to display messages from this application.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//
// Print "Hello World!" to the UART on the evaluation board.
//
//*****************************************************************************
int
main(void)
{
    //volatile uint32_t ui32Loop;

    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    MAP_FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal.
    //
	// Specify System Clock to be:
	// Main Osc using 16MHz crystal driving Phae Lock Loop
	// PPL is 400MHz, divided by 2 by system, divided  by 2.5 by config below
		// PPL 400MHz -> 200 MHz -> 80 MHz

    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);
		// Now system clock is defined and enabled, we can enable peripherals
    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
		// delay for good practise after peripheral enable, 
		//  may not be necessary for GPIO peripheral
		SysCtlDelay(3);

    //
    // Enable the GPIO pins for the LED (PF2 & PF3).
    //
    // MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
		
		// Enable the SW1 pin as input
		GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4); 
		GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //
    // Initialize the UART.
		// System Clock is now defined, so OK to set up UART
    //
    ConfigureUART();

    //
    // Hello!
    //
    UARTprintf("HelloClock, world!\n");

    //
    // Report the Clock Frequency
		
		char clockfreqstr[50]; 
    // get the system clock frequency 
    sprintf(clockfreqstr, "%d", SysCtlClockGet() ); 
		
		UARTprintf("Current Clock Speed (Hz): ");
		UARTprintf(clockfreqstr);
		UARTprintf("\n");
    //
		// blink LED in all-done loop
		uint32_t value; // will hold button state =0 button down
    while(1)
    {
				// Read Button 1
				
				value = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
			
				if( value == 0){
					UARTprintf("Button down (==0) \n");
				} else {
					UARTprintf("Button up (!=0) \n");
				}; // end if
				
        //
        // Turn on the BLUE LED.
        //
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

        //
        // Delay for a bit.
        //
        SysCtlDelay(SysCtlClockGet() / 2 / 3);

        //
        // Turn off the BLUE LED.
        //
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);

        //
        // Delay for a bit.
        //
        SysCtlDelay(SysCtlClockGet() / 2 / 3);
    }
}
