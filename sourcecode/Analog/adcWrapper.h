#ifndef ADCWRAPPER_H
#define ADCWRAPPER_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "common.h"
// #include "ssi3_ADCs.h"

/*******************************\
| Defines
\*******************************/
// ADC
#define ADC_SSI_CLKRATE ssi_clkRate_1MHz // Default-Clockrate

// ADC Datastructure
#define ADC_NCHAINS 2      // Amount of ADC Daisy-Chains
#define ADC_NADCS 2        // Amount of ADCs in daisy-chain
#define ADC_CONFPACKS 1    // Amount of config-packs (DWORD!)
#define ADC_CPACK_NWORDS 2 // Conf-DWORD = 2 * 16bit
#define ADC_CHANNELS 8     // Amount of outputvoltage-packs (WORD!)
#define ADC_CH_NWORDS 1    // Each Channel = 1 * 16bit

#define ADC_POSITIVE_RANGE 10.0
#define ADC_POSITIVE_RESOLUTION 0x7FFF // signed 16bit resolution = 0x7FFF (positive half)

// Amount of WORDs per send
#define ADC_NALLCONFPACKS (ADC_NADCS * ADC_CONFPACKS)
#define ADC_NALLCHPACKS (ADC_NADCS * ADC_CHANNELS)

// Amount of WORDs for buffers
#define ADC_CPACKS_NALLWORDS (ADC_NADCS * ADC_CONFPACKS * ADC_CPACK_NWORDS)
#define ADC_CHANNELS_NALLWORDS (ADC_NADCS * ADC_CHANNELS * ADC_CH_NWORDS)
/*******************************\
| Enum/Struct/Union
\*******************************/
enum adcChain
{
    adcChain_CF,
    adcChain_UDrp,
};

enum adcVPackIndex
{
    adcCh0 = 0,
    adcCh1,
    adcCh2,
    adcCh3,
    adcCh4,
    adcCh5,
    adcCh6,
    adcCh7,
    adcCh8,
    adcCh9,
    adcCh10,
    adcCh11,
    adcCh12,
    adcCh13,
    adcCh14,
    adcCh15,
    // adcChannel_Count,
};

// enum adcStatus
// {
//     adcOk,
//     adcError,
//     adcBusy,
// }

typedef struct
{
    // Sizes and Counts
    const uint16_t nChannels;
    const uint16_t nWords;

    // Arrays
    const uint16_t zeroStream[ADC_CHANNELS_NALLWORDS];
    struct
    {
        int16_t raw[ADC_CHANNELS_NALLWORDS];
        double voltages[ADC_NALLCHPACKS];
    } chains[ADC_NCHAINS];

} adc_measurementValues_t;

/*******************************\
| Function declaration
\*******************************/
void adcs_init(void);

cBool adc_chipselect(enum adcChain chain, cBool csState);        // Nonblocking chipselect
void adc_chipselectBlocking(enum adcChain chain, cBool csState); // Blocking core until chipselect can be done

enum adcStatus adc_startConv(enum adcChain chain);
enum adcStatus adc_startConvBlocking(enum adcChain chain);

void adc_takeMeasurement(enum adcChain chain);
adc_measurementValues_t *adc_grabMeasurements(void); // Return the measurement-data structure
// ADC_Data_t *adc_grabTxDataStruct(void); // Return the Tx-Datastructure

// void adc_queryPack(enum adc_packIndex packIndex);

// void adc_setChVoltage(uint16_t channel, float voltage);

// void testvalues(void);

#endif