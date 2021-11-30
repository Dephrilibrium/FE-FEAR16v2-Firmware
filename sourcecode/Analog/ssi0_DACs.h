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
// // Data and packages
// //#define DACS_IN_DAISY_CHAIN 2 // Amount of daisy-chained DACs
// #define DACS_BYTES_PER_DAC 3 // 1 Control-Byte, 2 Data-Bytes
// #ifdef DACS_IN_DAISY_CHAIN
// #define DAC_BYTESIZE (DACS_IN_DAISY_CHAIN * (DAC_CTRL_BYTE + DAC_DATA_WORDS * 2)) // Each Channel has 2 DataBytes
// #else
// #define DAC_BYTESIZE DACS_BYTES_PER_DAC // Each Channel has 2 DataBytes
// #endif

// // DAC
// #define DAC_READ 0x00    // Do nothing when reading from DAC
// #define DAC_WRITE BIT(7) // Set when writing to DAC-Registers
// #pragma region DAC Register Addresses
// #define DAC_REG_ADDR_NOP OPTION(0x00, 0)         // Use for no proper operation
// #define DAC_REG_ADDR_DEVICEID OPTION(0x01, 0)    // [2:15] Device-ID; [0:1] Version-ID;
// #define DAC_REG_ADDR_STATUS OPTION(0x02, 0)      // [0] Temp-Alarm; [1] DAC-Busy; [2] CRC-Alarm; [3:15] Reserved
// #define DAC_REG_ADDR_SPICONFIG OPTION(0x03, 0)   // [0,7,8,12:15] Reserved; [1] FSDO
// #define DAC_REG_ADDR_GENCONFIG OPTION(0x04, 0)   //
// #define DAC_REG_ADDR_BRDCONFIG OPTION(0x05, 0)   //
// #define DAC_REG_ADDR_SYNCCONFIG OPTION(0x06, 0)  //
// #define DAC_REG_ADDR_TOGGCONFIG0 OPTION(0x07, 0) //
// #define DAC_REG_ADDR_TOGGCONFIG1 OPTION(0x08, 0) //
// #define DAC_REG_ADDR_DACPWDWN OPTION(0x09, 0)    //
// #define DAC_REG_ADDR_DACRANGE0 OPTION(0x0B, 0)   //
// #define DAC_REG_ADDR_DACRANGE1 OPTION(0x0C, 0)   //
// #define DAC_REG_ADDR_TRIGGER OPTION(0x0E, 0)     //
// #define DAC_REG_ADDR_BRDCAST OPTION(0x0F, 0)     //
// #define DAC_REG_ADDR_DAC0 OPTION(0x14, 0)        //
// #define DAC_REG_ADDR_DAC1 OPTION(0x15, 0)        //
// #define DAC_REG_ADDR_DAC2 OPTION(0x16, 0)        //
// #define DAC_REG_ADDR_DAC3 OPTION(0x17, 0)        //
// #define DAC_REG_ADDR_DAC4 OPTION(0x18, 0)        //
// #define DAC_REG_ADDR_DAC5 OPTION(0x19, 0)        //
// #define DAC_REG_ADDR_DAC6 OPTION(0x1A, 0)        //
// #define DAC_REG_ADDR_DAC7 OPTION(0x1B, 0)        //
// #define DAC_REG_ADDR_OFFSET0 OPTION(0x21, 0)     //
// #define DAC_REG_ADDR_OFFSET1 OPTION(0x22, 0)     //
// #pragma endregion DAC Register Addresses

// #pragma region DAC Register Bits
// #define DAC_NOP_DATA 0x00 // Always use this when writing to NOP (e.g. reading data)
// #define DAC_DEVICEID_VERSIONID
// #pragma endregion DAC Register Bits

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

void ssi0_clrDACs(cBool state);
void ssi0_rstDACs(cBool state);
void ssi0_ldacDACs(cBool state);
DAC_DataOut_t *DAC_getDataOut(void);
void ssi0_transmit(void);

#endif