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
void ssi_changeClkRate(SSI0_Type *ssi, enum ssi_clkRate clkRate);

enum ssi_sendingStatus ssi_SendindStatus(SSI0_Type *ssi);
enum ssi_FIFOStatus ssi_RxFifoStatus(SSI0_Type *ssi);
enum ssi_FIFOStatus ssi_TxFifoStatus(SSI0_Type *ssi);

void ssi_transmit(SSI0_Type *ssi, uint8_t *serializedStream, uint16_t bytes_n);
void ssi_receive(SSI0_Type *ssi, uint8_t *serializedStream, uint16_t *nBytes, uint16_t nMaxBytes);
void ssi_clearRxFIFO(SSI0_Type *ssi);

#endif