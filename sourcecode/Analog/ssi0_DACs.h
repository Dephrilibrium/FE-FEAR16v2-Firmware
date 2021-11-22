#ifndef SSI0_DACs_H
#define SSI0_DACs_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "common.h"

/*******************************\
| Defines
\*******************************/
// Data and packages
#define DAC_DATA_BUFFER_SIZE 16 * 24 // 24 Bits

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

// struct DACData
// {
//   uint16_t Size;
//   uint16_t Filled;
//   union
//   {
//     uint8_t SerializedStream[DAC_DATA_BUFFER_SIZE];
//     struct
//     {
//       uint8_t
//     }
//   };
// };
// typedef struct DACData DACData_t;

/*******************************\
| Function declaration
\*******************************/
void ssi0_init(enum ssi0_clkRate clkRate);
void ssi0_setClkRate(enum ssi0_clkRate clkRate);

void ssi0_clrDACs(cBool state);
void ssi0_rstDACs(cBool state);

#endif