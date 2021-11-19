#ifndef DMA_H
#define DMA_H

/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdint.h"

/* Project specific */
#include "helper.h"
#include "cTypes.h"

/*******************************\
| Defines
\*******************************/
// Common
#define DMA_MAXIMUM_TRANSFERS 1023 // = 1024 - 1 || 1024 =  Maximum transfers without PingPong
#define DMA_CCT_REGISTERCOUNT 256
#define DMA_CCT_REGISTER_BYTESIZE 4

#define SYSCTL_RCGCDMA_DMA BIT(0) // DMA Clk
#define DMA_CFG_MASTEN BIT(0);    // Master enable

// ChannelControl Control-Word
#pragma region Transfermode                                              // Region contains Transfermode-Defines
#define DMA_CHCTL_XFERMODE_STOP OPTION(0, 0)                             // Channel is stopped
#define DMA_CHCTL_XFERMODE_BASIC OPTION(1, 0)                            // Channel performs ARBSIZE transfer when triggered by peripheral or software
#define DMA_CHCTL_XFERMODE_AUTOREQUEST OPTION(2, 0)                      // Channel performs XFERSIZE transfer when initiated by peripheral or software
#define DMA_CHCTL_XFERMODE_PINGPONG OPTION(3, 0)                         // Using both (Primary and Alternate) channel. Toggles between Primary and Alternate when XFERSIZE-transfers of one channel is done.
#define DMA_CHCTL_XFERMODE_MEMORZSCATTERGATHER OPTION(4, 0)              // Complex! See datasheet
#define DMA_CHCTL_XFERMODE_ALTERNATEMEMORZSCATTERGATHER OPTION(5, 0)     // Complex! See datasheet
#define DMA_CHCTL_XFERMODE_PERIPHERALSCATTERGATHER OPTION(6, 0)          // Complex! See datasheet
#define DMA_CHCTL_XFERMODE_ALTERNATEPERIPHERALSCATTERGATHER OPTION(7, 0) // Complex! See datasheet
#pragma endregion Transfermode
#define DMA_CHCTL_NXTUSEBURST(x) OPTION(x, 3) // Use Burst-Transfer: x = 1 active, x = 0 deactive
#define DMA_CHCTL_XFERSIZE_OFFSET 4
#define DMA_CHCTL_XFERSIZE(x) OPTION(x, DMA_CHCTL_XFERSIZE_OFFSET) // Transfers - 1
#pragma region ARBSIZE                                             // Region contains the arbitration-size defines
#define DMA_CHCTL_ARBSIZE_1 OPTION(0x00, 14)                       // 1 transfer before re-arbitration
#define DMA_CHCTL_ARBSIZE_2 OPTION(0x01, 14)                       // 2 ransfers before re-arbitration
#define DMA_CHCTL_ARBSIZE_4 OPTION(0x02, 14)                       // 4 ransfers before re-arbitration
#define DMA_CHCTL_ARBSIZE_8 OPTION(0x03, 14)                       // 8 ransfers before re-arbitration
#define DMA_CHCTL_ARBSIZE_16 OPTION(0x04, 14)                      // 16 ransfers before re-arbitration
#define DMA_CHCTL_ARBSIZE_32 OPTION(0x05, 14)                      // 32 ransfers before re-arbitration
#define DMA_CHCTL_ARBSIZE_64 OPTION(0x06, 14)                      // 64 ransfers before re-arbitration
#define DMA_CHCTL_ARBSIZE_128 OPTION(0x07, 14)                     // 128 ransfers before re-arbitration
#define DMA_CHCTL_ARBSIZE_256 OPTION(0x08, 14)                     // 256 ransfers before re-arbitration
#define DMA_CHCTL_ARBSIZE_512 OPTION(0x09, 14)                     // 512 ransfers before re-arbitration
#define DMA_CHCTL_ARBSIZE_1024 OPTION(0x0A, 14)                    // 1024 ransfers before re-arbitration
#pragma endregion ARBSIZE
#pragma region Source Data Size               // Region contains source data size defines
#define DMA_CHCTL_SRCSIZE_8 OPTION(0x00, 24)  // 8 bit
#define DMA_CHCTL_SRCSIZE_16 OPTION(0x01, 24) // 16 bit
#define DMA_CHCTL_SRCSIZE_32 OPTION(0x02, 24) // 32 bit
#pragma endregion Source Data Size
#pragma region Source Data Increment         // Region contains source data increment defines
#define DMA_CHCTL_SRCINC_8 OPTION(0x00, 24)  // 8 bit
#define DMA_CHCTL_SRCINC_16 OPTION(0x01, 24) // 16 bit
#define DMA_CHCTL_SRCINC_32 OPTION(0x02, 24) // 32 bit
#define DMA_CHCTL_SRCINC_0 OPTION(0x03, 24)  // No increment
#pragma endregion Source Data Increment
#pragma region Source Destination Size        // Region contains destination data size defines
#define DMA_CHCTL_DSTSIZE_8 OPTION(0x00, 24)  // 8 bit
#define DMA_CHCTL_DSTSIZE_16 OPTION(0x01, 24) // 16 bit
#define DMA_CHCTL_DSTSIZE_32 OPTION(0x02, 24) // 32 bit
#pragma endregion Source Destination Size
#pragma region Source Destination Increment  // Region contains destination data increment defines
#define DMA_CHCTL_DSTINC_8 OPTION(0x00, 24)  // 8 bit
#define DMA_CHCTL_DSTINC_16 OPTION(0x01, 24) // 16 bit
#define DMA_CHCTL_DSTINC_32 OPTION(0x02, 24) // 32 bit
#define DMA_CHCTL_DSTINC_0 OPTION(0x03, 24)  // No increment
#pragma endregion Destination Increment

#define NVIC_ISER1_DMA_HANDLE BIT(15) // Enable global IR for DMA finished (Bit46 -> 15 in ISER[1])
#define NVIC_ISER1_DMA_ERROR BIT(16)  // Enable global IR for DMA error (Bit47 -> 16 in ISER[1])

// UART1
// #define DMA_CH8_U1RX BIT(8)
// #define DMA_CH9_U1TX BIT(9)
// // #define DMA_CH22_U1RX BIT(22)
// // #define DMA_CH23_U1TX BIT(23)
// #define DMA_CHMAP_CLR(x) OPTION(0x0F, (x * 4) % 32) // Bitmask to clear Channel Mapping Selection; Calculation keeps channel-bitshift in range of 32
// #define DMA_CHMAP_CH8_U1RX OPTION(0x01, 0)
// #define DMA_CHMAP_CH9_U1TX OPTION(0x01, 4)

/*******************************\
| Enum/Struct/Union
\*******************************/
#pragma region DMA Channel Structure
struct DMA_Channel_Structure
{
  uint32_t *SourceEndPointer;
  uint32_t *DestinationEndPointer;
  uint32_t ControlWord;
  uint32_t Reserved;
};
struct DMA_ChannelControlTable
{
  struct DMA_Channel_Structure CH0P;
  struct DMA_Channel_Structure CH1P;
  struct DMA_Channel_Structure CH2P;
  struct DMA_Channel_Structure CH3P;
  struct DMA_Channel_Structure CH4P;
  struct DMA_Channel_Structure CH5P;
  struct DMA_Channel_Structure CH6P;
  struct DMA_Channel_Structure CH7P;
  struct DMA_Channel_Structure CH8P;
  struct DMA_Channel_Structure CH9P;
  struct DMA_Channel_Structure CH10P;
  struct DMA_Channel_Structure CH11P;
  struct DMA_Channel_Structure CH12P;
  struct DMA_Channel_Structure CH13P;
  struct DMA_Channel_Structure CH14P;
  struct DMA_Channel_Structure CH15P;
  struct DMA_Channel_Structure CH16P;
  struct DMA_Channel_Structure CH17P;
  struct DMA_Channel_Structure CH18P;
  struct DMA_Channel_Structure CH19P;
  struct DMA_Channel_Structure CH20P;
  struct DMA_Channel_Structure CH21P;
  struct DMA_Channel_Structure CH22P;
  struct DMA_Channel_Structure CH23P;
  struct DMA_Channel_Structure CH24P;
  struct DMA_Channel_Structure CH25P;
  struct DMA_Channel_Structure CH26P;
  struct DMA_Channel_Structure CH27P;
  struct DMA_Channel_Structure CH28P;
  struct DMA_Channel_Structure CH29P;
  struct DMA_Channel_Structure CH30P;
  struct DMA_Channel_Structure CH31P;
  struct DMA_Channel_Structure CH0A;
  struct DMA_Channel_Structure CH1A;
  struct DMA_Channel_Structure CH2A;
  struct DMA_Channel_Structure CH3A;
  struct DMA_Channel_Structure CH4A;
  struct DMA_Channel_Structure CH5A;
  struct DMA_Channel_Structure CH6A;
  struct DMA_Channel_Structure CH7A;
  struct DMA_Channel_Structure CH8A;
  struct DMA_Channel_Structure CH9A;
  struct DMA_Channel_Structure CH10A;
  struct DMA_Channel_Structure CH11A;
  struct DMA_Channel_Structure CH12A;
  struct DMA_Channel_Structure CH13A;
  struct DMA_Channel_Structure CH14A;
  struct DMA_Channel_Structure CH15A;
  struct DMA_Channel_Structure CH16A;
  struct DMA_Channel_Structure CH17A;
  struct DMA_Channel_Structure CH18A;
  struct DMA_Channel_Structure CH19A;
  struct DMA_Channel_Structure CH20A;
  struct DMA_Channel_Structure CH21A;
  struct DMA_Channel_Structure CH22A;
  struct DMA_Channel_Structure CH23A;
  struct DMA_Channel_Structure CH24A;
  struct DMA_Channel_Structure CH25A;
  struct DMA_Channel_Structure CH26A;
  struct DMA_Channel_Structure CH27A;
  struct DMA_Channel_Structure CH28A;
  struct DMA_Channel_Structure CH29A;
  struct DMA_Channel_Structure CH30A;
  struct DMA_Channel_Structure CH31A;
};
#pragma endregion DMA Channel Structure

// enum dmaE
// {
//   dmaE_Ok = 0,
//   dmaE_ArgNULL,
//   dmaE_MaxTransferExceeded,
//   dmaE_TransmitCountZero,
//   dmaE_TransmitOngoing,
// };

/*******************************\
| Function declaration
\*******************************/
// Common
void dma_uart1_init(void);

// UART1
// enum dmaE dma_uart1_ConfigRxBuffer(charBuffer_t *rxBuffer);
// uint16_t dma_uart1_GetRxXFERSIZE(void);
// uint32_t *dma_uart1_GetRxHead(void);

// enum dmaE dma_uart1_ConfigTxBuffer(charBuffer_t *txBuffer);
// enum dmaE dma_uart1_TxTransmit(void);

#endif
