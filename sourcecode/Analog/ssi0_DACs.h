#ifndef SSI0_DACS_H
#define SSI0_DACS_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "common.h"
#include "dacWrapper.h"
#include "ssiCommon.h"

/*******************************\
| Defines
\*******************************/
#define SSI0_BUFFERSIZE (DAC_NDACS * DAC_PACK_NBYTES) // Max-Transfer can be "DACs * Bytes per DAC" in Bytes

/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/
void ssi0_init(enum ssi_clkRate clkRate);
void ssi0_setClkRate(enum ssi_clkRate clkRate);

// enum ssi_sendingStatus ssi0_SendindStatus(void);
// enum ssi_FIFOStatus ssi0_RxFifoStatus(void);
// enum ssi_FIFOStatus ssi0_TxFifoStatus(void);

void ssi0_clrDACs(cBool state);   // Turns output '0' or '1'
void ssi0_rstDACs(cBool state);   // Turns output '0' or '1'
void ssi0_ldacDACs(cBool state);  // Turns output '0' or '1';
void ssi0_RxOnOff(cBool ioOnOff); // Complete Dis- or Enables the IO
void ssi0_TxOnOff(cBool ioOnOff); // Complete Dis- or Enables the IO

void ssi0_selectDACs(cBool state);
void ssi0_transmit(uint8_t *serializedStream, uint16_t bytes_n);
void ssi0_receive(uint8_t *serializedStream, uint16_t *nBytes, uint16_t nMaxBytes);
void ssi0_clearRxFIFO(void);

#endif