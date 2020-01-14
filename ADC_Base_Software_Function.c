/* -------------------------------------------------------------------------------------------------------------------- */
// F28335 - Base Project
/* -------------------------------------------------------------------------------------------------------------------- */
//
// Author:      Renan R. Duarte
// Date:        27/09/19
// Device:      Piccolo F28335 Launchpad
//
/* -------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------- */
// Includes
/* -------------------------------------------------------------------------------------------------------------------- */

// Devide macros and defines - Includes constant definitions and macros used on TI examples
#include "DSP28x_Project.h"

/* -------------------------------------------------------------------------------------------------------------------- */
// Hardware definitions
/* -------------------------------------------------------------------------------------------------------------------- */


//Turn LEDs on
#define LED0_ON             GpioDataRegs.GPACLEAR.bit.GPIO31 = 1
#define LED1_ON             GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1

//Turn LEDs off
#define LED0_OFF            GpioDataRegs.GPASET.bit.GPIO31 = 1
#define LED1_OFF            GpioDataRegs.GPBSET.bit.GPIO34 = 1

//Toggle LEDs
#define LED0_TOGGLE         GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1
#define LED1_TOGGLE         GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1


/* -------------------------------------------------------------------------------------------------------------------- */
// Program definitions
/* -------------------------------------------------------------------------------------------------------------------- */


// Defines for ADC start parameters
#if (CPU_FRQ_150MHZ)
    #define ADC_MODCLK 0x1  // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*1)   = 75.0 MHz
#endif

#define ADC_CKPS   0x1   // ADC module clock = HSPCLK/2*ADC_CKPS   = 75.0MHz/(1*2) = 37.5MHz
#define ADC_SHCLK  0xf   // S/H width in ADC module periods = 16 ADC clocks
#define AVG        1000  // Average sample limit


/* -------------------------------------------------------------------------------------------------------------------- */
// Variables
/* -------------------------------------------------------------------------------------------------------------------- */


Uint16 VoltagePV = 0;
Uint16 CurrentPV = 0;
Uint16 TempPV = 0;
Uint16 Photosensor = 0;
Uint16 VoltageBAT = 0;
Uint16 CurrentBAT = 0;


/* -------------------------------------------------------------------------------------------------------------------- */
// Function prototypes
/* -------------------------------------------------------------------------------------------------------------------- */


int A (Chanel)
    {
	      while(AdcRegs.ADCTRL2.all == 0)
          {
            VoltagePV = ((AdcRegs.ADCRESULT0>>4));
            CurrentPV = ((AdcRegs.ADCRESULT1>>4));
            TempPV = ((AdcRegs.ADCRESULT2>>4));
            Photosensor = ((AdcRegs.ADCRESULT3>>4));
          }

		  AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
		  AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear SEQ1

      return VoltagePV;
      return CurrentPV;
      return TempPV;
      return Photosensor;
    }


/*int B (CurrentPV)
    {
	      while(AdcRegs.ADCTRL2.all == 0)
          {
            CurrentPV = ((AdcRegs.ADCRESULT1>>4));
          }

		  AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
		  AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear SEQ1

      return CurrentPV;
    }


int C (TempPV)
    {
	      while(AdcRegs.ADCTRL2.all == 0)
          {
            TempPV = ((AdcRegs.ADCRESULT2>>4));
          }

		  AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
		  AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear SEQ1

      return TempPV;
    }


int D (Photosensor)
    {
	      while(AdcRegs.ADCTRL2.all == 0)
          {
            Photosensor = ((AdcRegs.ADCRESULT3>>4));
          }

		  AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
		  AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear SEQ1

      return Photosensor;
    }


int E (VoltageBAT)
    {
	      while(AdcRegs.ADCTRL2.all == 0)
          {
            VoltageBAT = ((AdcRegs.ADCRESULT4>>4));
          }

		  AdcRegs.ADCTRL2.bit.RST_SEQ2 = 1;         // Reset SEQ2
		  AdcRegs.ADCST.bit.INT_SEQ2_CLR = 1;       // Clear SEQ2

      return VoltageBAT;
    }


int F (CurrentBAT)
    {
	      while(AdcRegs.ADCTRL2.all == 0)
          {
            CurrentBAT = ((AdcRegs.ADCRESULT0>>4));
          }

		  AdcRegs.ADCTRL2.bit.RST_SEQ2 = 1;         // Reset SEQ2
		  AdcRegs.ADCST.bit.INT_SEQ2_CLR = 1;       // Clear SEQ2

      return CurrentBAT;
    }

*/

/* ---------------------------------------------------------------------------------------------------------------- */
// Main loop
/* ---------------------------------------------------------------------------------------------------------------- */

void main(void)
{
    /* ---------------------------------------------------------------------------------------------------------------- */
    // Memory settings
    /* ---------------------------------------------------------------------------------------------------------------- */

    // Copy functions from flash to RAM - DO NOT REMOVE (Uses F28027.cmd instead of 28027_RAM_Ink.cmd)
    #ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
    #endif

    /* ---------------------------------------------------------------------------------------------------------------- */
    // MCU settings
    /* ---------------------------------------------------------------------------------------------------------------- */

    // Basic Core Init
    InitSysCtrl();

    // Specific clock setting for this example
    EALLOW;
    SysCtrlRegs.HISPCP.all = ADC_MODCLK;	// HSPCLK = SYSCLKOUT/ADC_MODCLK
    EDIS;

    // Disable all interrupts
    DINT;

    // Basic setup of PIE table
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags
    IER = 0x0000;
    IFR = 0x0000;

    // Set default ISR's in PIE
    InitPieVectTable ();

    //Init ADC
    InitAdc();

    // Specific ADC setup for this example:
    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;   //Tamanho da janela de aquisição --> 0xf
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;  //Divisor do relógio principal --> freq = 37.5MHz
    AdcRegs.ADCMAXCONV.all = 0x0101;          // Setup 6 conv's
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;    // Setup ADCINA0 as 1st SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;    // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;    // Setup ADCINA2 as 3rd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;    // Setup ADCINA3 as 4th SEQ1 conv.
    AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4;    // Setup ADCINA4 as 1st SEQ2 conv.
    AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x5;    // Setup ADCINA5 as 2nd SEQ2 conv.
    AdcRegs.ADCTRL1.bit.CONT_RUN = 1;         // Setup continuous run

    // Sets all pins to be muxed to GPIO in input mode with pull-ups enabled
    //InitGpio ();

    /* ---------------------------------------------------------------------------------------------------------------- */
    // GPIO configuration - LEDs
    /* ---------------------------------------------------------------------------------------------------------------- */

    EALLOW;

    // LED pins as outputs
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;

    EDIS;

    // Start SEQ1
    AdcRegs.ADCTRL2.all = 0x2000;

    // Take ADC data and log the in SampleTable array
    for(;;)
    {
    	LED0_ON;
    	LED1_ON;

        // Wait for interrupt
        while (AdcRegs.ADCTRL2.all == 0)
        {
           A(Chanel);
        }

/*        while (AdcRegs.ADCTRL2.all == 0)
        {
           B(CurrentPV);
        }

        while (AdcRegs.ADCTRL2.all == 0)
        {
           C(TempPV);
        }

        while (AdcRegs.ADCTRL2.all == 0)
        {
           D(Photosensor);
        }

        while (AdcRegs.ADCTRL2.all == 0)
        {
           E(VoltageBAT);
        }

        while (AdcRegs.ADCTRL2.all == 0)
        {
           F(CurrentBAT);
        }

*/
            LED0_OFF;
            LED1_OFF;
    }


}

/* -------------------------------------------------------------------------------------------------------------------- */
// End of file
/* -------------------------------------------------------------------------------------------------------------------- */
