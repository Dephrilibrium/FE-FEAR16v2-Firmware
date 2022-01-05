#ifndef SSI3_ADCS_H
#define SSI3_ADCS_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "common.h"
#include "ssiCommon.h"
#include "adcWrapper.h"

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
void ssi3_init(enum ssi_clkRate clkRate);
void ssi3_setClkRate(enum ssi_clkRate clkRate);

// enum ssi_sendingStatus ssi3_SendindStatus(void);
// enum ssi_FIFOStatus ssi3_RxFifoStatus(void);
// enum ssi_FIFOStatus ssi3_TxFifoStatus(void);

// void ssi3_clrDACs(cBool state);   // Turns output '0' or '1'
void ssi3_rstADCs(enum adcChain chain, cBool state);  // Turns reset-output '0' or '1' of the given chain
void ssi3_convADCs(enum adcChain chain, cBool state); // Turns conversion-output '0' or '1' of the given chain
cBool ssi3_ADCsBusy(enum adcChain chain);             // Check the busy-state of the given chain

void ssi3_RxOnOff(cBool ioOnOff); // Complete Dis- or Enables the IO
void ssi3_TxOnOff(cBool ioOnOff); // Complete Dis- or Enables the IO

void ssi3_selectADCs(enum adcChain chain, cBool state);
// void ssi3_transmit(uint8_t *serializedStream, uint16_t bytes_n);
// void ssi3_receive(uint8_t *serializedStream, uint16_t *nBytes, uint16_t nMaxBytes);
// void ssi3_clearRxFIFO(void);
#endif