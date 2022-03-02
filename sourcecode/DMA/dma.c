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
// CLKs
#define SYSCTL_RCGCDMA_DMA BIT(0) // DMA Clk
#define DMA_CFG_MASTEN BIT(0);    // Master enable

/*******************************\
| Local function declarations
\*******************************/
void dma_clear_dmaCCT(void);

/*******************************\
| Global variables
\*******************************/
cBool _dmaAlreadyInitialized = bFalse;
DMA_ChannelControlTable _dmaCCT __attribute__((aligned(DMA_CCT_ALIGNBOUNDARY))); // Channel Control Table have to be aligned to 1024!

uint32_t _dmaInDummies[DMA_CCT_CHANNELS] = {0};
uint32_t _dmaOutDummies[DMA_CCT_CHANNELS] = {0};

#pragma region Buffers

#pragma endregion Buffers

/*******************************\
| Function definitons
\*******************************/
void dma_init(void)
{
  if (_dmaAlreadyInitialized)
    return;

  _dmaAlreadyInitialized = bTrue;
  SYSCTL->RCGCDMA |= SYSCTL_RCGCDMA_DMA;
  UDMA->CFG = DMA_CFG_MASTEN;

  UDMA->CTLBASE = (uint32_t)&_dmaCCT;
  // UDMA->REQMASKSET = (uint32_t)-1; // Disable all channels by default!
}

DMA_ChannelControlTable *dma_getCCT(void)
{
  return &_dmaCCT;
}

DMA_Channel_Structure *dma_getCCTChannel(enum dmaChannelStructure cctChannel)
{
  return &_dmaCCT.ChannelsAsArray[cctChannel];
}
