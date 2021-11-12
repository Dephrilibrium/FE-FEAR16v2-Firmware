/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "TM4C123GH6PM.h"

/* Project specific */
#include "dma.h"
#include "uart1.h"

/*******************************\
| Local function declarations
\*******************************/

/*******************************\
| Global variables
\*******************************/
struct DMA_ChannelControlTable dmaCCT __attribute__((aligned(DMA_CCT_REGISTERCOUNT * DMA_CCT_REGISTER_BYTESIZE))); // Channel Control Table have to be aligned to 1024!
uint32_t dma_RxCtrlWord;                                                                                           // Backup of the RX config-DWORD
uint32_t dma_TxCtrlWord;                                                                                           // Backup of the TX config-DWORD

extern char RxBuffer[UART1_BUFFERSIZE];
extern char TxBuffer[UART1_BUFFERSIZE];

/*******************************\
| Function definitons
\*******************************/
#pragma region DMA common functions
void dma_uart1_init(void)
{
  // --- Common ---
  SYSCTL->RCGCDMA |= SYSCTL_RCGCDMA_DMA;
  UDMA->CFG |= DMA_CFG_MASTEN;
  UDMA->CTLBASE = &dmaCCT;

  // --- Settings regarding UART1 ---
  UDMA->PRIOCLR |= DMA_CH22_U1RX | DMA_CH23_U1TX;     // Channel priority
  UDMA->ALTCLR |= DMA_CH22_U1RX | DMA_CH23_U1TX;      // Channel 8 no alternative structure
  UDMA->USEBURSTCLR |= DMA_CH22_U1RX | DMA_CH23_U1TX; // Channel 8 no burstmode
  UDMA->REQMASKCLR |= DMA_CH22_U1RX | DMA_CH23_U1TX;  // Channel 8 enable requests
                                                      // Rx-Buffer
  dmaCCT.CH22P.DestinationEndPointer = (RxBuffer + UART1_BUFFERSIZE - 1);
  dmaCCT.CH22P.SourceEndPointer = UART1->DR;
  dma_RxCtrlWord = DMA_CHCTL_XFERMODE_BASIC               // Basic transfermode
                   | DMA_CHCTL_NXTUSEBURST                // Use burst on next transfer
                   | DMA_CHCTL_XFERSIZE(UART1_BUFFERSIZE) // Maximum byte-transfers = RxBuffer-Size
                   | DMA_CHCTL_ARBSIZE_8                  // Burst sends 8 transfers
                   | DMA_CHCTL_SRCSIZE_8                  // Source data size 8 bit
                   | DMA_CHCTL_SRCINC_0                   // Source data increment: none
                   | DMA_CHCTL_DSTSIZE_8                  // Source destination size 8 bit
                   | DMA_CHCTL_DSTINC_8;                  // Source destination increment: 8 bit
                                                          // Tx-Buffer
  dmaCCT.CH22P.DestinationEndPointer = UART1->DR;
  dmaCCT.CH22P.SourceEndPointer = (TxBuffer + UART1_BUFFERSIZE - 1);
  dma_TxCtrlWord = DMA_CHCTL_XFERMODE_BASIC // Basic transfermode
                   | DMA_CHCTL_NXTUSEBURST  // Use burst on next transfer
                   | DMA_CHCTL_XFERSIZE(0)  // Keep unpredefined: Tx-Transfers depends on sendbuffer content!
                   | DMA_CHCTL_ARBSIZE_8    // Burst sends 8 transfers
                   | DMA_CHCTL_SRCSIZE_8    // Source data size 8 bit
                   | DMA_CHCTL_SRCINC_0     // Source data increment: none
                   | DMA_CHCTL_DSTSIZE_8    // Source destination size 8 bit
                   | DMA_CHCTL_DSTINC_8;    // Source destination increment: 8 bit
}
#pragma endregion DMA common functions

#pragma region UART1 specific DMA functions
void dma_uart1_RxReset(void)
{
  dmaCCT.CH22P.ControlWord = dma_RxCtrlWord;
}

void dma_uart1_TxSend(char *string, int count)
{
  char *dma_bufferTail = dmaCCT.CH22P.SourceEndPointer;
  char *dma_bufferOrigin = dma - count;
  for(

  dmaCCT.CH22P.ControlWord = dma_RxCtrlWord                   // Basic control word
                             | DMA_CHCTL_XFERSIZE(count - 1); // Include transfer-size
}
#pragma endregion UART1 specific DMA functions
