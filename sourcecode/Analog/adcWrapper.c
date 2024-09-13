/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "math.h"
#include "stdio.h"

/* Project specific */
#include "common.h"
#include "adcWrapper.h"
#include "cmdsADC.h"
#include "dma.h"
#include "ssi3_ADCs.h"
// #include "preciseTime.h"
#include "DelayTimer.h"
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
    const uint16_t nADCs;
    const uint16_t nPacks;
    const uint16_t nWords;

    const uint32_t stdConf;
    const uint16_t zeroStream[ADC_CPACKS_NALLWORDS];

    struct
    {
        uint32_t confPacks[ADC_NALLCONFPACKS];
        union
        {
            uint16_t confStream[ADC_CPACKS_NALLWORDS];
            uint32_t confStreamSwapped[ADC_NALLCONFPACKS];
        };
        uint32_t responseConf[ADC_NALLCONFPACKS];
        union
        {
            uint16_t responseStream[ADC_CPACKS_NALLWORDS];
            uint32_t responseStreamSwapped[ADC_NALLCONFPACKS];
        };
        // uint16_t responseStream[ADC_CPACKS_NALLWORDS];
    } chains[ADC_NCHAINS];
} adc_sendConf_t;

typedef struct
{
    double targetRange;
    double voltQuantum;
} adc_voltRange_t;

/*******************************\
| Local function declarations
\*******************************/
void adcs_dmaConfig(void);
void adc_wipeADCData(void);
void adc_setupSequence(void);
void adc_setCPacks(uint32_t config);
void adc_setVPacks(double value);

void adc_reverseRaw(enum adcChain chain);
void adc_convertRaw2Double(enum adcChain chain);

/*******************************\
| Global variables
\*******************************/
adc_sendConf_t _configuration = {
    .nChains = ADC_NCHAINS,
    .nADCs = ADC_NADCS,
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
                                    //   | ADC_CONF_CLKSEL    b  // Use external conversion clock
                                    //   | ADC_CONF_READ_EN     // Read config-register in the next 2 cycles!
                                    //   | ADC_CONF_WRITE_EN   // Update register contents
    ,
    .zeroStream = {0},
    .chains = {{{0}}}};
adc_measurementValues_t _measVal = {.nChains = ADC_NCHAINS,
                                    .nADCs = ADC_NADCS,
                                    .nChannels = ADC_NALLCHPACKS,
                                    .nWords = ADC_CHANNELS_NALLWORDS,
                                    .zeroStream = {0},
                                    .chains = {{{0}}}};
adc_voltRange_t _range = {.targetRange = ADC_POSITIVE_RANGE,
                          .voltQuantum = -ADC_POSITIVE_RANGE / ADC_POSITIVE_RESOLUTION}; // - because the OpAmp is inverting!

/*******************************\
| Function definitons
\*******************************/
void adcs_init(void)
{
    ssi3_init(ADC_SSI_CLKRATE);
    adc_wipeADCData();
    adc_setupSequence();

    cmdsADC_InitializeMeanStruct();
    cmdsADC_InitializeMeasurementDelta();
}

void adc_wipeADCData(void)
{
    /* Wipe structure */
    adc_setCPacks(0);
    adc_setVPacks(0);
}

void adc_setCPacks(uint32_t conf)
{
    for (uint8_t chainIndex = 0; chainIndex < ADC_NCHAINS; chainIndex++)
        for (uint16_t cIndex = 0; cIndex < _configuration.nPacks; cIndex++)
        {
            _configuration.chains[chainIndex].confPacks[cIndex] = conf;
            _configuration.chains[chainIndex].confStreamSwapped[cIndex] = swap_dword(conf);
        }
}

void adc_setVPacks(double value)
{
    int16_t setValue = (int16_t)(value / _range.voltQuantum);

    for (uint8_t chainIndex = 0; chainIndex < _measVal.nChains; chainIndex++)
        for (uint16_t rIndex = 0; rIndex < _measVal.nChannels; rIndex++)
            _measVal.chains[chainIndex].raw[rIndex] = setValue;

    adc_convertRaw2Double(adcChain_SHNT);
    adc_convertRaw2Double(adcChain_UDrp);
}

void adc_setupSequence(void)
{
    Delay_Await(10); // Wait 10ms
    while (ssi3_ADCsBusy(adcChain_SHNT) || ssi3_ADCsBusy(adcChain_UDrp))
        ; // To be sure ADCs POR is finished, wait for ADC-chains ready

    // Prepare config
    adc_setCPacks(_configuration.stdConf | ADC_CONF_WRITE_EN | ADC_CONF_READ_EN);

    // Send config one by one (could also be done simultaneously, but to avoid may occuring shortcuts between slave outputs)
    for (uint8_t chainIndex = 0; chainIndex < _configuration.nChains; chainIndex++)
    {
        adc_chipselectBlocking(chainIndex, bOn);
        ssi_transceive16Bit(SSI3, _configuration.chains[chainIndex].confStream, _configuration.chains[chainIndex].responseStream, _configuration.nWords / ADC_NADCS); // MOSI connected to both MISO-Pins of the ADCs -> All ADCs initialized with same message!
        adc_chipselectBlocking(chainIndex, bOff);

        adc_chipselectBlocking(chainIndex, bOn);
        for (uint8_t iSend = 4; iSend; iSend--)
            ssi_transceive16Bit(SSI3, NULL, _configuration.chains[chainIndex].confStream, _configuration.nWords); // Response is via Daisy-Chain
        adc_chipselectBlocking(chainIndex, bOff);
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

    uint16_t iLowerADC, iUpperADC;
    uint16_t iLowerCopy, iUpperCopy;
    uint16_t iHalfADCs = _measVal.nADCs / 2;
    uint16_t chPerADC = _measVal.nChannels / _measVal.nADCs;

    for (iLowerADC = 0; iLowerADC < iHalfADCs; iLowerADC++)
    {
        iUpperADC = (_measVal.nADCs - 1) - iLowerADC;
        for (uint16_t iCh = 0; iCh < chPerADC; iCh++)
        {
            iLowerCopy = iLowerADC * chPerADC + iCh;
            iUpperCopy = iUpperADC * chPerADC + iCh;

            bakVal = _measVal.chains[chain].raw[iLowerCopy];
            _measVal.chains[chain].raw[iLowerCopy] = _measVal.chains[chain].raw[iUpperCopy];
            _measVal.chains[chain].raw[iUpperCopy] = bakVal;
        }
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

    adc_chipselectBlocking(chain, bTrue);
    ssi_transceive16Bit(SSI3, _measVal.zeroStream, (uint16_t *)_measVal.chains[chain].raw, _measVal.nWords);
    adc_chipselectBlocking(chain, bFalse);

    adc_convertRaw2Double(chain);
}

adc_measurementValues_t *adc_grabMeasurements(void)
{
    return &_measVal;
}
