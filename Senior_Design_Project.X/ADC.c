/**
 * @file ADC.c
 * @author Kue Yang
 * @date 2/27/2017
 * @details The ADC module will handle reading strumming sensor data and 
 * kick start the audio playback process.
 */

#include <p32xxxx.h>
#include "plib/plib.h"
#include "HardwareProfile.h"
#include "STDDEF.h"
#include "IO.h"
#include "TIMER.h"
#include "ADC.h"

/**@def NUM_OF_ADCCHANNELS 
 * Defines the number of ADC channels used. */
#define NUM_OF_ADCCHANNELS      1
/** @def ADC_ARRAY_SIZE 
 * Defines the ADC local array size. */
#define ADC_ARRAY_SIZE          5
/** @def ADC_MIDRAIL 
 * Defines the ADC mid-rail. */
#define ADC_MIDRAIL             512
/** @def ADC_NOISEMAG 
 * Defines the minimum noise magnitude ADC thread hold. */
#define ADC_NOISEMAG            150         
/** @def ADC_MINDELTA 
 * Defines the minimum change in ADC sample for indicating strum. */
#define ADC_MINDELTA            10  
/** @def ADC_MINSAMPLE 
 * Defines the minimum sample count for strum detection. */
#define ADC_MINSAMPLE           256

/** @var isPositive 
 * Indicates if the sample is the positive or negative part of signal. */
BOOL isPositive;
/** @var peakMax 
 * Stores max values for a single peak. */
UINT16 peakMax[ADC_ARRAY_SIZE];
/** @var localMax 
 * Stores max values for different peaks. */
UINT16 localMax[ADC_ARRAY_SIZE];
/** @var sampleCount 
 * Counts the number of adc samples since playing a tone. */
UINT32 sampleCount;
/** @var startStrumDetection 
 * Boolean used to enable the strum detection. */
BOOL startStrumDetection;

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
    
    /* AD3CON configurations. Samples every 51.2 us.*/
    AD1CON3bits.ADRC = 0;           // ADC conversion clock is PBCLK, TPB = 1/PCLK = 25 ns
    AD1CON3bits.ADCS = 0xFF;        // ADC conversion clock, TAD = 512*TPB = 12.8 us
    AD1CON3bits.SAMC = 0b00100;     // Sample Time period, 4 TADs
    
    /* AD1CH configurations */
    AD1CHSbits.CH0NA = 0;           // Channel 0 negative input is VREFL
    AD1CHSbits.CH0SA = 0x001C;      // Channel 0 positive input for Sample A is AN28
    
    /* AD1CSSL configurations */
    AD1CSSLbits.CSSL = 0x001C;      // Selects AN28 for input scan, all others are skipped
    
    AD1CON1bits.ON = 1;             // Enables ADC
    AD1CON1bits.ASAM = 1;           // Sampling begins immediately
    
    // Set up the ADC interrupt with a priority of 2
    IFS0bits.AD1IF = 0;
    IEC0bits.AD1IE = 1;
    IPC5bits.AD1IP = 2;
    IPC5bits.AD1IS = 3;
    
    // Initializes strumming variables
    ADC_ZeroBuffer();
    isPositive = FALSE;
    sampleCount = 0;
    startStrumDetection = TRUE;
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
        localMax[i] = ADC_MIDRAIL;
    }
}

/**
 * @brief ADC Interrupt Service Routine.
 * @details The interrupt service routine is used read the strummer sensor.
 * @return Void.
 */
void __ISR(_ADC_VECTOR, IPL2AUTO) ADCHandler(void)
{
    CLEAR_WATCHDOG_TIMER;
            
    // Reads the ADC buffer
    UINT16 adcSample = (UINT16)ADC1BUF0;
    sampleCount++;
    
    if(adcSample >= (ADC_MIDRAIL+ADC_NOISEMAG) || adcSample <= (ADC_MIDRAIL-ADC_NOISEMAG))
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

        // Checks if sample changes polarity
        if(!isPositive && (peakMax[0] > ADC_MIDRAIL))
        {
            // Takes the average of the local max
            localMax[4] = localMax[3];
            localMax[3] = localMax[2];
            localMax[2] = localMax[1];
            localMax[1] = localMax[0];
            localMax[0] = 2*(peakMax[4] + peakMax[3] + peakMax[2] + peakMax[1] + peakMax[0])/10;
            UINT16 tempMax = (localMax[4] + localMax[3] + localMax[2] + localMax[1])/4;

            // Compares local maxs to determine if user has strum.
            if((localMax[0] > (tempMax+ADC_MINDELTA)) && startStrumDetection)
            {
                AUDIO_setNewTone(IO_scanFrets(), localMax[0]);                // Sets the file to be read.
                if(!TIMER3_IsON())
                {
                    TIMER3_ON(TRUE);                            // Kick starts reading the audio file process.
                    MON_SendString("ADC: Turning on timer.");
                }
                sampleCount = 0;
                startStrumDetection = FALSE;
            }

            ADC_ZeroBuffer();
        }

        if(sampleCount >= ADC_MINSAMPLE)
        {
            startStrumDetection = TRUE;
        }
    }
    
    // Resets the sample counter if overflows
    if(sampleCount >= INT32_MAX_NUM)
    {
        sampleCount = 0;
    }
    
    // Clear the interrupt flag
    IFS0bits.AD1IF = 0;
    
    CLEAR_WATCHDOG_TIMER;
}