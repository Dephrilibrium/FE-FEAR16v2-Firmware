#ifndef DMA_H
#define DMA_H

/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdint.h"

/* Project specific */
#include "common.h"

/*******************************\
| Defines
\*******************************/
// Common
#define DMA_CCT_ALIGNBOUNDARY 1024
#define DMA_CCT_CHANNELS 64
#define DMA_CCT_REGISTERCOUNT 256
#define DMA_CCT_REGISTER_BYTESIZE 4
#define DMA_MAXIMUM_TRANSFERS 1023 // = 1024 - 1 || 1024 =  Maximum transfers without PingPong

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
#define DMA_CHCTL_SRCINC_8 OPTION(0x00, 26)  // 8 bit
#define DMA_CHCTL_SRCINC_16 OPTION(0x01, 26) // 16 bit
#define DMA_CHCTL_SRCINC_32 OPTION(0x02, 26) // 32 bit
#define DMA_CHCTL_SRCINC_0 OPTION(0x03, 26)  // No increment
#pragma endregion Source Data Increment
#pragma region Source Destination Size        // Region contains destination data size defines
#define DMA_CHCTL_DSTSIZE_8 OPTION(0x00, 28)  // 8 bit
#define DMA_CHCTL_DSTSIZE_16 OPTION(0x01, 28) // 16 bit
#define DMA_CHCTL_DSTSIZE_32 OPTION(0x02, 28) // 32 bit
#pragma endregion Source Destination Size
#pragma region Source Destination Increment  // Region contains destination data increment defines
#define DMA_CHCTL_DSTINC_8 OPTION(0x00, 30)  // 8 bit
#define DMA_CHCTL_DSTINC_16 OPTION(0x01, 30) // 16 bit
#define DMA_CHCTL_DSTINC_32 OPTION(0x02, 30) // 32 bit
#define DMA_CHCTL_DSTINC_0 OPTION(0x03, 30)  // No increment
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
typedef struct
{
  uint32_t *SourceEndPointer;
  uint32_t *DestinationEndPointer;
  uint32_t ControlWord;
  uint32_t Reserved;
} DMA_Channel_Structure;

typedef union
{
  DMA_Channel_Structure ChannelsAsArray[DMA_CCT_CHANNELS];
  struct
  {
    DMA_Channel_Structure CH0P;
    DMA_Channel_Structure CH1P;
    DMA_Channel_Structure CH2P;
    DMA_Channel_Structure CH3P;
    DMA_Channel_Structure CH4P;
    DMA_Channel_Structure CH5P;
    DMA_Channel_Structure CH6P;
    DMA_Channel_Structure CH7P;
    DMA_Channel_Structure CH8P;
    DMA_Channel_Structure CH9P;
    DMA_Channel_Structure CH10P;
    DMA_Channel_Structure CH11P;
    DMA_Channel_Structure CH12P;
    DMA_Channel_Structure CH13P;
    DMA_Channel_Structure CH14P;
    DMA_Channel_Structure CH15P;
    DMA_Channel_Structure CH16P;
    DMA_Channel_Structure CH17P;
    DMA_Channel_Structure CH18P;
    DMA_Channel_Structure CH19P;
    DMA_Channel_Structure CH20P;
    DMA_Channel_Structure CH21P;
    DMA_Channel_Structure CH22P;
    DMA_Channel_Structure CH23P;
    DMA_Channel_Structure CH24P;
    DMA_Channel_Structure CH25P;
    DMA_Channel_Structure CH26P;
    DMA_Channel_Structure CH27P;
    DMA_Channel_Structure CH28P;
    DMA_Channel_Structure CH29P;
    DMA_Channel_Structure CH30P;
    DMA_Channel_Structure CH31P;
    DMA_Channel_Structure CH0A;
    DMA_Channel_Structure CH1A;
    DMA_Channel_Structure CH2A;
    DMA_Channel_Structure CH3A;
    DMA_Channel_Structure CH4A;
    DMA_Channel_Structure CH5A;
    DMA_Channel_Structure CH6A;
    DMA_Channel_Structure CH7A;
    DMA_Channel_Structure CH8A;
    DMA_Channel_Structure CH9A;
    DMA_Channel_Structure CH10A;
    DMA_Channel_Structure CH11A;
    DMA_Channel_Structure CH12A;
    DMA_Channel_Structure CH13A;
    DMA_Channel_Structure CH14A;
    DMA_Channel_Structure CH15A;
    DMA_Channel_Structure CH16A;
    DMA_Channel_Structure CH17A;
    DMA_Channel_Structure CH18A;
    DMA_Channel_Structure CH19A;
    DMA_Channel_Structure CH20A;
    DMA_Channel_Structure CH21A;
    DMA_Channel_Structure CH22A;
    DMA_Channel_Structure CH23A;
    DMA_Channel_Structure CH24A;
    DMA_Channel_Structure CH25A;
    DMA_Channel_Structure CH26A;
    DMA_Channel_Structure CH27A;
    DMA_Channel_Structure CH28A;
    DMA_Channel_Structure CH29A;
    DMA_Channel_Structure CH30A;
    DMA_Channel_Structure CH31A;
  };
} DMA_ChannelControlTable;
#pragma endregion DMA Channel Structure

enum dmaChannelStructure
{
  dmaCH0Primary = 0,
  dmaCH1Primary,
  dmaCH2Primary,
  dmaCH3Primary,
  dmaCH4Primary,
  dmaCH5Primary,
  dmaCH6Primary,
  dmaCH7Primary,
  dmaCH8Primary,
  dmaCH9Primary,
  dmaCH10Primary,
  dmaCH11Primary,
  dmaCH12Primary,
  dmaCH13Primary,
  dmaCH14Primary,
  dmaCH15Primary,
  dmaCH16Primary,
  dmaCH17Primary,
  dmaCH18Primary,
  dmaCH19Primary,
  dmaCH20Primary,
  dmaCH21Primary,
  dmaCH22Primary,
  dmaCH23Primary,
  dmaCH24Primary,
  dmaCH25Primary,
  dmaCH26Primary,
  dmaCH27Primary,
  dmaCH28Primary,
  dmaCH29Primary,
  dmaCH30Primary,
  dmaCH31Primary,
  dmaCH0Alternative = dmaCH0Primary + 32,
  dmaCH1Alternative = dmaCH1Primary + 32,
  dmaCH2Alternative = dmaCH2Primary + 32,
  dmaCH3Alternative = dmaCH3Primary + 32,
  dmaCH4Alternative = dmaCH4Primary + 32,
  dmaCH5Alternative = dmaCH5Primary + 32,
  dmaCH6Alternative = dmaCH6Primary + 32,
  dmaCH7Alternative = dmaCH7Primary + 32,
  dmaCH8Alternative = dmaCH8Primary + 32,
  dmaCH9Alternative = dmaCH9Primary + 32,
  dmaCH10Alternative = dmaCH10Primary + 32,
  dmaCH11Alternative = dmaCH11Primary + 32,
  dmaCH12Alternative = dmaCH12Primary + 32,
  dmaCH13Alternative = dmaCH13Primary + 32,
  dmaCH14Alternative = dmaCH14Primary + 32,
  dmaCH15Alternative = dmaCH15Primary + 32,
  dmaCH16Alternative = dmaCH16Primary + 32,
  dmaCH17Alternative = dmaCH17Primary + 32,
  dmaCH18Alternative = dmaCH18Primary + 32,
  dmaCH19Alternative = dmaCH19Primary + 32,
  dmaCH20Alternative = dmaCH20Primary + 32,
  dmaCH21Alternative = dmaCH21Primary + 32,
  dmaCH22Alternative = dmaCH22Primary + 32,
  dmaCH23Alternative = dmaCH23Primary + 32,
  dmaCH24Alternative = dmaCH24Primary + 32,
  dmaCH25Alternative = dmaCH25Primary + 32,
  dmaCH26Alternative = dmaCH26Primary + 32,
  dmaCH27Alternative = dmaCH27Primary + 32,
  dmaCH28Alternative = dmaCH28Primary + 32,
  dmaCH29Alternative = dmaCH29Primary + 32,
  dmaCH30Alternative = dmaCH30Primary + 32,
  dmaCH31Alternative = dmaCH31Primary + 32,
};

/*******************************\
| Global variables
\*******************************/
extern uint32_t _dmaInDummies[DMA_CCT_CHANNELS];  // Can be used to store and read data if no channelbuffers given
extern uint32_t _dmaOutDummies[DMA_CCT_CHANNELS]; // Can be used to store and read data if no channelbuffers given

/*******************************\
| Function declaration
\*******************************/
// Common
// void dma_uart1_init(void);
void dma_init(void);
DMA_ChannelControlTable *dma_getCCT(void);
DMA_Channel_Structure *dma_getCCTChannel(enum dmaChannelStructure cctChannel);

// UART1
// enum dmaE dma_uart1_ConfigRxBuffer(charBuffer_t *rxBuffer);
// uint16_t dma_uart1_GetRxXFERSIZE(void);
// uint32_t *dma_uart1_GetRxHead(void);

// enum dmaE dma_uart1_ConfigTxBuffer(charBuffer_t *txBuffer);
// enum dmaE dma_uart1_TxTransmit(void);

#endif
