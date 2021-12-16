/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "ssiCommon.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Hardwaredefines

#pragma endregion Hardwaredefines

/*******************************\
| Local Enum/Struct/Union
\*******************************/

/*******************************\
| Local function declarations
\*******************************/

/*******************************\
| Global variables
\*******************************/

/*******************************\
| Function definitons
\*******************************/
// void ssi_transmit(SSI0_Type *ssi, uint8_t *serializedStream, uint16_t bytes_n)
// {
//   // Has to be managed by the user!
//   // ssi0_selectDACs(bTrue); // Chip-Select

//   for (int iByte = bytes_n - 1; iByte >= 0; iByte--) // Use underflow-trick to check "-1"
//   {
//     while (ssi0_TxFifoStatus() == ssi_FIFO_Full)
//       ; // Wait when FIFO is full!

//     SSI0->DR = serializedStream[iByte];
//   }

//   // while (ssi0_SendindStatus() == ssi0_sending_busy)
//   //   ;                      // Wait for send finished
//   // ssi0_selectDACs(bFalse); // Chip-Deselect
// }

// void ssi_receive(SSI0_Type *ssi, uint8_t *serializedStream, uint16_t *nBytes, uint16_t nMaxBytes)
// {
//   /* The received bytes comes in reversed order! */

//   int iCpyByte = nMaxBytes - 1; // CopyIndex for reversed order

//   for (; iCpyByte >= 0; iCpyByte--)
//   {
//     if (ssi0_RxFifoStatus() == ssi_FIFO_Empty)
//       break;

//     serializedStream[iCpyByte] = SSI0->DR;
//   }

//   *nBytesCopied = nMaxBytes - (iCpyByte + 1); // Set amount of copied bytes
// }
