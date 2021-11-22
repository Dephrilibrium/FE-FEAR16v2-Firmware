/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "TM4C123GH6PM.h"
#include "dma.h"
#include "uart1.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Local Defines

// CLKs
#define SYSCTL_RCGCDMA_DMA BIT(0) // DMA Clk
#define DMA_CFG_MASTEN BIT(0);    // Master enable

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
#define DMA_CHCTL_SRCINC_8 OPTION(0x00, 24)  // 8 bit
#define DMA_CHCTL_SRCINC_16 OPTION(0x01, 24) // 16 bit
#define DMA_CHCTL_SRCINC_32 OPTION(0x02, 24) // 32 bit
#define DMA_CHCTL_SRCINC_0 OPTION(0x03, 24)  // No increment
#pragma endregion Source Data Increment
#pragma region Source Destination Size        // Region contains destination data size defines
#define DMA_CHCTL_DSTSIZE_8 OPTION(0x00, 24)  // 8 bit
#define DMA_CHCTL_DSTSIZE_16 OPTION(0x01, 24) // 16 bit
#define DMA_CHCTL_DSTSIZE_32 OPTION(0x02, 24) // 32 bit
#pragma endregion Source Destination Size
#pragma region Source Destination Increment  // Region contains destination data increment defines
#define DMA_CHCTL_DSTINC_8 OPTION(0x00, 24)  // 8 bit
#define DMA_CHCTL_DSTINC_16 OPTION(0x01, 24) // 16 bit
#define DMA_CHCTL_DSTINC_32 OPTION(0x02, 24) // 32 bit
#define DMA_CHCTL_DSTINC_0 OPTION(0x03, 24)  // No increment
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
#pragma endregion Local Defines

/*******************************\
| Local function declarations
\*******************************/
void dma_clear_dmaCCT(void);

/*******************************\
| Global variables
\*******************************/
struct DMA_ChannelControlTable dmaCCT __attribute__((aligned(DMA_CCT_REGISTERCOUNT * DMA_CCT_REGISTER_BYTESIZE))); // Channel Control Table have to be aligned to 1024!

// uint32_t dma_RxCtrlWord; // Backup of the RX config-DWORD
// uint32_t dma_TxCtrlWord; // Backup of the TX config-DWORD

#pragma region Buffers
// Internal receive/send buffers
// charBuffer_t *_extRxBuffer;
// charBuffer_t *_extTxBuffer;
#pragma endregion Buffers

/*******************************\
| Function definitons
\*******************************/
#pragma region DMA common functions
void dma_uart1_init(void)
{
  // --- Common ---
  SYSCTL->RCGCDMA |= SYSCTL_RCGCDMA_DMA;
  dma_clear_dmaCCT(); // Wipe dmaCCT
  UDMA->CFG |= DMA_CFG_MASTEN;
  UDMA->CTLBASE = (uint32_t)&dmaCCT;

  /* --- Settings regarding UART1 ---
  * UART1 causes bus faults when DMA tries to copy from (by hardware-request) or to (by software-init) the UARTs DataRegister
  * This SHOULDN'T happen (also in ERATA is no error known!). In ERATA is one UART1 HW-Fault known regarding the IR-Mode
  *  --------------------------------
  */
  // UDMA->PRIOCLR |= DMA_CH8_U1RX; // Channel priority CH22
  // //UDMA->PRIOCLR |= DMA_CH23_U1TX;       // Channel priority CH23
  // UDMA->ALTCLR |= DMA_CH8_U1RX; // Channel 22 no alternative structure
  // //UDMA->ALTCLR |= DMA_CH23_U1TX;        // Channel 23 no alternative structure
  // UDMA->USEBURSTCLR |= DMA_CH8_U1RX; // Channel 23 no burstmode
  // //UDMA->USEBURSTCLR |= DMA_CH23_U1TX;   // Channel 23 no burstmode
  // UDMA->REQMASKCLR |= DMA_CH8_U1RX; // Channel 22 enable requests
  // //UDMA->REQMASKCLR |= DMA_CH23_U1TX;    // Channel 23 enable requests´
  // UDMA->CHMAP1 &= ~DMA_CHMAP_CLR(8);  // Bitmask to clear CH8 mappings
  // UDMA->CHMAP1 |= DMA_CHMAP_CH8_U1RX; // Bitmask to set CH8 to U1Rx
  // UDMA->CHMAP2 &= ~DMA_CHMAP_CLR(22); // Bitmask to clear/set CH22 to U1Rx
  //UDMA->CHMAP2 &= ~(DMA_CHMAP_CLR(23)); // Bitmask to clear/set CH23 to U1Tx
  //NVIC->ISER[1] |= NVIC_ISER1_DMA_HANDLE; // Enable global DMA finished IR handlingvoid dma_clear_dmaCCT(void);
  //NVIC->ISER[1] |= NVIC_ISER1_DMA_ERROR;  // Enable global DMA error IR handling
}

void dma_clear_dmaCCT(void)
{
  uint32_t copyBytes = sizeof(dmaCCT) / 4;
  uint32_t *ptr_dmaCCT = (uint32_t *)&dmaCCT;

  for (int nIndex = 0; nIndex < copyBytes; nIndex++)
    ptr_dmaCCT[nIndex] = 0;
}
#pragma endregion DMA common functions

#pragma region UART1 specific DMA functions
// enum dmaE dma_uart1_ConfigRxBuffer(charBuffer_t *rxBuffer)
// {
//   /* Guard clauses */
//   if (rxBuffer == NULL)
//     return dmaE_ArgNULL;

//   if (rxBuffer->Size > DMA_MAXIMUM_TRANSFERS)
//     return dmaE_MaxTransferExceeded;
//   /* ------------- */

//   _extRxBuffer = rxBuffer; // Copy for instantaneous updating the RxBuffer of UART1

//   dmaCCT.CH8P.DestinationEndPointer = (uint32_t *)(_extRxBuffer->Buffer + _extRxBuffer->Size - 1);
//   dmaCCT.CH8P.SourceEndPointer = (uint32_t *)&(UART1->DR);
//   // dmaCCT.CH22P.DestinationEndPointer = (uint32_t *)(_extRxBuffer->Buffer + _extRxBuffer->Size - 1);
//   // dmaCCT.CH22P.SourceEndPointer = (uint32_t *)&(UART1->DR);

//   dma_RxCtrlWord = DMA_CHCTL_XFERMODE_BASIC;                    // Basic transfermode
//   dma_RxCtrlWord |= DMA_CHCTL_NXTUSEBURST(0);                   // No burst on transfer
//   dma_RxCtrlWord |= DMA_CHCTL_XFERSIZE(_extRxBuffer->Size - 1); // Maximum byte-transfers = RxBuffer-Size
//   dma_RxCtrlWord |= DMA_CHCTL_ARBSIZE_1;                        // Burst sends 1 transfers
//   dma_RxCtrlWord |= DMA_CHCTL_SRCSIZE_8;                        // Source data size 8 bit
//   dma_RxCtrlWord |= DMA_CHCTL_SRCINC_0;                         // Source data increment: none
//   dma_RxCtrlWord |= DMA_CHCTL_DSTSIZE_8;                        // Source destination size 8 bit
//   dma_RxCtrlWord |= DMA_CHCTL_DSTINC_8;                         // Source destination increment: 8 bit
//   // dma_RxCtrlWord = DMA_CHCTL_XFERMODE_BASIC                     // Basic transfermode
//   //                  | DMA_CHCTL_NXTUSEBURST(0)                   // No burst on transfer
//   //                  | DMA_CHCTL_XFERSIZE(_extRxBuffer->Size - 1) // Maximum byte-transfers = RxBuffer-Size
//   //                  | DMA_CHCTL_ARBSIZE_1                        // Burst sends 1 transfers
//   //                  | DMA_CHCTL_SRCSIZE_8                        // Source data size 8 bit
//   //                  | DMA_CHCTL_SRCINC_0                         // Source data increment: none
//   //                  | DMA_CHCTL_DSTSIZE_8                        // Source destination size 8 bit
//   //                  | DMA_CHCTL_DSTINC_8;                        // Source destination increment: 8 bit
//   dmaCCT.CH8P.ControlWord = dma_RxCtrlWord; // Put initial standard-config to CH22P
//   // dmaCCT.CH22P.ControlWord = dma_RxCtrlWord; // Put initial standard-config to CH22P

//   // UDMA->ENASET |= DMA_CH22_U1RX;
//   UDMA->ENASET |= DMA_CH8_U1RX;
//   return dmaE_Ok;
// }

// enum dmaE dma_uart1_ConfigTxBuffer(charBuffer_t *txBuffer)
// {
//   /* Guard clauses */
//   if (txBuffer == NULL)
//     return dmaE_ArgNULL;

//   if (txBuffer->Size > DMA_MAXIMUM_TRANSFERS)
//     return dmaE_MaxTransferExceeded;
//   /* ------------- */

//   _extTxBuffer = txBuffer;

//   dmaCCT.CH23P.DestinationEndPointer = (uint32_t *)&(UART1->DR);
//   dmaCCT.CH23P.SourceEndPointer = (uint32_t *)(_extTxBuffer->Buffer + _extTxBuffer->Size - 1); // Must be re-configurated when transmitting
//   dma_TxCtrlWord = DMA_CHCTL_XFERMODE_BASIC                                                    // Basic transfermode
//                    | DMA_CHCTL_NXTUSEBURST(0)                                                  // No burst on transfer
//                    | DMA_CHCTL_XFERSIZE(0)                                                     // Keep unpredefined: Tx-Transfers depends on sendbuffer content!
//                    | DMA_CHCTL_ARBSIZE_1                                                       // Burst sends 1 transfers
//                    | DMA_CHCTL_SRCSIZE_8                                                       // Source data size 8 bit
//                    | DMA_CHCTL_SRCINC_8                                                        // Source data increment: 8 bit
//                    | DMA_CHCTL_DSTSIZE_8                                                       // Source destination size 8 bit
//                    | DMA_CHCTL_DSTINC_0;                                                       // Source destination increment: none
//   dmaCCT.CH23P.ControlWord = dma_TxCtrlWord;                                                   // Put initial standard-config to CH22P

//   return dmaE_Ok;
// }

// uint16_t dma_uart1_GetRxXFERSIZE(void)
// {
//   uint32_t cw = dmaCCT.CH22P.ControlWord;
//   cw >>= DMA_CHCTL_XFERSIZE_OFFSET;
//   cw &= 0x3FF;

//   return (dmaCCT.CH22P.ControlWord >> DMA_CHCTL_XFERSIZE_OFFSET) & 0x3FF; // XFERSIZE is 2^10 bit -> 0x3FF
// }

// uint32_t *dma_uart1_GetRxHead(void)
// {
//   return (uint32_t *)(dmaCCT.CH22P.DestinationEndPointer - dma_uart1_GetRxXFERSIZE());
// }

// enum dmaE dma_uart1_TxTransmit(void)
// {
//   if (_extTxBuffer->Filled == 0)
//     return dmaE_TransmitCountZero;

//   if (_extTxBuffer->Filled > DMA_MAXIMUM_TRANSFERS)
//     return dmaE_MaxTransferExceeded;

//   dmaCCT.CH23P.SourceEndPointer = (uint32_t *)(_extTxBuffer->Buffer + _extTxBuffer->Filled - 1); // Recalculate new End-Pointer
//   dmaCCT.CH23P.ControlWord = dma_TxCtrlWord                                                      // Regular standard-configuration
//                              | DMA_CHCTL_XFERSIZE(_extTxBuffer->Filled - 1);                     // Including amount of transfers

//   UDMA->ENASET |= DMA_CH9_U1TX;
//   // UDMA->ENASET |= DMA_CH23_U1TX;
//   return dmaE_Ok;
// }

#pragma endregion UART1 specific DMA functions

#pragma region IR Handling
// void uDMA_Handler(void)
// {
//   static int i = 0;
//   i++;
// }

// void uDMA_Error(void)
// {
//   static int i = 0;
//   i++;
// }
#pragma endregion IR Handling
