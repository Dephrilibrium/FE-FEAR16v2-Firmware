/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "ssi3_ADCs.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Local Defines
// Clks
#define SYSCTL_RCGCSSI_SPI3 BIT(3)
#define SYSCTL_RCGCGPIO_GPIOC BIT(2)
#define SYSCTL_RCGCGPIO_GPIOD BIT(3)
#define SYSCTL_RCGCGPIO_GPIOE BIT(4)
#define SYSCTL_RCGCGPIO_GPIOF BIT(5)

/* GPIOs */
// Both
#define PD0_ADC_SCLK3 BIT(0) // SSI3-Clock
#define PD2_ADC_MISO3 BIT(2) // SSI3-MasterInSlaveOut
#define PD3_ADC_MOSI3 BIT(3) // SSi3-MasterOutSlaveIn

// Currenflow-ADCs (CFADC)
#define PF3_CFADC_CS BIT(3)     // Chipselect (low)
#define PC4_CFADC_RST BIT(4)    // Reset (low)
#define PC5_CFADC_CONVST BIT(5) // Start measurement
#define PC6_CFADC_BUSY1 BIT(6)  // Busy first daisy-chain
#define PC7_CFADC_BUSY2 BIT(7)  // Busy seconf daisy-chain

// Voltragedrop-ADCs (UDRPADC)
#define PD1_UDRPADC_CS BIT(1)     // Chipselect
#define PE1_UDRPADC_RST BIT(1)    // Reset (low)
#define PE2_UDRPADC_CONVST BIT(2) // Start measurement
#define PE4_UDRPADC_BUSY1 BIT(4)  // Busy first Daisy-chain
#define PE5_UDRPADC_BUSY2 BIT(5)  // Busy second Daisy-chain

#define GPIO_PCTL_CLEAR(x) OPTION(0xf, x * 4)       // Each bitfield has 4 bits
#define GPIOD_PCTL_PD0_SSI3CLK OPTION(0x01, 0 * 4)  // PCTL for PA2 = SSI3-CLK
#define GPIOD_PCTL_PD2_SSI3MISO OPTION(0x01, 2 * 4) // PCTL for PA2 = SSI3-MISO
#define GPIOD_PCTL_PD3_SSI3MOSI OPTION(0x01, 3 * 4) // PCTL for PA2 = SSI3-MOSI

/* SPI0 */
#define SSI_CR1_SSE BIT(1)            // SSI Enable
#define SSI_CR_MS BIT(2)              // Master (clear) / Slave (set)
#define SSI_CC_SYSCTL OPTION(0x00, 0) // Make CoreClk to ClockSource (can depend on clock-settings!)
#define SSI_CC_PIOSC OPTION(0x05, 0)  // Make PIOSC (Precise Internal Oscillator = 16MHz) to ClockSource
// #pragma region CR0 Datasize
// #define SSI_CR0_DATASIZE_4 OPTION(0x03, 0)  // 4-bit data
// #define SSI_CR0_DATASIZE_5 OPTION(0x04, 0)  // 5-bit data
// #define SSI_CR0_DATASIZE_6 OPTION(0x05, 0)  // 6-bit data
// #define SSI_CR0_DATASIZE_7 OPTION(0x06, 0)  // 7-bit data
// #define SSI_CR0_DATASIZE_8 OPTION(0x07, 0)  // 8-bit data
// #define SSI_CR0_DATASIZE_9 OPTION(0x08, 0)  // 9-bit data
// #define SSI_CR0_DATASIZE_10 OPTION(0x09, 0) // 10-bit data
// #define SSI_CR0_DATASIZE_11 OPTION(0x0A, 0) // 11-bit data
// #define SSI_CR0_DATASIZE_12 OPTION(0x0B, 0) // 12-bit data
// #define SSI_CR0_DATASIZE_13 OPTION(0x0C, 0) // 13-bit data
// #define SSI_CR0_DATASIZE_14 OPTION(0x0D, 0) // 14-bit data
// #define SSI_CR0_DATASIZE_15 OPTION(0x0E, 0) // 15-bit data
// #define SSI_CR0_DATASIZE_16 OPTION(0x0F, 0) // 16-bit data
// #pragma endregion CR0 Datasize
#define SSI_CR0_FREESCALE OPTION(0x0, 4) // Freescale SPI
#define SSI_CR0_SPO BIT(6)               // Clock idle polarity (0 = low; 1 = high)
#define SSI_CR0_SPH BIT(7)               // Data clock phase (0 = first clock edge; 1 = second clock edge)
#define SSI_CR0_CLR OPTION(0xFF, 8)      // Bitmask for CR0 SCR
#define SSI_CR0_SCR(x) OPTION(x, 8)      // SCR-factor

#define SSI_SR_TFE BIT(0)  // Tx FIFO Empty
#define SSI_SR_TNF BIT(1)  // Tx FIFO Not Full
#define SSI_SR_RNE BIT(2)  // Rx FIFO Not Empty
#define SSI_SR_RFF BIT(3)  // Rx FIFO Full
#define SSI_SR_BUSY BIT(4) // SSI0 Sending (busy-bit)

#pragma endregion Local Defines

/*******************************\
| Local function declarations
\*******************************/
// void ssi3_changeClkRate(enum ssi_clkRate clkRate); // Now in ssiCommon
// void ssi3_enable(cBool state);

/*******************************\
| Global variables
\*******************************/
enum ssi_clkRate _ssi3_clkRate = ADC_SSI_CLKRATE; // Default 1 MHz
// ssi3_dacData_t _serializedOutput = {.Size = SSI3_BUFFERSIZE, .Filled = 0};
// ssi3_dacData_t _serializedInput = {.Size = SSI3_BUFFERSIZE, .Filled = 0};

/*******************************\
| Function definitons
\*******************************/
void ssi3_init(enum ssi_clkRate clkRate)
{
    SYSCTL->RCGCSSI |= SYSCTL_RCGCSSI_SPI3;      // SPI0-CLK
    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_GPIOC    // GPIOC-CLK
                        | SYSCTL_RCGCGPIO_GPIOD  // GPIOD-CLK
                        | SYSCTL_RCGCGPIO_GPIOE  // GPIOE-CLK
                        | SYSCTL_RCGCGPIO_GPIOF; // GPIOF-CLK

    // IOs
    // Unlock pins
    GPIOC->LOCK = 0x4C4F434B; // Unlock CR
    GPIOC->CR = 0xFF;         // Unlock all pins

    GPIOD->LOCK = 0x4C4F434B; // Unlock CR
    GPIOD->CR = 0xFF;         // Unlock all pins

    GPIOE->LOCK = 0x4C4F434B; // Unlock CR
    GPIOE->CR = 0xFF;         // Unlock all pins

    GPIOF->LOCK = 0x4C4F434B; // Unlock CR
    GPIOF->CR = 0xFF;         // Unlock all pins

    // Setup AFSEL
    // Both
    GPIOD->AFSEL |= PD0_ADC_SCLK3    // SPI0-Clock
                    | PD2_ADC_MISO3  // Master In Slave Out
                    | PD3_ADC_MOSI3; // Master Out Slave In

    // CurrentFlow-ADCs
    GPIOF->AFSEL &= ~PF3_CFADC_CS;        // ~Chipselect
    GPIOC->AFSEL &= ~(PC4_CFADC_RST       // ~Reset
                      | PC5_CFADC_CONVST  // Start
                      | PC6_CFADC_BUSY1   // Busy CF-ADC1
                      | PC7_CFADC_BUSY2); // Busy CF-ADC2

    // Voltagedrop-ADCs
    GPIOD->AFSEL &= ~PD1_UDRPADC_CS;        // ~Chipselect
    GPIOE->AFSEL &= ~(PE1_UDRPADC_RST       // ~Reset
                      | PE2_UDRPADC_CONVST  // Start
                      | PE4_UDRPADC_BUSY1   // Busy UDRP-ADC1
                      | PE5_UDRPADC_BUSY2); // Busy UDRP-ADC2

    // Change PCTL
    // Both
    GPIOD->PCTL &= ~(GPIO_PCTL_CLEAR(0)       // PD0 = CLK
                     | GPIO_PCTL_CLEAR(2)     // PD2 = MISO
                     | GPIO_PCTL_CLEAR(3));   // PD3 = MOSI
    GPIOD->PCTL |= GPIOD_PCTL_PD0_SSI3CLK     // CLK
                   | GPIOD_PCTL_PD2_SSI3MISO  // MISO
                   | GPIOD_PCTL_PD3_SSI3MOSI; // MOSI

    // Currentflow-ADCs
    GPIOF->PCTL &= ~(GPIO_PCTL_CLEAR(3));   // Clear PF3 (~CS)
    GPIOC->PCTL &= ~(GPIO_PCTL_CLEAR(4)     // Clear PC4 (~Rst)
                     | GPIO_PCTL_CLEAR(5)   // Clear PC5 (Start measurement)
                     | GPIO_PCTL_CLEAR(6)   // Clear PC6 (Busy ADC1)
                     | GPIO_PCTL_CLEAR(7)); // Clear PC7 (Busy ADC2)

    // Voltagedrop-ADC
    GPIOD->PCTL &= ~(GPIO_PCTL_CLEAR(1));   // Clear PD1 (~CS)
    GPIOE->PCTL &= ~(GPIO_PCTL_CLEAR(1)     // Clear PE1 (~Rst)
                     | GPIO_PCTL_CLEAR(2)   // Clear PE2 (Start measurement)
                     | GPIO_PCTL_CLEAR(4)   // Clear PE4 (Busy ADC1)
                     | GPIO_PCTL_CLEAR(5)); // Clear PE5 (Busy ADC2)

    // Make output where necessary
    // Currentflow-ADCs
    GPIOF->DIR |= PF3_CFADC_CS;       // ~CS
    GPIOC->DIR |= PC4_CFADC_RST       // ~RST
                  | PC5_CFADC_CONVST; // Star meas

    // Voltagedrop-ADCs
    GPIOD->DIR |= PD1_UDRPADC_CS;       // ~CS
    GPIOE->DIR |= PE1_UDRPADC_RST       // ~RST
                  | PE2_UDRPADC_CONVST; // Star meas

    // IO Enable
    // Both
    GPIOD->DEN |= PD0_ADC_SCLK3    // SPI0-Clock
                  | PD2_ADC_MISO3  // Master In Slave Out
                  | PD3_ADC_MOSI3; // Master Out Slave In

    // CurrentFlow-ADCs
    GPIOF->DEN |= PF3_CFADC_CS;      // ~Chipselect
    GPIOC->DEN |= PC4_CFADC_RST      // ~Reset
                  | PC5_CFADC_CONVST // Start
                  | PC6_CFADC_BUSY1  // Busy CF-ADC1
                  | PC7_CFADC_BUSY2; // Busy CF-ADC2

    // Voltagedrop-ADCs
    GPIOD->DEN |= PD1_UDRPADC_CS;      // ~Chipselect
    GPIOE->DEN |= PE1_UDRPADC_RST      // ~Reset
                  | PE2_UDRPADC_CONVST // Start
                  | PE4_UDRPADC_BUSY1  // Busy UDRP-ADC1
                  | PE5_UDRPADC_BUSY2; // Busy UDRP-ADC2

    // Prepare DAC for SSI0-Init
    ssi3_rstADCs(adcChain_CF, bTrue);       // Set ADC-reset while init ssi3
    ssi3_rstADCs(adcChain_UDrp, bTrue);     // Set ADC-reset while init ssi3
    ssi3_selectADCs(adcChain_CF, bFalse);   // Deselect ADCs
    ssi3_selectADCs(adcChain_UDrp, bFalse); // Deselect ADCs
    ssi3_convADCs(adcChain_CF, bFalse);     // Stop accidentially conversion
    ssi3_convADCs(adcChain_UDrp, bFalse);   // Stop accidentially conversion

    // SPI0
    ssi_enable(SSI3, bOff);           // Ensure SSI0 is off
    SSI3->CR1 &= SSI_CR_MS;           // Make SSI0 Master
    SSI3->CC = SSI_CC_PIOSC;          // Use PIOSC (16MHz)
    ssi_changeClkRate(SSI3, clkRate); // Just change! Do not use set-method!
    ssi_changeDataSize(SSI3, ssiDataSize_16bit);
    SSI3->CR0 |= SSI_CR0_FREESCALE // No SPI format
                 | SSI_CR0_SPO     // Clock Polarity: Idle = high
                 | SSI_CR0_SPH;    // Take Data on second clock edge
    // SSI3->CR0 &= ~SSI_CR0_SPO;     // Clock Polarity low
    ssi_enable(SSI3, bOn);

    // Prepare DAC for regular use
    ssi3_rstADCs(adcChain_CF, bFalse);   // Set ADC-reset while init ssi3
    ssi3_rstADCs(adcChain_UDrp, bFalse); // Set ADC-reset while init ssi3
    // ssi0_clrDACs(bFalse);    // Already off
    // ssi0_selectDACs(bFalse); // Already off
    // ssi0_ldacDACs(bFalse);   // Already disabled sychronous output

    // ssi0_clearRxFIFO(); // Be sure nothing undefined is stored in RxFIFO during the init-process
}

void ssi3_setClkRate(enum ssi_clkRate clkRate)
{
    ssi_enable(SSI3, bOff);
    ssi_changeClkRate(SSI3, clkRate);
    _ssi3_clkRate = clkRate;
    ssi_enable(SSI3, bOn);
}

// void ssi3_changeClkRate(enum ssi_clkRate clkRate)
// {
//     if (clkRate > PIOSC_MHZ)
//         return;

//     uint8_t CPSR;
//     uint8_t SCR = 0; // Is 0 in all cases!

//     switch (clkRate)
//     {
//     case ssi_clkRate_125kHz:
//         CPSR = 128;
//         break;

//     case ssi_clkRate_250kHz:
//         CPSR = 64;
//         break;

//     case ssi_clkRate_500kHz:
//         CPSR = 32;
//         break;

//     default:
//         clkRate = ssi_clkRate_1MHz;
//         CPSR = 16;
//         break;
//     }

//     _clkRate = clkRate;
//     SSI3->CPSR = CPSR;
//     SSI3->CR0 &= ~SSI_CR0_CLR;
//     SSI3->CR0 |= SSI3_CR0_SCR(SCR);
// }

// void ssi3_enable(cBool state)
// {
//     if (state == bTrue)
//         SSI3->CR1 |= SSI_CR1_SSE; // Enable SSI
//     else
//         SSI3->CR1 &= ~SSI_CR1_SSE; // Disable SSI
// }

void ssi3_selectADCs(enum adcChain chain, cBool state)
{
    switch (chain)
    {
    case adcChain_CF:
        if (state)
            GPIOF->DATA &= ~PF3_CFADC_CS;
        else
            GPIOF->DATA |= PF3_CFADC_CS;
        break;

    case adcChain_UDrp:
        if (state)
            GPIOD->DATA &= ~PD1_UDRPADC_CS;
        else
            GPIOD->DATA |= PD1_UDRPADC_CS;
        break;

    default:
        break;
    }
}

void ssi3_rstADCs(enum adcChain chain, cBool state)
{
    switch (chain)
    {
    case adcChain_CF:
        if (state)
            GPIOC->DATA |= PC4_CFADC_RST;
        else
            GPIOC->DATA &= ~PC4_CFADC_RST;
        break;

    case adcChain_UDrp:
        if (state)
            GPIOE->DATA |= PE1_UDRPADC_RST;
        else
            GPIOE->DATA &= ~PE1_UDRPADC_RST;
        break;

    default:
        break;
    }
}

void ssi3_convADCs(enum adcChain chain, cBool state)
{
    switch (chain)
    {
    case adcChain_CF:
        if (state)
            GPIOC->DATA |= PC5_CFADC_CONVST;
        else
            GPIOC->DATA &= ~PC5_CFADC_CONVST;
        break;

    case adcChain_UDrp:
        if (state)
            GPIOE->DATA |= PE2_UDRPADC_CONVST;
        else
            GPIOE->DATA &= ~PE2_UDRPADC_CONVST;
        break;

    default:
        break;
    }
}

cBool ssi3_ADCsBusy(enum adcChain chain)
{
    uint32_t status;
    switch (chain)
    {
    case adcChain_CF:
        status = GPIOC->DATA;
        status &= (PC6_CFADC_BUSY1); // | PC7_CFADC_BUSY2);
        break;

    case adcChain_UDrp:
        status = GPIOE->DATA;
        status &= (PE4_UDRPADC_BUSY1); // | PE5_UDRPADC_BUSY2);
        break;

    default:
        return bFalse;
    }

    if (status)
        return bTrue;

    return bFalse; // Securityline to avoid a compilerwarning
}
