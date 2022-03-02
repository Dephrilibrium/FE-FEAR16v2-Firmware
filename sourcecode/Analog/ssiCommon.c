/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "common.h"
#include "ssiCommon.h"
#include "dma.h"
#include "ssi0_DACs.h"
#include "ssi3_ADCs.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Local Defines
// // Clks
// #define SYSCTL_RCGCSSI_SPI3 BIT(3)
// #define SYSCTL_RCGCGPIO_GPIOC BIT(2)
// #define SYSCTL_RCGCGPIO_GPIOD BIT(3)
// #define SYSCTL_RCGCGPIO_GPIOE BIT(4)
// #define SYSCTL_RCGCGPIO_GPIOF BIT(5)

// /* GPIOs */
// // Both
// #define PD0_ADC_SCLK3 BIT(0) // SSI3-Clock
// #define PD2_ADC_MISO3 BIT(2) // SSI3-MasterInSlaveOut
// #define PD3_ADC_MOSI3 BIT(3) // SSi3-MasterOutSlaveIn

// // Currenflow-ADCs (CFADC)
// #define PF3_CFADC_CS BIT(3)     // Chipselect (low)
// #define PC4_CFADC_RST BIT(4)    // Reset (low)
// #define PC5_CFADC_CONVST BIT(5) // Start measurement
// #define PC6_CFADC_BUSY1 BIT(6)  // Busy first daisy-chain
// #define PC7_CFADC_BUSY2 BIT(7)  // Busy seconf daisy-chain

// // Voltragedrop-ADCs (UDRPADC)
// #define PD1_UDRPADC_CS BIT(1)     // Chipselect
// #define PE1_UDRPADC_RST BIT(1)    // Reset (low)
// #define PE2_UDRPADC_CONVST BIT(2) // Start measurement
// #define PE4_UDRPADC_BUSY1 BIT(4)  // Busy first Daisy-chain
// #define PE5_UDRPADC_BUSY2 BIT(5)  // Busy second Daisy-chain

// #define GPIO_PCTL_CLEAR(x) OPTION(0xf, x * 4)       // Each bitfield has 4 bits
// #define GPIOD_PCTL_PD0_SSI3CLK OPTION(0x02, 0 * 4)  // PCTL for PA2 = SPI0-CLK
// #define GPIOD_PCTL_PD2_SSI3MISO OPTION(0x02, 2 * 4) // PCTL for PA2 = SPI0-MISO
// #define GPIOD_PCTL_PD3_SSI3MOSI OPTION(0x02, 3 * 4) // PCTL for PA2 = SPI0-MOSI

/* SSI */
#define SSI_CR1_SSE BIT(1)            // SSI Enable
#define SSI_CR_MS BIT(2)              // Master (clear) / Slave (set)
#define SSI_CC_SYSCTL OPTION(0x00, 0) // Make CoreClk to ClockSource (can depend on clock-settings!)
#define SSI_CC_PIOSC OPTION(0x05, 0)  // Make PIOSC (Precise Internal Oscillator = 16MHz) to ClockSource
#pragma region CR0 Datasize
#define SSI_CR0_DATASIZE_CLR OPTION(0xF, 0) // Invers Clear-mask
#define SSI_CR0_DATASIZE(x) OPTION(x, 0)    // Shifts given value from enum ssi_dataSize
// #define SSI_CR0_DATASIZE_4 OPTION(0x3, 0)   // 4-bit data
// #define SSI_CR0_DATASIZE_5 OPTION(0x4, 0)   // 5-bit data
// #define SSI_CR0_DATASIZE_6 OPTION(0x5, 0)   // 6-bit data
// #define SSI_CR0_DATASIZE_7 OPTION(0x6, 0)   // 7-bit data
// #define SSI_CR0_DATASIZE_8 OPTION(0x7, 0)   // 8-bit data
// #define SSI_CR0_DATASIZE_9 OPTION(0x8, 0)   // 9-bit data
// #define SSI_CR0_DATASIZE_10 OPTION(0x9, 0)  // 10-bit data
// #define SSI_CR0_DATASIZE_11 OPTION(0xA, 0)  // 11-bit data
// #define SSI_CR0_DATASIZE_12 OPTION(0xB, 0)  // 12-bit data
// #define SSI_CR0_DATASIZE_13 OPTION(0xC, 0)  // 13-bit data
// #define SSI_CR0_DATASIZE_14 OPTION(0xD, 0)  // 14-bit data
// #define SSI_CR0_DATASIZE_15 OPTION(0xE, 0)  // 15-bit data
// #define SSI_CR0_DATASIZE_16 OPTION(0xF, 0)  // 16-bit data
#pragma endregion CR0 Datasize
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
| Local Enum/Struct/Union
\*******************************/
enum ssi_transceiveBitSize
{
    ssi_transceive8Bits,
    ssi_transceive16Bits,
    // ssi_transceive32Bits
};

/*******************************\
| Local function declarations
\*******************************/
enum ssi_FIFOStatus ssi_RxFifoStatus(SSI0_Type *ssi);
void ssi_transceive(SSI0_Type *ssi, const void *serializedOutput, void *serializedInput, uint16_t nTransfers, enum ssi_transceiveBitSize nBitSize);

/*******************************\
| Global variables
\*******************************/

/*******************************\
| Function definitons
\*******************************/
void ssi_enable(SSI0_Type *ssi, cBool state)
{
    if (state == bTrue)
        ssi->CR1 |= SSI_CR1_SSE; // Enable SSI
    else
        ssi->CR1 &= ~SSI_CR1_SSE; // Disable SSI
}

void ssi_changeClkRate(SSI0_Type *ssi, enum ssi_clkRate clkRate)
{
    if (clkRate > PIOSC_MHZ)
        return;

    uint8_t CPSR;
    uint8_t SCR = 0; // Is 0 in all cases!

    switch (clkRate)
    {
    case ssi_clkRate_125kHz:
        CPSR = 128;
        break;

    case ssi_clkRate_250kHz:
        CPSR = 64;
        break;

    case ssi_clkRate_500kHz:
        CPSR = 32;
        break;

    default:
        clkRate = ssi_clkRate_1MHz;
        CPSR = 16;
        break;
    }

    ssi->CPSR = CPSR;
    ssi->CR0 &= ~SSI_CR0_CLR;
    ssi->CR0 |= SSI_CR0_SCR(SCR);
}

void ssi_changeDataSize(SSI0_Type *ssi, enum ssiDataSize dSize)
{
    uint32_t cr0 = ssi->CR0 & ~(SSI_CR0_DATASIZE_CLR);
    ssi->CR0 = cr0 | SSI_CR0_DATASIZE((uint8_t)dSize);
}

enum ssi_sendingStatus ssi_SendindStatus(SSI0_Type *ssi)
{
    if (ssi->SR & SSI_SR_BUSY)
        return ssi_sending_busy;

    return ssi_sending_idle;
}

enum ssi_sendingStatus ssi_TransceiveStatus(SSI0_Type *ssi)
{
    switch ((uint32_t)ssi)
    {
    case (uint32_t)SSI0:
        if (UDMA->ENASET & (BIT(SSI0_RX_DMA_CHNUM) | BIT(SSI0_TX_DMA_CHNUM)))
            return ssi_sending_busy;
        return ssi_sending_idle;

    case (uint32_t)SSI3:
        if (UDMA->ENASET & (BIT(SSI3_RX_DMA_CHNUM) | BIT(SSI3_TX_DMA_CHNUM)))
            return ssi_sending_busy;
        return ssi_sending_idle;

    default:
        break;
    }
    return ssi_sending_unknown;
}

enum ssi_FIFOStatus ssi_RxFifoStatus(SSI0_Type *ssi)
{
    uint32_t SR = ssi->SR;

    /* KEEP IN MIND!
     * When Debugging the Debugger reads out the SSI0-DR Register!
     * Therefore sometimes it can be that the RxFIFO Empty bit can
     *  disappear for "no reason"!
     */

    if (SR & SSI_SR_RFF) // Check receive-FIFO Full
        return ssi_FIFO_Full;
    else if (SR & SSI_SR_RNE) // Check receive FIFO not empty
        return ssi_FIFO_NeitherEmptyOrFull;

    // If both not fullfilled FIFO is empty
    return ssi_FIFO_Empty;
}

// enum ssi_FIFOStatus ssi_TxFifoStatus(SSI0_Type *ssi)
// {
//     uint32_t SR = ssi->SR;

//     if (SR & SSI_SR_TFE) // Check transmit-FIFO empty
//         return ssi_FIFO_Empty;
//     else if (SR & SSI_SR_TNF) // Check transmit-FIFO not full
//         return ssi_FIFO_NeitherEmptyOrFull;

//     // If both not fullfilled FIFO is full
//     return ssi_FIFO_Full;
// }

// Clearing RX-Buffer shouldn't be necessary anymore due to DMA always grabs the same amount of sent packages
// But for security this function will be kept
void ssi_clearRxFIFO(SSI0_Type *ssi)
{
    while (ssi_RxFifoStatus(ssi) != ssi_FIFO_Empty) // Force clearing RxFIFO
        ssi->DR;
}

#pragma region 8 - Bit methods
// Used to directly send nBytes of the given stream to the corresponding SSI-DR register
// void ssi_transmit8Bit(SSI0_Type *ssi, const uint8_t *serializedStream, uint16_t bytes_n)
// {
//     // Has to be managed by the user!
//     // ssi0_selectDACs(bTrue); // Chip-Select

//     for (int iByte = bytes_n - 1; iByte >= 0; iByte--) // Use underflow-trick to check "-1"
//     {
//         while (ssi_TxFifoStatus(ssi) == ssi_FIFO_Full)
//             ; // Wait when FIFO is full!

//         ssi->DR = serializedStream[iByte];
//     }

//     // while (ssi0_SendindStatus() == ssi_sending_busy)
//     //   ;                      // Wait for send finished
//     // ssi0_selectDACs(bFalse); // Chip-Deselect
// }

// // Can be used to read up to 8 bytes from the given SSI-Rx-FIFO into the given serialized stream
// // Gives back the amount of copied bytes (until the current fifo is empty) and checks also a maximum of bytes which can be copied
// void ssi_receive8Bit(SSI0_Type *ssi, uint8_t *serializedStream, uint16_t *nBytesCopied, uint16_t nMaxBytes)
// {
//     /* The received bytes comes in reversed order! */

//     int iCpyByte = nMaxBytes - 1; // CopyIndex for reversed order

//     for (; iCpyByte >= 0; iCpyByte--)
//     {
//         if (ssi_RxFifoStatus(ssi) == ssi_FIFO_Empty)
//             break;

//         serializedStream[iCpyByte] = (uint8_t)ssi->DR;
//     }

//     // Sometimes the RxFifo stores a byte more than expected. This shifts the entire response into wrong positions!
//     // Workaround: Clear RxFIFO after each read
//     ssi_clearRxFIFO(ssi);

//     *nBytesCopied = nMaxBytes - (iCpyByte + 1); // Set amount of copied bytes
// }

void ssi_transceive8Bit(SSI0_Type *ssi, const uint8_t *serializedOutput, uint8_t *serializedInput, uint16_t nBytes)
{
    ssi_transceive(ssi, serializedOutput, serializedInput, nBytes, ssi_transceive8Bits);

    // if (ssi_TransceiveStatus(ssi)) // Do nothing when ongoing action
    //     return;

    // DMA_ChannelControlTable *cct = dma_getCCT();
    // uint16_t nTransfers = nBytes - 1;
    // uint32_t baseConfig = DMA_CHCTL_XFERMODE_BASIC         // One-Shot
    //                       | DMA_CHCTL_NXTUSEBURST(0)       // No burst
    //                       | DMA_CHCTL_XFERSIZE(nTransfers) // Amount of copy
    //                       | DMA_CHCTL_ARBSIZE_1            // 1 Transfer at a time
    //                       | DMA_CHCTL_SRCSIZE_8            // 8-bit outgoing data
    //                       //   | DMA_CHCTL_SRCINC_8 // Not a base-config see following
    //                       | DMA_CHCTL_DSTSIZE_8 // 8-bit incoming data
    //                                             //   | DMA_CHCTL_DSTINC_8  // Not a base-config see following
    //     ;

    // uint32_t rxConfig = baseConfig                                                             // Basic config from above!
    //                     | DMA_CHCTL_SRCINC_0                                                   // Do not increment SSI->DR
    //                     | (serializedInput != NULL ? DMA_CHCTL_DSTINC_8 : DMA_CHCTL_DSTINC_0); // Increment serializedInput for valid pointers

    // uint32_t txConfig = baseConfig                                                              // Basic config from above!
    //                     | DMA_CHCTL_DSTINC_0                                                    // Do not increment SSI->DR
    //                     | (serializedOutput != NULL ? DMA_CHCTL_SRCINC_8 : DMA_CHCTL_SRCINC_0); // Increment serializedOutput for valid pointers

    // uint8_t *ptr;
    // uint32_t chSetMask;
    // switch ((uint32_t)ssi)
    // {
    // case (uint32_t)SSI0:
    //     break;

    // case (uint32_t)SSI3:
    //     // Rx
    //     // Calculate end-pointer for destination
    //     if (serializedInput != NULL)
    //         ptr = serializedInput + nTransfers;
    //     else
    //         ptr = &_dmaInDummies[SSI3_RX_DMA_CHNUM];
    //     // Assign to rxChannel
    //     cct->CH14P.ControlWord = rxConfig;
    //     cct->CH14P.SourceEndPointer = (uint32_t *)&ssi->DR;
    //     cct->CH14P.DestinationEndPointer = (uint32_t *)ptr;

    //     // Tx
    //     // Calculate end-pointer for source
    //     if (serializedOutput != NULL)
    //         ptr = serializedOutput + nTransfers;
    //     else
    //         ptr = &_dmaOutDummies[SSI3_TX_DMA_CHNUM];
    //     // Assign to txChannel
    //     cct->CH15P.ControlWord = txConfig;
    //     cct->CH15P.SourceEndPointer = (uint32_t *)ptr;
    //     cct->CH15P.DestinationEndPointer = (uint32_t *)&ssi->DR;

    //     // Calculate bitmask for reset alt and enable
    //     chSetMask = BIT(SSI3_RX_DMA_CHNUM) | BIT(SSI3_TX_DMA_CHNUM);
    //     break;

    // default:
    //     break;
    // }

    // // Reset to primary and enable
    // UDMA->ALTCLR = chSetMask; // Ensure Primary is used!
    // UDMA->ENASET = chSetMask; // Hardware instantly requests data from DMA
}
#pragma endregion 8 - Bit methods

#pragma region 16 - Bit methods
// Used to directly send nWords of the given stream to the corresponding SSI-DR register
// void ssi_transmit16Bit(SSI0_Type *ssi, const uint16_t *serializedStream, uint16_t words_n)
// {
//     // Has to be managed by the user!
//     // ssi0_selectDACs(bTrue); // Chip-Select

//     for (int iByte = words_n - 1; iByte >= 0; iByte--) // Use underflow-trick to check "-1"
//     {
//         while (ssi_TxFifoStatus(ssi) == ssi_FIFO_Full)
//             ; // Wait when FIFO is full!

//         ssi->DR = serializedStream[iByte];
//     }

//     // while (ssi0_SendindStatus() == ssi_sending_busy)
//     //   ;                      // Wait for send finished
//     // ssi0_selectDACs(bFalse); // Chip-Deselect
// }

// Can be used to read up to 8 words from the given SSI-Rx-FIFO into the given serialized stream
// Gives back the amount of copied words (until the current fifo is empty) and checks also a maximum of words which can be copied
// void ssi_receive16Bit(SSI0_Type *ssi, uint16_t *serializedStream, uint16_t *nWordsCopied, uint16_t nMaxWords)
// {
//     // uint8_t testIter = 0;
//     // uint16_t test[] = {121,
//     //                    222,
//     //                    323,
//     //                    424,
//     //                    525,
//     //                    626,
//     //                    727,
//     //                    828,
//     //                    911,
//     //                    1012,
//     //                    1113,
//     //                    1214,
//     //                    1315,
//     //                    1416,
//     //                    1517,
//     //                    1618};
//     /* The received bytes comes in reversed order! */

//     int iCpyWord = nMaxWords - 1; // CopyIndex for reversed order

//     for (; iCpyWord >= 0; iCpyWord--)
//     {
//         if (ssi_RxFifoStatus(ssi) == ssi_FIFO_Empty)
//             break;

//         serializedStream[iCpyWord] = (uint16_t)ssi->DR;
//         // serializedStream[iCpyWord] = test[testIter++];
//     }

//     // Sometimes the RxFifo stores a byte more than expected. This shifts the entire response into wrong positions!
//     // Workaround: Clear RxFIFO after each read
//     ssi_clearRxFIFO(ssi);

//     *nWordsCopied = nMaxWords - (iCpyWord + 1); // Set amount of copied bytes
// }

void ssi_transceive16Bit(SSI0_Type *ssi, const uint16_t *serializedOutput, uint16_t *serializedInput, uint16_t nWords)
{
    ssi_transceive(ssi, serializedOutput, serializedInput, nWords, ssi_transceive16Bits);

    // if (UDMA->ENASET & (BIT(14) | BIT(15))) // If not finished DO NOT CHANGE configuration during send!
    //     return;

    // DMA_ChannelControlTable *cct = dma_getCCT();
    // uint16_t nTransfers = nWords - 1;
    // uint32_t baseConfig = DMA_CHCTL_XFERMODE_BASIC         // One-Shot
    //                       | DMA_CHCTL_NXTUSEBURST(0)       // No burst
    //                       | DMA_CHCTL_XFERSIZE(nTransfers) // Amount of copy
    //                       | DMA_CHCTL_ARBSIZE_1            // 1 Transfer at a time
    //                       | DMA_CHCTL_SRCSIZE_16           // 16-bit outgoing data
    //                       //   | DMA_CHCTL_SRCINC_16 // Not a base-config see following
    //                       | DMA_CHCTL_DSTSIZE_16 // 16-bit incoming data
    //                                              //   | DMA_CHCTL_DSTINC_16  // Not a base-config see following
    //     ;

    // uint32_t rxConfig = baseConfig                                                              // Basic config from above!
    //                     | DMA_CHCTL_SRCINC_0                                                    // Do not increment SSI->DR
    //                     | (serializedInput != NULL ? DMA_CHCTL_DSTINC_16 : DMA_CHCTL_DSTINC_0); // Increment serializedInput for valid pointers

    // uint32_t txConfig = baseConfig                                                               // Basic config from above!
    //                     | DMA_CHCTL_DSTINC_0                                                     // Do not increment SSI->DR
    //                     | (serializedOutput != NULL ? DMA_CHCTL_SRCINC_16 : DMA_CHCTL_SRCINC_0); // Increment serializedOutput for valid pointers

    // uint16_t *ptr;
    // switch ((uint32_t)ssi)
    // {
    // case (uint32_t)SSI0:
    //     break;

    // case (uint32_t)SSI3:
    //     // Rx
    //     if (serializedInput != NULL)
    //         ptr = serializedInput + nTransfers;
    //     else
    //         cct->CH14P.DestinationEndPointer = (uint32_t *)_dmaInDummies[SSI3_RX_DMA_CHNUM];
    //     cct->CH14P.ControlWord = rxConfig;
    //     cct->CH14P.SourceEndPointer = (uint32_t *)&ssi->DR;
    //     cct->CH14P.DestinationEndPointer = (uint32_t *)ptr;

    //     // Tx
    //     if (serializedOutput != NULL)
    //         ptr = serializedOutput + nTransfers;
    //     else
    //         cct->CH15P.SourceEndPointer = &_dmaOutDummies[SSI3_TX_DMA_CHNUM];
    //     cct->CH15P.ControlWord = txConfig;
    //     cct->CH15P.SourceEndPointer = (uint32_t *)ptr;
    //     cct->CH15P.DestinationEndPointer = (uint32_t *)&ssi->DR;

    //     UDMA->ALTCLR = BIT(SSI3_RX_DMA_CHNUM) | BIT(SSI3_TX_DMA_CHNUM); // Ensure Primary is used!
    //     break;

    // default:
    //     break;
    // }
    // UDMA->ENASET = BIT(14) | BIT(15); // Hardware instantly requests data from DMA
}
#pragma endregion 16 - Bit methods

void ssi_transceive(SSI0_Type *ssi, const void *serializedOutput, void *serializedInput, uint16_t nTransfers, enum ssi_transceiveBitSize nBitSize)
{
    if (ssi_TransceiveStatus(ssi) != ssi_sending_idle) // Do nothing when ongoing action
        return;

    uint8_t rxChNum;
    uint8_t txChNum;
    switch ((uint32_t)ssi)
    {
    case (uint32_t)SSI0:
        rxChNum = SSI0_RX_DMA_CHNUM;
        txChNum = SSI0_TX_DMA_CHNUM;
        break;

    case (uint32_t)SSI3:
        rxChNum = SSI3_RX_DMA_CHNUM;
        txChNum = SSI3_TX_DMA_CHNUM;
        break;

    default: // No matched SSI
        return;
    }

    DMA_ChannelControlTable *cct = dma_getCCT();
    nTransfers -= 1;

    // Build basic configuration
    uint32_t baseConfig = DMA_CHCTL_XFERMODE_BASIC         // One-Shot
                          | DMA_CHCTL_NXTUSEBURST(0)       // No burst
                          | DMA_CHCTL_XFERSIZE(nTransfers) // Amount of copy
                          | DMA_CHCTL_ARBSIZE_1;           // 1 Transfer at a time

    // Build transfer-size options depending on nBitSize
    uint8_t nTransfersMultiplier;    // Depending on nBitSize nTransfers has to be multiplied to match with byte-pointers!
    uint32_t dataSizeConfig;         // Option for bit-size of data
    uint32_t dataIncrementSrcConfig; // Option for regular bit-increment of data
    uint32_t dataIncrementDstConfig; // Option for regular bit-increment of data
    switch (nBitSize)
    {
    case ssi_transceive8Bits:
        nTransfersMultiplier = 1;                     // uint8_t = 1 byte
        dataSizeConfig = DMA_CHCTL_SRCSIZE_8          // 8-bit outgoing data
                         | DMA_CHCTL_DSTSIZE_8;       // 8-bit incomming data
        dataIncrementSrcConfig = DMA_CHCTL_SRCSIZE_8; // 8-bit outgoing data
        dataIncrementDstConfig = DMA_CHCTL_DSTSIZE_8; // 8-bit incoming data
        break;
    case ssi_transceive16Bits:
        nTransfersMultiplier = 2;                      // uint16_t = 2 byte
        dataSizeConfig = DMA_CHCTL_SRCSIZE_16          // 16-bit outgoing data
                         | DMA_CHCTL_DSTSIZE_16;       // 16-bit incomming data
        dataIncrementSrcConfig = DMA_CHCTL_SRCSIZE_16; // 16-bit outgoing data
        dataIncrementDstConfig = DMA_CHCTL_DSTSIZE_16; // 16-bit incoming data
        break;
        // case ssi_transceive32Bits:                         // SSI can transfer max 16 bit at once
        //     nTransfersMultiplier = 4;                      // uint32_t = 4 bytes
        //     dataSizeConfig = DMA_CHCTL_SRCSIZE_32          // 32-bit outgoing data
        //                      | DMA_CHCTL_DSTSIZE_32;       // 32-bit incomming data
        //     dataIncrementSrcConfig = DMA_CHCTL_SRCSIZE_32; // 32-bit outgoing data
        //     dataIncrementDstConfig = DMA_CHCTL_DSTSIZE_32; // 32-bit incoming data
        //     break;

    default: // Error not matching transceive-size
        return;
    }

    // Build dma-config for RX-Channel. DST-increment depends on validity of the given pointer!
    uint8_t *rxDstPtr;
    uint32_t rxConfig = baseConfig            // Basic config from above!
                        | dataSizeConfig      // Add data-size for
                        | DMA_CHCTL_SRCINC_0; // Do not increment SSI->DR

    if (serializedInput != NULL) // If valid rx-Destination-pointer
    {
        rxDstPtr = (uint8_t *)serializedInput + (nTransfers * nTransfersMultiplier); // Calculate last valid address in array
        rxConfig |= dataIncrementDstConfig;                                          // And set the correct increment
    }
    else // NULL-Ptr given -> Use dummy
    {
        rxDstPtr = (uint8_t *)&_dmaInDummies[rxChNum]; // Get dummy-address as byte-pointer
        rxConfig |= DMA_CHCTL_DSTINC_0;                // Increment serializedInput for valid pointers
    }

    // Build dma-config for TX-Channel. SRC-increment depends on validity of the given pointer!
    uint8_t *txDstPtr;
    uint32_t txConfig = baseConfig            // Basic config from above!
                        | dataSizeConfig      // Add data-size for
                        | DMA_CHCTL_DSTINC_0; // Do not increment SSI->DR

    if (serializedOutput != NULL) // If valid rx-Destination-pointer
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
        txDstPtr = (uint8_t *)serializedOutput + (nTransfers * nTransfersMultiplier); // Calculate last valid address in array; Argument and contents won't be changed
#pragma GCC diagnostic pop
        txConfig |= dataIncrementSrcConfig; // And set the correct increment
    }
    else // NULL-Ptr given -> Use dummy
    {
        txDstPtr = (uint8_t *)&_dmaOutDummies[txChNum]; // Get dummy-address as byte-pointer
        txConfig |= DMA_CHCTL_DSTINC_0;                 // Increment serializedInput for valid pointers
    }

    // Assign to Rx-Channel
    cct->ChannelsAsArray[rxChNum].ControlWord = rxConfig;
    cct->ChannelsAsArray[rxChNum].SourceEndPointer = (uint32_t *)&ssi->DR;
    cct->ChannelsAsArray[rxChNum].DestinationEndPointer = (uint32_t *)rxDstPtr;

    // Assign to Tx-Channel
    cct->ChannelsAsArray[txChNum].ControlWord = txConfig;
    cct->ChannelsAsArray[txChNum].SourceEndPointer = (uint32_t *)txDstPtr;
    cct->ChannelsAsArray[txChNum].DestinationEndPointer = (uint32_t *)&ssi->DR;

    // switch ((uint32_t)ssi)
    // {
    // case (uint32_t)SSI0:
    //     // Rx
    //     // Calculate end-pointer for destination
    //     if (serializedInput == NULL)
    //         rxDstPtr = &_dmaInDummies[SSI3_RX_DMA_CHNUM];
    //     // Assign to rxChannel
    //     cct->CH14P.ControlWord = rxConfig;
    //     cct->CH14P.SourceEndPointer = (uint32_t *)&ssi->DR;
    //     cct->CH14P.DestinationEndPointer = (uint32_t *)rxDstPtr;

    //     // Tx
    //     // Calculate end-pointer for source
    //     if (serializedOutput == NULL)
    //         txSrcPtr = &_dmaOutDummies[SSI3_TX_DMA_CHNUM];
    //     // Assign to txChannel
    //     cct->CH15P.ControlWord = txConfig;
    //     cct->CH15P.SourceEndPointer = (uint32_t *)ptr;
    //     cct->CH15P.DestinationEndPointer = (uint32_t *)&ssi->DR;

    //     // Calculate bitmask for reset alt and enable
    //     chSetMask = BIT(SSI3_RX_DMA_CHNUM) | BIT(SSI3_TX_DMA_CHNUM);
    //     break;

    // case (uint32_t)SSI3:
    //     // Rx
    //     // Calculate end-pointer for destination
    //     if (serializedInput != NULL)
    //         ptr = serializedInput + nTransfers;
    //     else
    //         ptr = &_dmaInDummies[SSI3_RX_DMA_CHNUM];
    //     // Assign to rxChannel
    //     cct->CH14P.ControlWord = rxConfig;
    //     cct->CH14P.SourceEndPointer = (uint32_t *)&ssi->DR;
    //     cct->CH14P.DestinationEndPointer = (uint32_t *)ptr;

    //     // Tx
    //     // Calculate end-pointer for source
    //     if (serializedOutput != NULL)
    //         ptr = serializedOutput + nTransfers;
    //     else
    //         ptr = &_dmaOutDummies[SSI3_TX_DMA_CHNUM];
    //     // Assign to txChannel
    //     cct->CH15P.ControlWord = txConfig;
    //     cct->CH15P.SourceEndPointer = (uint32_t *)ptr;
    //     cct->CH15P.DestinationEndPointer = (uint32_t *)&ssi->DR;

    //     // Calculate bitmask for reset alt and enable
    //     chSetMask = BIT(SSI3_RX_DMA_CHNUM) | BIT(SSI3_TX_DMA_CHNUM);
    //     break;

    // default:
    //     break;
    // }

    // Reset to primary and enable
    uint32_t chSetMask = BIT(rxChNum) | BIT(txChNum);
    UDMA->ALTCLR = chSetMask; // Ensure Primary is used!
    UDMA->ENASET = chSetMask; // Hardware instantly requests data from DMA
}