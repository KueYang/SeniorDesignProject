#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "IO.h"
#include "TIMER.h"
#include "ADC.h"

/**@def NUM_OF_ADCCHANNELS Defines the number of ADC channels used. */
#define NUM_OF_ADCCHANNELS  1
#define ADC_BUF_SIZE        128

#define config1     ADC_FORMAT_INTG | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON
#define config2     ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_1 | ADC_ALT_BUF_OFF | ADC_ALT_INPUT_OFF
#define config3     ADC_CONV_CLK_INTERNAL_RC | ADC_SAMPLE_TIME_15
#define configscan  SKIP_SCAN_ALL
#define configport  ENABLE_AN2_ANA

BYTE adcData[ADC_BUF_SIZE];
int adcBufIndex;
BOOL isStrumming;

/**
 * @brief Initializes the ADC module.
 * @return Void
 */
void ADC_Init(void)
{
    /* AD1CON configurations */
    AD1CON1bits.ON = 0;         // Disables ADC
    AD1CON1bits.SIDL = 0;       // Continue in Idle Mode
    AD1CON1bits.FORM = 0b000;   // Data output, 16-bit Integer
    AD1CON1bits.SSRC = 0b111;   // Auto Conversion
    AD1CON1bits.CLRASAM = 0;    // Stop Conversion, buffer contents is overwritten
    AD1CON1bits.ASAM = 1;       // Sampling begins immediately
    AD1CON1bits.SAMP = 1;       // Sample Enable Bit, enable
    
    /* AD2CON configurations */
    AD1CON2bits.VCFG = 0b000;   // AVDD and AVSS are the voltage references
    AD1CON2bits.OFFCAL = 0;     // Input offset calibration mode disabled
    AD1CON2bits.CSCNA = 0;      // Scans inputs disabled
    AD1CON2bits.SMPI = 0b0000;  // Interrupts at the completion of conversion for each sample/convert sequence
    AD1CON2bits.BUFM = 0;       // Buffer  is configured as one 16-Word buffer
    AD1CON2bits.ALTS = 0;       // Always use sample A input multiplexer settings
    
    /* AD3CON configurations. Configured for 1000 ksps. */
    AD1CON3bits.ADRC = 0;       // ADC conversion clock is PBCLK
    AD1CON3bits.SAMC = 0b01000; // Sample Time period, 8 TADs
    AD1CON3bits.ADCS = 0x01;    // ADC conversion clock, TAD = 2*TPB = 50 ns
    
    /* AD1CH configurations */
    AD1CHSbits.CH0NA = 0;       // Channel 0 negative input is VREFL
    AD1CHSbits.CH0SA = 0x2;     // Channel 0 positive input for Sample A is AN2
    
    /* AD1CSSL configurations */
    AD1CSSLbits.CSSL = 0x2;     // Selects AN2 for input scan, all others are skipped
    
    AD1CON1bits.ON = 1;         // Enables ADC
    
//    CloseADC10();   // Ensure the ADC is off before setting the configuration
//    SetChanADC10( ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN2);
//    OpenADC10( config1, config2, config3, configport, configscan );
//    EnableADC10(); // Enable the ADC
    
    // Set up the ADC interrupt with a priority of 2
    INTEnable(INT_AD1, INT_ENABLED);
    INTSetVectorPriority(INT_ADC_VECTOR, INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_ADC_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    
    adcBufIndex = 0;
    isStrumming = FALSE;
}

/**
 * @brief Process ADC related operations. 
 * @return Void
 */
void ADC_Process(void)
{
    
}

/**
 * @brief ADC Interrupt Service Routine.
 * @details The interrupt service routine is used read the strummer sensor.
 * @return Void.
 */
void __ISR(_ADC_VECTOR, IPL2AUTO) ADCHandler(void)
{
    // Reads the ADC channel
    adcData[adcBufIndex] = ADC1BUF0;
    
    // Checks if strumming occur, strumming occurs if the voltage read is non-zero
    if(adcData[adcBufIndex++] > 10)
    {
        IO_scanFrets();                             // Scans for the fret press.
        AUDIO_setNewTone(IO_getCurrentFret());      // Sets the file to be read.
        if(!TIMER3_IsON())
        {
            TIMER3_ON(TRUE);    // Kick starts reading the audio file process.
        }
    }
    
    if(adcBufIndex >= ADC_BUF_SIZE)
    {
        adcBufIndex = 0;
    }
    
    // Clear the interrupt flag
    INTClearFlag(INT_AD1);
}