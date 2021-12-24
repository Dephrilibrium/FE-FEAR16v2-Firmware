#ifndef ADCWRAPPER_H
#define ADCWRAPPER_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */

/*******************************\
| Defines
\*******************************/
// DAC
#define DAC_SSI_CLKRATE ssi0_clkRate_1MHz // Default-Clockrate

// DAC Datastructure per daisy-chain
#define ADC_ADCS 2        // Amount of DACs in daisy-chain
#define ADC_CPACKS 1      // Amoutn of config-packs
#define ADC_VPACKS 8      // Amount of outputvoltage-packs
#define ADC_PACK_NBYTES 4 // Bytes per data-pack

#define DAC_BYTES_PER_SEND (ADC_ADCS * ADC_PACK_NBYTES)

#define DAC_NALLCONFPACKS (ADC_ADCS * ADC_CPACKS)
#define DAC_NALLVOLTPACKS (ADC_ADCS * ADC_VPACKS)
#define DAC_NALLPACKS (DAC_NALLCONFPACKS + DAC_NALLVOLTPACKS) // Amount of all packs
#define DAC_ALLPACKS_NBYTES (DAC_NALLPACKS * ADC_PACK_NBYTES) // Amount of bytes for all data-packs. Be carefull when updating code for more than 2 daisychain-DACs. Struct not using this calculation to modify the entire struct!
/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/

#endif