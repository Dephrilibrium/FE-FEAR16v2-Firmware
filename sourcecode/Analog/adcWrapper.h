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
#define ADC_NADCS 2        // Amount of ADCs in daisy-chain
#define ADC_CONFPACKS 1    // Amount of config-packs (DWORD!)
#define ADC_VOLTPACKS 8    // Amount of outputvoltage-packs (WORD!)
#define ADC_CPACK_NBYTES 4 // Bytes per conf-pack
#define ADC_VPACK_NBYTES 2 // Bytes per volt-pack

#define ADC_BYTES_PER_CSEND (ADC_NADCS * ADC_CONFPACKS * ADC_CPACK_NBYTES)
#define ADC_BYTES_PER_VREAD (ADC_NADCS * ADC_VOLTPACKS * ADC_VPACK_NBYTES)

#define ADC_NALLCONFPACKS_NBYTES (ADC_NADCS * (ADC_CONFPACKS * ADC_CPACK_NBYTES))
#define ADC_NALLVOLTPACKS_NBYTES (ADC_NADCS * ADC_VOLTPACKS)
/*******************************\
| Enum/Struct/Union
\*******************************/
enum adcChain
{
    adcChain_CF = 0,
    adcChain_UDrp,
};

// enum adc_packIndex
// {
//     adc_packs_Volt,
//     adc_packs_Conf,
// };

// typedef union
// {
//     uint32_t DataDWORD;
//     uint16_t DataWORDs;
//     struct
//     {
//         uint16_t DataLoWORD;
//         uint16_t DataHiWORD;
//     };
// } __attribute__((packed)) ADC_StructuralConfPack_t;
// // typedef union DAC_StructuralDataPack DAC_StructuralDataPack_t;

// typedef struct
// {
//     const uint8_t nVPacks;
//     const uint8_t nCPacks;
//     const uint8_t nPacks;

//     union
//     {
//         struct
//         {
//             uint16_t vPacks[ADC_VOLTPACKS];
//             ADC_StructuralConfPack_t cPacks[ADC_CONFPACKS];
//         };

//         struct
//         {
//             uint16_t Packs[ADC_NALLPACKS];
//         };
//     };
// } ADC_Data_t;

// typedef struct
// {
//     uint16_t nWords;
//     const uint16_t nSize;
//     const uint16_t nPacks;
//     enum adc_packIndex targetPack;

//     union
//     {
//         uint16_t SerializedStream[ADC_NALLVOLTPACKS_NBYTES];
//     } __attribute__((packed)); // To be able to iterate through serializedStream it has to be packed
// } adc_ssiStream_t;

/*******************************\
| Function declaration
\*******************************/
void adcs_init(void);

cBool adc_chipselect(enum adcChain chain, cBool csState);        // Nonblocking chipselect
void adc_chipselectBlocking(enum adcChain chain, cBool csState); // Blocking core until chipselect can be done

// ADC_Data_t *adc_grabRxDataStruct(void); // Return the Rx-Datastructure
// ADC_Data_t *adc_grabTxDataStruct(void); // Return the Tx-Datastructure

// void adc_queryPack(enum adc_packIndex packIndex);

// void adc_setChVoltage(uint16_t channel, float voltage);

// void testvalues(void);

#endif