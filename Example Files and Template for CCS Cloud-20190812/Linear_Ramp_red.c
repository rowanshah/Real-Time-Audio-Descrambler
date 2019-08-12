
/*******************************************************************************
The program sets up the MSP432 to output a linear ramp.  The output is taken from
the 8 digital pins of PORT_P2 so you need to build an external DAC to
convert these digital outputs to an analogue signal and hence the linear ramp.

The master clock is set to 48 MHz and the SysTick timer is set to count to 960.
Therefore, the systick_isr is called at a rate of 48 MHz / 960 = 50 kHz.  PORT_P2
is an 8 bit I/O register which can have any integer value between 0 and 255.
The content of PORT_P2 can be assigned by writing an integer value to P2OUT.
Initial, P2OUT is set to 0.  Then every time the systick_isr is called, P2OUT is
incremented by 1 until 255 and then afterwards it will start again.  Therefore,
the ramp signal will have a frequency of 50 kHz / 256 = 195.3125 Hz.


 * Author: Dr Chin-Pang Liu, University College London, 2018
 ******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <math.h>


int main(void)
{
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();


    /* Configuring pins for HFXT crystal */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_PJ,
            GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);


    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0);


    /* Configuring GPIOs (P4.3 MCLK) */
       MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P4, GPIO_PIN3,
       GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting the external clock frequency. This API is optional, but will
     * come in handy if the user ever wants to use the getMCLK/getACLK/etc
     * functions
     */
    CS_setExternalClockSourceFrequency(32000,48000000);

    /* Starting HFXT in non-bypass mode without a timeout. Before we start
     * we have to change VCORE to 1 to support the 48MHz frequency */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    CS_startHFXT(false);

    /* Initializing MCLK to HFXT (effectively 48MHz) */
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Configuring SysTick to trigger at 960 (MCLK is 48MHz so te SysTick is called at a rate of 50 kHz) */
    MAP_SysTick_enableModule();
    MAP_SysTick_setPeriod(960);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_SysTick_enableInterrupt();



     /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();

    while (1)
    {
        //MAP_PCM_gotoLPM0();
    }
}

void SysTick_Handler(void)
{
    P6OUT |= BIT0; // set P6 PIN0 High

    static int j = 0;


    P2OUT = j;  // This is where the 8 output pins of port 2 are assigned the values of j (from 0 to 255)

    j++;
        if (j == 256)
        {j = 0;}


    P6OUT &= ~BIT0; // set P6 PIN0 Low


}


