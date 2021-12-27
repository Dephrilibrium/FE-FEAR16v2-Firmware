/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "math.h"
#include "stdio.h"

/* Project specific */
#include "common.h"
#include "adcWrapper.h"
#include "preciseTime.h"
#include "swap.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Hardwaredefines

#pragma endregion Hardwaredefines

/*******************************\
| Local Enum/Struct/Union
\*******************************/

/*******************************\
| Local function declarations
\*******************************/
void adc_wipeADCData(void);
void adc_setupSequence(void);
// void adc_setCPacks(uint8_t cmdByte, uint16_t dataWord);
// void adc_setVPacks(uint8_t cmdByte, uint16_t dataWord);
// void dac_setToPacks(uint8_t cmdByte, uint16_t dataWord, DAC_StructuralDataPack_t *packCollection, uint8_t nPacks);

// enum dac_packIndex adc_IndexInt2EnumPackIndex(uint16_t iPackIndex);
// uint16_t adc_IndexEnumPackIndex2Int(enum dac_packIndex packIndex);
// void adc_prepareTxData(enum dac_packIndex packIndex);
// void adc_fetchRxData(enum dac_packIndex packIndex);

// uint16_t adc_selectVoltRange(uint16_t chRange);
// uint16_t adc_convertFloatTo16BitRange(float voltage);

/*******************************\
| Global variables
\*******************************/

/*******************************\
| Function definitons
\*******************************/
void adcs_init(void)
{
    ssi3_init(ADC_SSI_CLKRATE);
    adc_wipeADCData();
    adc_setupSequence();
}

void adc_wipeADCData(void)
{
    //       /* Wipe structure */
    // #pragma GCC diagnostic push
    // #pragma GCC diagnostic ignored "-Waddress-of-packed-member"

    // adc_setCPacks(0, 0);
    // adc_setVPacks(0, 0);
    // #pragma GCC diagnostic pop
}

void adc_setupSequence(void)
{
}
