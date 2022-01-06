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
typedef struct
{
    const uint16_t nChains;
    const uint16_t nPacks;
    const uint16_t nWords;

    const uint32_t stdConf;
    const uint16_t zeroStream[ADC_CPACKS_NALLWORDS];

    struct
    {
        union
        {
            uint32_t confPacks[ADC_NALLCONFPACKS];
            uint16_t confStream[ADC_CPACKS_NALLWORDS];
        };
        union // Currently not implemented
        {
            uint32_t responseConf[ADC_NALLCONFPACKS];
            uint16_t responseStream[ADC_CPACKS_NALLWORDS];
        };
    } chains[ADC_NCHAINS];
} adc_sendConf_t;

typedef struct
{
    double targeRange;
    double voltQuantum;
} adc_voltRange_t;

/*******************************\
| Local function declarations
\*******************************/
void adc_wipeADCData(void);
void adc_setupSequence(void);
void adc_setCPacks(uint32_t config);
// void adc_setVPacks(int16_t value);
void adc_setVPacks(double value);
// void adc_setToPacks(uint8_t cmdByte, uint16_t dataWord, ADC_StructuralDataPack_t *packCollection, uint8_t nPacks);

// enum adc_packIndex adc_IndexInt2EnumPackIndex(uint16_t iPackIndex);
// uint16_t adc_IndexEnumPackIndex2Int(enum adc_packIndex packIndex);
// void adc_prepareTxData(enum adc_packIndex packIndex);
// void adc_fetchRxData(enum adc_packIndex packIndex);

// double adc_selectVoltRange(double voltRange); // Hardly to calculate -> Maybe a feature in future
void adc_reverseRaw(enum adcChain chain);
void adc_convertRaw2Double(enum adcChain chain);

/*******************************\
| Global variables
\*******************************/
adc_sendConf_t _configuration = {
    .nChains = ADC_NCHAINS,
    .nPacks = ADC_NALLCONFPACKS,
    .nWords = ADC_CPACKS_NALLWORDS,
    .stdConf = ADC_CONF_REDAC(1023) // Scale VRef -> 2.5V
                                    //   | ADC_CONF_VREF_3V     // Do not extend internal Reference
                                    //   | ADC_CONF_REFBUF_OFF  // Disable internal VRef Buffer
               | ADC_CONF_REF_EN    // Enable internal VRef
                                    //   | ADC_CONF_PD_D        // Powerdown Channels
                                    //   | ADC_CONF_RANGE_D     // Range 2 * VRef
                                    //   | ADC_CONF_PD_C        // Powerdown Channels
                                    //   | ADC_CONF_RANGE_C     // Range 2 * VRef
                                    //   | ADC_CONF_PD_B        // Powerdown Channels
                                    //   | ADC_CONF_RANGE_B     // Range 2 * VRef
                                    //   | ADC_CONF_RANGE_A     // Range 2 * VRef
                                    //   | ADC_CONF_NSTBY       // Powerdown Device
                                    //   | ADC_CONF_BUSYPOL     // Busy-signal: lo-active
                                    //   | ADC_CONF_BUSY_OR_INT // Use as Interrupt-signal
                                    //   | ADC_CONF_CLKOUT_EN   // Enable clock-out (Pin 34)
                                    //   | ADC_CONF_CLKSEL      // Use external conversion clock
                                    //   | ADC_CONF_READ_EN     // Read config-register in the next 2 cycles!
                                    //   | ADC_CONF_WRITE_EN   // Update register contents
    ,
    .zeroStream = {0},
    .chains = {{{{0}}}}};
adc_measurementValues_t _measVal = {.nChains = ADC_NCHAINS,
                                    .nChannels = ADC_NALLCHPACKS,
                                    .nWords = ADC_CHANNELS_NALLWORDS,
                                    .zeroStream = {0},
                                    .chains = {{{0}}}};
adc_voltRange_t _range = {.targeRange = ADC_POSITIVE_RANGE,
                          .voltQuantum = ADC_POSITIVE_RANGE / ADC_POSITIVE_RESOLUTION};

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
    adc_setCPacks(0);
    adc_setVPacks(0);
    // #pragma GCC diagnostic pop
}

void adc_setCPacks(uint32_t conf)
{
    for (uint8_t chainIndex = 0; chainIndex < ADC_NCHAINS; chainIndex++)
        for (uint16_t cIndex = 0; cIndex < _configuration.nPacks; cIndex++)
            _configuration.chains[chainIndex].confPacks[cIndex] = conf;
}

void adc_setVPacks(double value)
{
    int16_t setValue = (int16_t)(value / _range.voltQuantum);

    for (uint8_t chainIndex = 0; chainIndex < _measVal.nChains; chainIndex++)
        for (uint16_t rIndex = 0; rIndex < _measVal.nChannels; rIndex++)
            _measVal.chains[chainIndex].raw[rIndex] = setValue;

    adc_convertRaw2Double(adcChain_CF);
    adc_convertRaw2Double(adcChain_UDrp);
}

void adc_setupSequence(void)
{
    // static uint32_t basicConfigDWORD = 0;
    // basicConfigDWORD = ADC_CONF_REDAC(1023); // VRef = 2.5V
    //                                          // basicConfigDWORD |= ADC_CONF_VREF_3V;    // Do not extend internal Reference
    //                                          // basicConfigDWORD |= ADC_CONF_REFBUF_OFF; // Disable internal VRef Buffer
    // basicConfigDWORD |= ADC_CONF_REF_EN;     // Enable internal VRef
    // // basicConfigDWORD |= ADC_CONF_PD_D;       // Powerdown Channels
    // // basicConfigDWORD |= ADC_CONF_RANGE_D;    // Range 2 * VRef
    // // basicConfigDWORD |= ADC_CONF_PD_C;       // Powerdown Channels
    // // basicConfigDWORD |= ADC_CONF_RANGE_C;    // Range 2 * VRef
    // // basicConfigDWORD |= ADC_CONF_PD_B;       // Powerdown Channels
    // // basicConfigDWORD |= ADC_CONF_RANGE_B;    // Range 2 * VRef
    // // basicConfigDWORD |= ADC_CONF_RANGE_A;     // Range 2 * VRef
    // // basicConfigDWORD |= ADC_CONF_NSTBY;       // Powerdown Device
    // // basicConfigDWORD |= ADC_CONF_BUSYPOL;     // Busy-signal: lo-active
    // // basicConfigDWORD |= ADC_CONF_BUSY_OR_INT; // Use as Interrupt-signal
    // // basicConfigDWORD |= ADC_CONF_CLKOUT_EN; // Enable clock-out (Pin 34)
    // // basicConfigDWORD |= ADC_CONF_CLKSEL;    // Use external conversion clock
    // // basicConfigDWORD |= ADC_CONF_READ_EN;  // Read config-register in the next 2 cycles!
    // // basicConfigDWORD |= ADC_CONF_WRITE_EN; // Update register contents

    pTime_wait(10000); // Wait 10ms
    while (ssi3_ADCsBusy(adcChain_CF) || ssi3_ADCsBusy(adcChain_UDrp))
        ; // To be sure ADCs POR is finished, wait for ADC-chains ready

    // Prepare config
    adc_setCPacks(_configuration.stdConf | ADC_CONF_WRITE_EN | ADC_CONF_READ_EN);

    // Send config one by one (could also be done simultaneously, but to avoid may occuring shortcuts between slave outputs)
    uint16_t wordCount = 0;
    for (uint8_t chainIndex = 0; chainIndex < _configuration.nChains; chainIndex++)
    {
        adc_chipselectBlocking(chainIndex, bOn);
        ssi_transmit16Bit(SSI3, _configuration.chains[chainIndex].confStream, _configuration.nWords);
        adc_chipselectBlocking(chainIndex, bOff);
        while (_configuration.chains[chainIndex].responseConf[_configuration.nPacks - 1] == 0)
        {
            adc_chipselectBlocking(chainIndex, bOn);
            ssi_transmit16Bit(SSI3, _configuration.zeroStream, _configuration.nWords);
            adc_chipselectBlocking(chainIndex, bOff);
            ssi_receive16Bit(SSI3, _configuration.chains[chainIndex].responseStream, &wordCount, _configuration.nWords);
        }
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

void adc_reverseRaw(enum adcChain chain)
{
    int16_t bakVal;

    uint16_t iOpposite;
    uint16_t halfIndex = _measVal.nChannels / 2;
    for (uint16_t iRev = 0; iRev < halfIndex; iRev++)
    {
        iOpposite = (_measVal.nChannels - 1) - iRev;

        bakVal = _measVal.chains[chain].raw[iRev];
        _measVal.chains[chain].raw[iRev] = _measVal.chains[chain].raw[iOpposite];
        _measVal.chains[chain].raw[iOpposite] = bakVal;
    }
}

void adc_convertRaw2Double(enum adcChain chain)
{
    adc_reverseRaw(chain);

    for (uint16_t iConv = 0; iConv < _measVal.nChannels; iConv++)
        _measVal.chains[chain].voltages[iConv] = _range.voltQuantum * _measVal.chains[chain].raw[iConv];
}

void adc_takeMeasurement(enum adcChain chain)
{
    while (ssi3_ADCsBusy(chain) || ssi_SendindStatus(SSI3) != ssi_sending_idle)
        ; // Wait for SSI and ADCs ready

    ssi3_convADCs(chain, bTrue);
    ssi3_convADCs(chain, bFalse);

    while (ssi3_ADCsBusy(chain))
        ; // Wait for ADCs finished sampling

    uint16_t wordCnt = 0;
    adc_chipselectBlocking(chain, bTrue);
    ssi_transmit16Bit(SSI3, _measVal.zeroStream, _measVal.nWords);
    adc_chipselectBlocking(chain, bFalse);
    ssi_receive16Bit(SSI3, (uint16_t *)_measVal.chains[chain].raw, &wordCnt, _measVal.nWords);
    // ssi_clearRxFIFO(SSI3);

    adc_convertRaw2Double(chain);
}

adc_measurementValues_t *adc_grabMeasurements(void)
{
    return &_measVal;
}
