#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "ADC.h"

/* ADC Configurations 1:
 * Turn module on 
 * Ouput in integer format 
 * Trigger mode auto 
 * Enable auto sample 
 * A/D data format signed fractional DsPic compatable */
#define config1     ADC_FORMAT_INTG | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON | ADC_FORMAT_SIGN_FRACT

/* ADC Configurations 2:
 * ADC ref Vref+ and Vref- 
 * Disable offset test 
 * Disable scan mode
 * Perform 2 samples 
 * Use one 16 WORD buffer 
 * Use only MUXA */
#define config2     ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_16 | ADC_BUF_16 | ADC_ALT_INPUT_OFF

/* ADC Configurations 3: 
 * Use ADC internal clock
 * Set sample time 16 TADs */
#define config3     ADC_CONV_CLK_INTERNAL_RC | ADC_SAMPLE_TIME_16

/* AN0 = Temperature Sensor */
#define configport  ENABLE_AN0_ANA

/* Do not assign channels to scan */
#define configscan  SKIP_SCAN_ALL

/* Defines the number of ADC channels */
#define NUM_OF_ADCCHANNELS 13

void ADC_Init(void)
{
    // Ensure the ADC is off before setting the configuration
    CloseADC10();
    
    // Configure ADC using the parameters defined above
    OpenADC10( config1, config2, config3, configport, configscan );
    
    // Use ground as neg ref for A | use AN0 for input A
    SetChanADC10( ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN0 );

    // Enable the ADC
    EnableADC10(); 
}

void ADC_Process(void)
{
    
}

