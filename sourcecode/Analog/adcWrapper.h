#ifndef ADCWRAPPER_H
#define ADCWRAPPER_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "common.h"
#include "ssi3_ADCs.h"

/*******************************\
| Defines
\*******************************/
// ADC
#define ADC_SSI_CLKRATE ssi_clkRate_1MHz // Default-Clockrate

// DAC Datastructure
#define ADC_NDACS 2       // Amount of DACs in daisy-chain
#define ADC_CONFPACKS 1   // Amoutn of config-packs
#define ADC_VOLTPACKS 8   // Amount of outputvoltage-packs
#define ADC_PACK_NBYTES 3 // Bytes per data-pack

#define ADC_BYTES_PER_SEND (DAC_NDACS * DAC_PACK_NBYTES)

#define ADC_NALLCONFPACKS (DAC_NDACS * DAC_CONFPACKS)
#define ADC_NALLVOLTPACKS (DAC_NDACS * DAC_VOLTPACKS)
#define ADC_NALLPACKS (DAC_NALLCONFPACKS + DAC_NALLVOLTPACKS) // Amount of all packs
#define ADC_ALLPACKS_NBYTES (DAC_NALLPACKS * DAC_PACK_NBYTES) // Amount of bytes for all data-packs. Be carefull when updating code for more than 2 daisychain-DACs. Struct not using this calculation to modify the entire struct!
/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/
void adcs_init(void);

// cBool adc_chipselect(cBool csState);        // Nonblocking chipselect
// void adc_chipselectBlocking(cBool csState); // Blocking core until chipselect can be done

// DAC_Data_t *adc_grabRxDataStruct(void); // Return the Rx-Datastructure
// DAC_Data_t *adc_grabTxDataStruct(void); // Return the Tx-Datastructure

// void adc_queryPack(enum dac_packIndex packIndex);

// void adc_setChVoltage(uint16_t channel, float voltage);

// void testvalues(void);

#endif