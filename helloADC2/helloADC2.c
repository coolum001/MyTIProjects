//*****************************************************************************
//
// hellotempadc.c - Read Temperature via ADC, report to UART.
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
#include "driverlib/adc.h"

int sprintf ( char * str, const char * format, ... );
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

// **************************************************
//
// Configure ADC
//
// **************************************************

void ConfigureADC(void){
	
	// enable the first ADC
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	
	//  For safety, delay after Peripheral enable
	//  may not be necessary for ADC peripheral (?)
	SysCtlDelay(3);
	
	// select sample sequencer 3.  Seq 3 Collects single sample
	// priority set to 0 (no other ADC sequencing being done
	// sequencer set to be triggered by Processor
	
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	
	// the Sequence 3 has one step, configured here to be
	// read temp sensor, set Interrupt Flag at end of sampling, end sampling 
	
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);

	
	// Sequence 3 now configured, so enable
	ADCSequenceEnable(ADC0_BASE, 3);
	
	//
	// Clear the interrupt status flag.  This is done to make sure the
	// interrupt flag is cleared before we sample.
	//
	ADCIntClear(ADC0_BASE, 3);
	
	// oversample to reduce noise / jitter
	
	uint32_t OVERSAMPLE = 64;
	ADCHardwareOversampleConfigure(ADC0_BASE, OVERSAMPLE);

	
	// Configuration of ADC complete
	
}; // end ConfigureADC
	
	
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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	

    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

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
    // This array is used for storing the data read from the ADC FIFO. It
    // must be as large as the FIFO for the sequencer in use.  This example
    // uses sequence 3 which has a FIFO depth of 1.  If another sequence
    // was used with a deeper FIFO, then the array size must be changed.
    //
    uint32_t pui32ADC0Value[1];

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
		// Main Osc using 16MHz crystal driving Phase Lock Loop
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
    // Enable the GPIO pins for the LED (PF2 ).
    //
    // MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
		
		// Enable the GPIO Port for analog input
		
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
		// delay for good practise after peripheral enable, 
		//  may not be necessary for GPIO peripheral
		SysCtlDelay(3);		
    //
		//
    // Select the analog ADC function for Pin on ADC Ch 0.
    // Consult the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);



    // Initialize the UART.
		// System Clock is now defined, so OK to set up UART
    //
    ConfigureUART();
		
		ConfigureADC();

    //
    // Hello!
    //
    UARTprintf("HelloADC2, world!\n");
		



    //
    // Report the Clock Frequency
		
		char clockfreqstr[50]; 
    // get the system clock frequency 
    sprintf(clockfreqstr, "%d", SysCtlClockGet() ); 
		
		UARTprintf("Current Clock Speed (Hz): ");
		UARTprintf(clockfreqstr);
		UARTprintf("\n");
    //
		// Do ADC, blink LED in for-ever loop
		
    while(1)
    {

        //
        // Trigger the ADC conversion, on Sequencer 3 (single reading sequencer)
        //
        ADCProcessorTrigger(ADC0_BASE, 3);

        //
        // Wait for conversion to be completed.
        //
        while(!ADCIntStatus(ADC0_BASE, 3, false))
        {
        };//end while

        //
        // Clear the ADC interrupt flag.
        //
        ADCIntClear(ADC0_BASE, 3);

        //
        // Read ADC Value.
        //
        ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);
				
        //

				 //
        // Display the adc value on the console.
        //
        UARTprintf("ADC Value = %4d \n", pui32ADC0Value[0]);


				
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
