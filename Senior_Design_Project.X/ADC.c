#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "IO.h"
#include "TIMER.h"
#include "DAC.h"
#include "ADC.h"

/**@def NUM_OF_ADCCHANNELS 
 * Defines the number of ADC channels used. */
#define NUM_OF_ADCCHANNELS      1
/** @def ADC_ARRAY_SIZE 
 * Defines the ADC array size. */
#define ADC_ARRAY_SIZE          5
/** @def ADC_MIDRAIL 
 * Defines the ADC mid-rail. */
#define ADC_MIDRAIL             479
/** @def ADC_MINMAG 
 * Defines the minimum magnitude ADC threashold. */
#define ADC_MINMAG              140         

/** @var isPositive 
 * Indicates if the sample is the positive or negative part of signal. */
BOOL isPositive;
/** @var peakMax 
 * Stores max values for a single peak. */
UINT16 peakMax[ADC_ARRAY_SIZE];
/** @var peakMin 
 * Stores min values for a single peak. */
UINT16 peakMin[ADC_ARRAY_SIZE];
/** @var localMax 
 * Stores max values for different peaks. */
UINT16 localMax[ADC_ARRAY_SIZE];
/** @var localMin 
 * Stores min values for different peaks. */
UINT16 localMin[ADC_ARRAY_SIZE];

void ADC_ZeroBuffer(void);

/**
 * @brief Initializes the ADC module.
 * @return Void
 */
void ADC_Init(void)
{
    /* AD1CON configurations */
    AD1CON1bits.ON = 0;             // Disables ADC
    AD1CON1bits.SIDL = 0;           // Continue in Idle Mode
    AD1CON1bits.FORM = 0b000;       // Data output, 16-bit Integer
    AD1CON1bits.SSRC = 0b111;       // Auto Conversion
    AD1CON1bits.CLRASAM = 0;        // Stop Conversion, buffer contents is overwritten
    
    /* AD2CON configurations */
    AD1CON2bits.VCFG = 0b000;       // AVDD and AVSS are the voltage references
    AD1CON2bits.OFFCAL = 0;         // Input offset calibration mode disabled
    AD1CON2bits.CSCNA = 0;          // Scans inputs disabled
    AD1CON2bits.SMPI = 0b0000;      // Interrupts at the completion of conversion for each sample/convert sequence
    AD1CON2bits.BUFM = 0;           // Buffer  is configured as one 16-Word buffer
    AD1CON2bits.ALTS = 0;           // Always use sample A input multiplexer settings
    
    /* AD3CON configurations. Samples every 256 us.*/
    AD1CON3bits.ADRC = 0;           // ADC conversion clock is PBCLK, TPB = 1/PCLK = 25 ns
    AD1CON3bits.ADCS = 0xFF;        // ADC conversion clock, TAD = 512*TPB = 12.8 us
    AD1CON3bits.SAMC = 0b10100;     // Sample Time period, 20 TADs
    
    /* AD1CH configurations */
    AD1CHSbits.CH0NA = 0;           // Channel 0 negative input is VREFL
    AD1CHSbits.CH0SA = 0x0002;      // Channel 0 positive input for Sample A is AN2
    
    /* AD1CSSL configurations */
    AD1CSSLbits.CSSL = 0x0002;      // Selects AN2 for input scan, all others are skipped
    
    AD1CON1bits.ON = 1;             // Enables ADC
    AD1CON1bits.ASAM = 1;           // Sampling begins immediately
    
    // Set up the ADC interrupt with a priority of 2
    INTEnable(INT_AD1, INT_ENABLED);
    INTSetVectorPriority(INT_ADC_VECTOR, INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_ADC_VECTOR, INT_SUB_PRIORITY_LEVEL_1);
    
    // Initializes strumming variables
    ADC_ZeroBuffer();
    isPositive = FALSE;
}

/**
 * @brief Process ADC related operations. 
 * @return Void
 */
void ADC_Process(void)
{
    
}

/**
 * @brief Reinitializes ADC buffers to the midrail.
 * @return Void
 */
void ADC_ZeroBuffer(void)
{
    int i = 0;
    for(i = 0; i < ADC_ARRAY_SIZE; i++)
    {
        peakMax[i] = ADC_MIDRAIL;
        peakMin[i] = ADC_MIDRAIL;
        localMax[i] = ADC_MIDRAIL;
        localMin[i] = ADC_MIDRAIL;
    }
}

/**
 * @brief ADC Interrupt Service Routine.
 * @details The interrupt service routine is used read the strummer sensor.
 * @return Void.
 */
void __ISR(_ADC_VECTOR, IPL2AUTO) ADCHandler(void)
{
    // Reads the ADC buffer
    UINT16 adcSample = (UINT16)ADC1BUF0;
    
    if(adcSample >= (ADC_MIDRAIL + ADC_MINMAG) || adcSample <= (ADC_MIDRAIL-ADC_MINMAG))
    {
       if(adcSample >= ADC_MIDRAIL)
        {
            isPositive = TRUE;
        }
        else
        {
            isPositive = FALSE;
        }

        // Stores the biggest sample read by magnitude
        if(isPositive && (adcSample > peakMax[0]))
        {
            peakMax[4] = peakMax[3];
            peakMax[3] = peakMax[2]; 
            peakMax[2] = peakMax[1];
            peakMax[1] = peakMax[0]; 
            peakMax[0] = adcSample;
        }
        else if(!isPositive && (adcSample < peakMin[0]))
        {
            peakMin[4] = peakMin[3];
            peakMin[3] = peakMin[2]; 
            peakMin[2] = peakMin[1];
            peakMin[1] = peakMin[0]; 
            peakMin[0] = adcSample;
        }

        // Checks if sample changes polarity
        if(!isPositive && (peakMax[0] > ADC_MIDRAIL))
        {
            // Takes the average of the local max
            localMax[4] = localMax[3];
            localMax[3] = localMax[2];
            localMax[2] = localMax[1];
            localMax[1] = localMax[0];
            localMax[0] = (peakMax[4] + peakMax[3] + peakMax[2] + peakMax[1] + peakMax[0])/5;
            UINT16 tempMax = (localMax[4] + localMax[3] + localMax[2] + localMax[1])/4;

            // Compares local maxs to determine if user has strum.
            if(localMax[0] > tempMax)
            {
                IO_scanFrets();                                 // Scans for the fret press.
                AUDIO_setNewTone(IO_getCurrentFret());          // Sets the file to be read.
                if(!TIMER3_IsON())
                {
                    TIMER3_ON(TRUE);                            // Kick starts reading the audio file process.
                }
            }
            else if((localMax[0] - ADC_MIDRAIL) < ADC_MINMAG)
            {
                if(TIMER3_IsON())
                {
                    TIMER3_ON(FALSE);                           // Stops the audio file process.
                }
            }

            ADC_ZeroBuffer();
        }
        else if(isPositive && (peakMin[0] < ADC_MIDRAIL))
        {
            localMin[4] = localMin[3];
            localMin[3] = localMin[2];
            localMin[2] = localMin[1];
            localMin[1] = localMin[0];
            localMin[0] = (peakMin[4] + peakMin[3] + peakMin[2] + peakMin[1] + peakMin[0])/5;
            UINT16 tempMin = (localMin[4] + localMin[3] + localMin[2] + localMin[1])/5;

            // Compares local mins to determine if user has strum.
            if(localMin[0] > tempMin)
            {
                IO_scanFrets();                                 // Scans for the fret press.
                AUDIO_setNewTone(IO_getCurrentFret());          // Sets the file to be read.
                if(!TIMER3_IsON())
                {
                    TIMER3_ON(TRUE);                            // Kick starts reading the audio file process.
                }
            }
            else if((ADC_MIDRAIL - localMin[0]) < ADC_MINMAG)
            {
                if(TIMER3_IsON())
                {
                    TIMER3_ON(FALSE);                           // Stops the audio file process.
                }
            }

            ADC_ZeroBuffer();                                   // Resets the mid-rails.
        } 
    }
    
    // Clear the interrupt flag
    INTClearFlag(INT_AD1);
}