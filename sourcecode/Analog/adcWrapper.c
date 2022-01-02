/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "math.h"
#include "stdio.h"

/* Project specific */
#include "common.h"
#include "adcWrapper.h"
#include "ssi3_ADCs.h"
#include "preciseTime.h"
#include "swap.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Hardwaredefines
// Reference settings
#define ADC_CONF_REDAC(CODE) OPTION(CODE, 0) // VRef = (Range * (CODE + 1)) / 1024
#define ADC_CONF_VREF_3V BIT(13)             // '0' = 2,5V; '1' = 3V
#define ADC_CONF_REFBUF_OFF BIT(14)          // '1' = Disable internal VRef-Buffers
#define ADC_CONF_REF_EN BIT(15)              // '1' = Enable internal VRef-Source

// Channels
#define ADC_CONF_PD_D BIT(18)    // '1' = Power down channel - pair
#define ADC_CONF_RANGE_D BIT(19) // '0' = Range: 4*VREF; '1' = Range: 2*VREF
#define ADC_CONF_PD_C BIT(20)    // '1' = Power down channel-pair
#define ADC_CONF_RANGE_C BIT(21) // '0' = Range: 4*VREF; '1' = Range: 2*VREF
#define ADC_CONF_PD_B BIT(22)    // '1' = Power down channel-pair
#define ADC_CONF_RANGE_B BIT(23) // '0' = Range: 4*VREF; '1' = Range: 2*VREF
#define ADC_CONF_RANGE_A BIT(24) // '0' = Range: 4*VREF; '1' = Range: 2*VREF

// Status, Pins and Access
#define ADC_CONF_NSTBY BIT(25)       // '1' = Power down device completely
#define ADC_CONF_BUSYPOL BIT(26)     // '0' = HI-active; '1' = LO-active
#define ADC_CONF_BUSY_OR_INT BIT(27) // '0' = Busy-status; '1' = Interrupt-signal
#define ADC_CONF_CLKOUT_EN BIT(28)   // '1' = Internal ConvCLK is available on Pin 34
#define ADC_CONF_CLKSEL BIT(29)      // '0' = Internal ConvCLK; '1' = External ConvCLK
#define ADC_CONF_READ_EN BIT(30)     // '1' ConfigRegister is clocked out (32bit)
#define ADC_CONF_WRITE_EN BIT(31)    // '1' to enable update of ConfigRegister
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
// void adc_setToPacks(uint8_t cmdByte, uint16_t dataWord, ADC_StructuralDataPack_t *packCollection, uint8_t nPacks);

// enum adc_packIndex adc_IndexInt2EnumPackIndex(uint16_t iPackIndex);
// uint16_t adc_IndexEnumPackIndex2Int(enum adc_packIndex packIndex);
// void adc_prepareTxData(enum adc_packIndex packIndex);
// void adc_fetchRxData(enum adc_packIndex packIndex);

// uint16_t adc_selectVoltRange(uint16_t chRange);
// uint16_t adc_convertFloatTo16BitRange(float voltage);

/*******************************\
| Global variables
\*******************************/
// ADC_Data_t _ADC_DataOut = {.nCPacks = ADC_NALLCONFPACKS, .nVPacks = ADC_NALLVOLTPACKS, .nPacks = ADC_NALLPACKS};
// ADC_Data_t _ADC_DataIn = {.nCPacks = ADC_NALLCONFPACKS, .nVPacks = ADC_NALLVOLTPACKS, .nPacks = ADC_NALLPACKS};

/*******************************\
| Function definitons
\*******************************/
void adcs_init(void)
{
    ssi3_init(ADC_SSI_CLKRATE);
    // adc_wipeADCData();
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
    static uint32_t basicConfigDWORD = 0;
    // basicConfigDWORD = ADC_CONF_REDAC(1023)                // VRef = 2.5V
    //                                                        //   | ADC_CONF_VREF_3V                                  // Do not extend internal Reference
    //                                                        //   | ADC_CONF_REFBUF // Disable internal VRef Buffer
    //                    | ADC_CONF_REF_EN                   // Enable internal VRef
    //                    | ADC_CONF_PD_D                     // Powerdown Channels
    //                                                        //   | ADC_CONF_RANGE_D                  // Range 2 * VRef
    //                    | ADC_CONF_PD_C                     // Powerdown Channels
    //                                                        //   | ADC_CONF_RANGE_C                  // Range 2 * VRef
    //                    | ADC_CONF_PD_B                     // Powerdown Channels
    //                                                        //   | ADC_CONF_RANGE_B                  // Range 2 * VRef
    //                    | ADC_CONF_RANGE_A | ADC_CONF_NSTBY // Powerdown Device
    //                                                        //   | ADC_CONF_BUSYPOL                  // Busy-signal: lo-active
    //                                                        //   | ADC_CONF_BUSY_OR_INT              // Busy as interrupt
    //                                                        //   | ADC_CONF_CLKOUT_EN                // Enable clock-out (Pin 34)
    //                                                        //   | ADC_CONF_CLKSEL                   // Use external conversion clock
    //                                                        //   | ADC_CONF_READ_EN                  // Read config-register in the next 2 cycles!
    //                                                        //   | ADC_CONF_WRITE_EN                // Update register contents
    //     ;

    basicConfigDWORD =                       // ADC_CONF_REDAC(1023); // VRef = 2.5V
                                             // basicConfigDWORD |= ADC_CONF_VREF_3V;    // Do not extend internal Reference
                                             // basicConfigDWORD |= ADC_CONF_REFBUF_OFF; // Disable internal VRef Buffer
        basicConfigDWORD |= ADC_CONF_REF_EN; // Enable internal VRef
    // basicConfigDWORD |= ADC_CONF_PD_D;       // Powerdown Channels
    // basicConfigDWORD |= ADC_CONF_RANGE_D;    // Range 2 * VRef
    // basicConfigDWORD |= ADC_CONF_PD_C;       // Powerdown Channels
    // basicConfigDWORD |= ADC_CONF_RANGE_C;    // Range 2 * VRef
    // basicConfigDWORD |= ADC_CONF_PD_B;       // Powerdown Channels
    // basicConfigDWORD |= ADC_CONF_RANGE_B;    // Range 2 * VRef
    // basicConfigDWORD |= ADC_CONF_RANGE_A;     // Range 2 * VRef
    // basicConfigDWORD |= ADC_CONF_NSTBY;       // Powerdown Device
    // basicConfigDWORD |= ADC_CONF_BUSYPOL;     // Busy-signal: lo-active
    // basicConfigDWORD |= ADC_CONF_BUSY_OR_INT; // Use as Interrupt-signal
    // basicConfigDWORD |= ADC_CONF_CLKOUT_EN; // Enable clock-out (Pin 34)
    // basicConfigDWORD |= ADC_CONF_CLKSEL;    // Use external conversion clock
    // basicConfigDWORD |= ADC_CONF_READ_EN;  // Read config-register in the next 2 cycles!
    // basicConfigDWORD |= ADC_CONF_WRITE_EN; // Update register contents

    pTime_wait(10000); // Wait 10ms
    while (ssi3_ADCsBusy(adcChain_CF || ssi3_ADCsBusy(adcChain_UDrp)))
        ; // To be sure ADCs POR is finished, wait for ADC-chains ready

    // Prepare
    uint32_t sendConfig;
    uint16_t *pSendConfig = (uint16_t *)&sendConfig;
    int16_t blank[16] = {0};
    int16_t response[16] = {0};
    double fac = 10.0 / 0x7FFF;
    double vVals[16][20] = {0};
    uint16_t wordCnt = 0;

    sendConfig = basicConfigDWORD | ADC_CONF_REDAC(1023) | ADC_CONF_WRITE_EN; //| ADC_CONF_READ_EN;
    adc_chipselectBlocking(adcChain_CF, bOn);
    ssi_transmit16Bit(SSI3, pSendConfig, 2);
    adc_chipselectBlocking(adcChain_CF, bOff);
    ssi_clearRxFIFO(SSI3);

    pTime_wait(5);

    // sendConfig = basicConfigDWORD | ADC_CONF_WRITE_EN | ADC_CONF_READ_EN;
    for (int mVal = 0; mVal < 20; mVal++)
    {
        ssi3_convADCs(adcChain_CF, bTrue);
        ssi3_convADCs(adcChain_CF, bFalse);
        while (ssi3_ADCsBusy(adcChain_CF))
            ; // To be sure ADCs POR is finished, wait for ADC-chains ready

        adc_chipselectBlocking(adcChain_CF, bOn);
        ssi_transmit16Bit(SSI3, (uint16_t *)&blank, 8);
        adc_chipselectBlocking(adcChain_CF, bOff);
        ssi_receive16Bit(SSI3, (uint16_t *)&response, &wordCnt, 8);
        ssi_clearRxFIFO(SSI3);

        for (int cVal = 0; cVal < 16; cVal++)
            vVals[cVal][mVal] = response[cVal] * fac;
        wordCnt = 0;
    }
}

cBool adc_chipselect(enum adcChain chain, cBool csState)
{
    if (ssi_SendindStatus(SSI3) == ssi_sending_busy)
        return bFalse; // Busy, nothin happened!

    ssi3_selectADCs(chain, csState);
    return bTrue;
}

void adc_chipselectBlocking(enum adcChain chain, cBool csState)
{
    while (adc_chipselect(chain, csState) == bFalse)
        ; // Wait until chip-select was done correctly
}
