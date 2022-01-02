#ifndef SSICOMMON_H
#define SSICOMMON_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "TM4C123GH6PM.h"
#include "common.h"

/*******************************\
| Defines
\*******************************/

/*******************************\
| Enum/Struct/Union
\*******************************/
enum ssi_clkRate
{
  ssi_clkRate_125kHz = 0,
  ssi_clkRate_250kHz,
  ssi_clkRate_500kHz,
  ssi_clkRate_1MHz,
};

enum ssiDataSize
{
  ssiDataSize_4bit = 0x03,
  ssiDataSize_5bit = 0x04,
  ssiDataSize_6bit = 0x05,
  ssiDataSize_7bit = 0x06,
  ssiDataSize_8bit = 0x07,
  ssiDataSize_9bit = 0x08,
  ssiDataSize_10bit = 0x09,
  ssiDataSize_11bit = 0x0A,
  ssiDataSize_12bit = 0x0B,
  ssiDataSize_13bit = 0x0C,
  ssiDataSize_14bit = 0x0D,
  ssiDataSize_15bit = 0x0E,
  ssiDataSize_16bit = 0x0F,
};

enum ssi_sendingStatus
{
  ssi_sending_idle = 0,
  ssi_sending_busy,
};

enum ssi_FIFOStatus
{
  ssi_FIFO_Empty = 0,
  ssi_FIFO_Full,
  ssi_FIFO_NeitherEmptyOrFull,
};

/*******************************\
| Function declaration
\*******************************/
void ssi_enable(SSI0_Type *ssi, cBool state);
void ssi_changeClkRate(SSI0_Type *ssi, enum ssi_clkRate clkRate);
void ssi_changeDataSize(SSI0_Type *ssi, enum ssiDataSize dSize);

enum ssi_sendingStatus ssi_SendindStatus(SSI0_Type *ssi);
enum ssi_FIFOStatus ssi_RxFifoStatus(SSI0_Type *ssi);
enum ssi_FIFOStatus ssi_TxFifoStatus(SSI0_Type *ssi);

void ssi_clearRxFIFO(SSI0_Type *ssi);

void ssi_transmit8Bit(SSI0_Type *ssi, uint8_t *serializedStream, uint16_t bytes_n);
void ssi_receive8Bit(SSI0_Type *ssi, uint8_t *serializedStream, uint16_t *nBytes, uint16_t nMaxBytes);

void ssi_transmit16Bit(SSI0_Type *ssi, uint16_t *serializedStream, uint16_t bytes_n);
void ssi_receive16Bit(SSI0_Type *ssi, uint16_t *serializedStream, uint16_t *nBytes, uint16_t nMaxBytes);

#endif