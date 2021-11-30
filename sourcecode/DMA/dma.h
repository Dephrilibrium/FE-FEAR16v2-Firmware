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
#define DMA_MAXIMUM_TRANSFERS 1023 // = 1024 - 1 || 1024 =  Maximum transfers without PingPong
#define DMA_CCT_REGISTERCOUNT 256
#define DMA_CCT_REGISTER_BYTESIZE 4

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
