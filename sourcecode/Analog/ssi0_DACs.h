#ifndef SSI0_DACs_H
#define SSI0_DACs_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "common.h"
#include "dacWrapper.h"

/*******************************\
| Defines
\*******************************/
#define SSI0_BUFFERSIZE (DAC_NDACS * DAC_PACK_NBYTES) // Max-Transfer can be "DACs * Bytes per DAC" in Bytes

/*******************************\
| Enum/Struct/Union
\*******************************/
enum ssi0_clkRate
{
  ssi0_clkRate_125kHz = 0,
  ssi0_clkRate_250kHz,
  ssi0_clkRate_500kHz,
  ssi0_clkRate_1MHz,
};

enum ssi0_sendingStatus
{
  ssi0_sending_idle = 0,
  ssi0_sending_busy,
};

enum ssi0_FIFOStatus
{
  ssi0_FIFO_Empty = 0,
  ssi0_FIFO_Full,
  ssi0_FIFO_NeitherEmptyOrFull,
};

/*******************************\
| Function declaration
\*******************************/
void ssi0_init(enum ssi0_clkRate clkRate);
void ssi0_setClkRate(enum ssi0_clkRate clkRate);

enum ssi0_sendingStatus ssi0_SendindStatus(void);
enum ssi0_FIFOStatus ssi0_RxFifoStatus(void);
enum ssi0_FIFOStatus ssi0_TxFifoStatus(void);

void ssi0_clrDACs(cBool state);   // Turns output '0' or '1'
void ssi0_rstDACs(cBool state);   // Turns output '0' or '1'
void ssi0_ldacDACs(cBool state);  // Turns output '0' or '1';
void ssi0_RxOnOff(cBool ioOnOff); // Complete Dis- or Enables the IO
void ssi0_TxOnOff(cBool ioOnOff); // Complete Dis- or Enables the IO

void ssi0_selectDACs(cBool state);
void ssi0_transmit(uint8_t *serializedStream, uint16_t bytes_n);
void ssi0_receive(uint8_t *serializedStream, uint16_t *nBytes, uint16_t nMaxBytes);

#endif