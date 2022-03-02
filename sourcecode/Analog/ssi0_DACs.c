/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "math.h"
#include "stdint.h"

/* Project specific */
#include "TM4C123GH6PM.h"
#include "ssi0_DACs.h"
#include "ssiCommon.h"
#include "dma.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Local Defines
// Clks
#define SYSCTL_RCGCSSI_SPI0 BIT(0)
#define SYSCTL_RCGCGPIO_GPIOA BIT(0)
#define SYSCTL_RCGCGPIO_GPIOB BIT(1)

/* GPIOs */
// GPIOA
#define PA2_DACS_SCLK0 BIT(2) // SPI0 Clk
#define PA3_DACS_CS BIT(3)    // ~Chip-Select of DAC (low-Active)
#define PA4_DACS_MISO0 BIT(4) // SPI0 MISO
#define PA5_DACS_MOSI0 BIT(5) // SPI0 MOSI
#define PA6_DACS_RST BIT(6)   // ~Reset of DAC (low-active)
#define PA7_DACS_CLR BIT(7)   // ~Clear of DAC (low-active)

#define PB4_DACS_LDAC BIT(4) // ~Low-Synchronization-Signal of DAC (low-active)

#define GPIO_PCTL_CLEAR(x) OPTION(0xf, x * 4)       // Each bitfield has 4 bits
#define GPIOA_PCTL_PA2_SPI0CLK OPTION(0x02, 2 * 4)  // PCTL for PA2 = SPI0-CLK
#define GPIOA_PCTL_PA4_SPI0MISO OPTION(0x02, 4 * 4) // PCTL for PA2 = SPI0-MISO
#define GPIOA_PCTL_PA5_SPI0MOSI OPTION(0x02, 5 * 4) // PCTL for PA2 = SPI0-MOSI

/* SPI0 */
#define SSI0_CR1_SSE BIT(1)            // SSI Enable
#define SSI0_CR_MS BIT(2)              // Master (clear) / Slave (set)
#define SSI0_CC_SYSCTL OPTION(0x00, 0) // Make CoreClk to ClockSource (can depend on clock-settings!)
#define SSI0_CC_PIOSC OPTION(0x05, 0)  // Make PIOSC (Precise Internal Oscillator = 16MHz) to ClockSource
// #pragma region CR0 Datasize
// #define SSI0_CR0_DATASIZE_4 OPTION(0x03, 0)  // 4-bit data
// #define SSI0_CR0_DATASIZE_5 OPTION(0x04, 0)  // 5-bit data
// #define SSI0_CR0_DATASIZE_6 OPTION(0x05, 0)  // 6-bit data
// #define SSI0_CR0_DATASIZE_7 OPTION(0x06, 0)  // 7-bit data
// #define SSI0_CR0_DATASIZE_8 OPTION(0x07, 0)  // 8-bit data
// #define SSI0_CR0_DATASIZE_9 OPTION(0x08, 0)  // 9-bit data
// #define SSI0_CR0_DATASIZE_10 OPTION(0x09, 0) // 10-bit data
// #define SSI0_CR0_DATASIZE_11 OPTION(0x0A, 0) // 11-bit data
// #define SSI0_CR0_DATASIZE_12 OPTION(0x0B, 0) // 12-bit data
// #define SSI0_CR0_DATASIZE_13 OPTION(0x0C, 0) // 13-bit data
// #define SSI0_CR0_DATASIZE_14 OPTION(0x0D, 0) // 14-bit data
// #define SSI0_CR0_DATASIZE_15 OPTION(0x0E, 0) // 15-bit data
// #define SSI0_CR0_DATASIZE_16 OPTION(0x0F, 0) // 16-bit data
// #pragma endregion CR0 Datasize
#define SSI0_CR0_FREESCALE OPTION(0x0, 4) // Freescale SPI
#define SSI0_CR0_SPO BIT(6)               // Clock idle polarity (0 = low; 1 = high)
#define SSI0_CR0_SPH BIT(7)               // Data clock phase (0 = first clock edge; 1 = second clock edge)
#define SSI0_CR0_CLR OPTION(0xFF, 8)      // Bitmask for CR0 SCR
#define SSI0_CR0_SCR(x) OPTION(x, 8)      // SCR-factor

#define SSI0_SR_TFE BIT(0)  // Tx FIFO Empty
#define SSI0_SR_TNF BIT(1)  // Tx FIFO Not Full
#define SSI0_SR_RNE BIT(2)  // Rx FIFO Not Empty
#define SSI0_SR_RFF BIT(3)  // Rx FIFO Full
#define SSI0_SR_BUSY BIT(4) // SSI0 Sending (busy-bit)

// DMA
#define SSI0_UDMA_RXDMAEN BIT(0)
#define SSI0_UDMA_TXDMAEN BIT(1)

#pragma endregion Local Defines

/*******************************\
| Local Enum/Struct/Union
\*******************************/
struct ssi0_serializedDACData
{
  uint8_t Size;
  uint8_t Filled;

  union
  {
    uint8_t SerializedData[SSI0_BUFFERSIZE];
    struct __attribute__((packed))
    { // Due to sending cmd byte is the first data shifted out, which ends at the last DAC!
      uint8_t CmdByte1;
      uint16_t DataWord1;
      uint8_t CmdByte0;
      uint16_t DataWord0;
    };
  };
};
typedef struct ssi0_serializedDACData ssi0_dacData_t;

/*******************************\
| Local function declarations
\*******************************/
// void ssi0_changeClkRate(enum ssi_clkRate clkRate); // Now in ssiCommon
// void ssi0_enable(cBool state);

/*******************************\
| Global variables
\*******************************/
enum ssi_clkRate _ssi0_clkRate = ssi_clkRate_1MHz; // Default 1 MHz
ssi0_dacData_t _serializedOutput = {.Size = SSI0_BUFFERSIZE, .Filled = 0};
ssi0_dacData_t _serializedInput = {.Size = SSI0_BUFFERSIZE, .Filled = 0};

/*******************************\
| Function definitons
\*******************************/
void ssi0_init(enum ssi_clkRate clkRate)
{
  SYSCTL->RCGCSSI |= SYSCTL_RCGCSSI_SPI0;      // SPI0-CLK
  SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_GPIOA    // GPIOA-CLK
                      | SYSCTL_RCGCGPIO_GPIOB; // GPIOB-CLK

  // IOs
  // Unlock pins
  GPIOA->LOCK = 0x4C4F434B; // Unlock CR
  GPIOA->CR = 0xFF;         // Unlock all pins

  GPIOB->LOCK = 0x4C4F434B; // Unlock CR
  GPIOB->CR = 0xFF;         // Unlock all pins

  // Setup AFSEL
  GPIOA->AFSEL |= PA2_DACS_SCLK0     // SPI0-Clock
                  | PA4_DACS_MISO0   // Master In Slave Out
                  | PA5_DACS_MOSI0;  // Master Out Slave In
  GPIOA->AFSEL &= ~(PA3_DACS_CS      // ~Chipselect for DACs(not switched by SPI itself)
                    | PA6_DACS_RST   // ~Reset for DACs
                    | PA7_DACS_CLR); // ~Clear for DACs

  GPIOB->AFSEL &= ~PB4_DACS_LDAC; //~LDAC of DACs

  // Change PCTL
  GPIOA->PCTL &= ~(GPIO_PCTL_CLEAR(2)       // Clear PA2 (CLK)
                   | GPIO_PCTL_CLEAR(3)     // Clear PA3 (~CS)
                   | GPIO_PCTL_CLEAR(4)     // Clear PA4 (MISO)
                   | GPIO_PCTL_CLEAR(5)     // Clear PA5 (MOSI)
                   | GPIO_PCTL_CLEAR(6)     // Clear PA6 (~RST)
                   | GPIO_PCTL_CLEAR(7));   // Clear PA7 (~CLR)
  GPIOA->PCTL |= GPIOA_PCTL_PA2_SPI0CLK     // PA2 = CLK
                 | GPIOA_PCTL_PA4_SPI0MISO  // PA4 = MISO
                 | GPIOA_PCTL_PA5_SPI0MOSI; // PA5 = MOSI

  GPIOB->PCTL &= ~GPIO_PCTL_CLEAR(4); // Clear PB4 (~LDAC)

  // Make output where necessary
  GPIOA->DIR |= PA3_DACS_CS     // Make ~Chip-Select to output
                | PA6_DACS_RST  // Make ~Reset to output
                | PA7_DACS_CLR; // Make ~CLR to output

  GPIOB->DIR |= PB4_DACS_LDAC; // Make ~LDAC to output

  // IO Enable
  GPIOA->DEN |= PA2_DACS_SCLK0 // Enable CLK
                | PA3_DACS_CS  // Enable ~CS
                // | PA4_DACS_MISO0 // Enable MISO
                // | PA5_DACS_MOSI0 // Enable MOSI
                | PA6_DACS_RST  // Enable ~RST
                | PA7_DACS_CLR; // Enable ~CLR
  ssi0_RxOnOff(bOn);            // Using the preparated DEN-functions for the MISO-Pins
  ssi0_TxOnOff(bOn);            // Using the preparated DEN-functions for the MOSI-Pins

  GPIOB->DEN |= PB4_DACS_LDAC; // Enable ~LDAC

  // Prepare DAC for SSI0-Init
  ssi0_rstDACs(bTrue);     // Set DAC to reset while init ssi0
  ssi0_ldacDACs(bFalse);   // No synchrone output by default
  ssi0_selectDACs(bFalse); // Deselect DACs
  ssi0_clrDACs(bFalse);    // Take back output clear

  // SPI0
  ssi_enable(SSI0, bOff);           // Ensure SSI0 is off
  SSI0->CR1 &= SSI0_CR_MS;          // Make SSI0 Master
  SSI0->CC = SSI0_CC_PIOSC;         // Use PIOSC (16MHz)
  ssi_changeClkRate(SSI0, clkRate); // Just change! Do not use set-method!
  ssi_changeDataSize(SSI0, ssiDataSize_8bit);
  SSI0->CR0 |= SSI0_CR0_FREESCALE // No SPI format
                                  //  | SSI0_CR0_SPO;      // Clock Polarity high
               | SSI0_CR0_SPH;    // Take Data on second clock edge
  SSI0->CR0 &= ~SSI0_CR0_SPO;     // Clock Polarity low
  SSI0->IM |= BIT(2)              // Local rx-Interrupts
              | BIT(3);           // Local tx-Interrupts

  // DMA Configuration
  dma_init();
  SSI0->DMACTL |= SSI0_UDMA_RXDMAEN | SSI0_UDMA_TXDMAEN;
  uint32_t chBitMask = BIT(SSI0_RX_DMA_CHNUM) | BIT(SSI0_TX_DMA_CHNUM);
  UDMA->PRIOCLR = chBitMask;
  UDMA->ALTCLR = chBitMask;
  UDMA->USEBURSTCLR = chBitMask;
  UDMA->REQMASKCLR = chBitMask;
  UDMA->CHMAP1 &= ~(OPTION(0xF, 8) | OPTION(0xF, 12));
  UDMA->CHMAP1 |= OPTION(0x0, 8)     // Map CH10 to SSI0 Rx
                  | OPTION(0x0, 12); // Map Ch11 to SSI0 Tx

  ssi_enable(SSI0, bOn);

  // Prepare DAC for regular use
  ssi0_rstDACs(bFalse); // Stop resetting DAC
  // ssi0_clrDACs(bFalse);    // Already off
  // ssi0_selectDACs(bFalse); // Already off
  // ssi0_ldacDACs(bFalse);   // Already disabled sychronous output

  ssi_clearRxFIFO(SSI0); // Be sure nothing undefined is stored in RxFIFO during the init-process
}

void ssi0_setClkRate(enum ssi_clkRate clkRate)
{
  ssi_enable(SSI0, bOff);
  ssi_changeClkRate(SSI0, clkRate);
  _ssi0_clkRate = clkRate;
  ssi_enable(SSI0, bOn);
}

// void ssi0_changeClkRate(enum ssi_clkRate clkRate)
// {
//   if (clkRate > PIOSC_MHZ)
//     return;

//   uint8_t CPSR;
//   uint8_t SCR = 0; // Is 0 in all cases!

//   switch (clkRate)
//   {
//   case ssi_clkRate_125kHz:
//     CPSR = 128;
//     break;

//   case ssi_clkRate_250kHz:
//     CPSR = 64;
//     break;

//   case ssi_clkRate_500kHz:
//     CPSR = 32;
//     break;

//   default:
//     clkRate = ssi_clkRate_1MHz;
//     CPSR = 16;
//     break;
//   }

//   _clkRate = clkRate;
//   SSI0->CPSR = CPSR;
//   SSI0->CR0 &= ~SSI0_CR0_CLR;
//   SSI0->CR0 |= SSI0_CR0_SCR(SCR);
// }

// enum ssi_sendingStatus ssi0_SendindStatus(void)
// {
//   if (SSI0->SR & SSI0_SR_BUSY)
//     return ssi_sending_busy;

//   return ssi_sending_idle;
// }

// enum ssi_FIFOStatus ssi0_RxFifoStatus(void)
// {
//   uint32_t SR = SSI0->SR;

//   /* KEEP IN MIND!
//    * When Debugging the Debugger reads out the SSI0-DR Register!
//    * Therefore sometimes it can be that the RxFIFO Empty bit can
//    *  disappear for "no reason"!
//    */

//   if (SR & SSI0_SR_RFF) // Check receive-FIFO Full
//     return ssi_FIFO_Full;
//   else if (SR & SSI0_SR_RNE) // Check receive FIFO not empty
//     return ssi_FIFO_NeitherEmptyOrFull;

//   // If both not fullfilled FIFO is empty
//   return ssi_FIFO_Empty;
// }

// enum ssi_FIFOStatus ssi0_TxFifoStatus(void)
// {
//   uint32_t SR = SSI0->SR;

//   if (SR & SSI0_SR_TFE) // Check transmit-FIFO empty
//     return ssi_FIFO_Empty;
//   else if (SR & SSI0_SR_TNF) // Check transmit-FIFO not full
//     return ssi_FIFO_NeitherEmptyOrFull;

//   // If both not fullfilled FIFO is full
//   return ssi_FIFO_Full;
// }

// void ssi0_enable(cBool state)
// {
//   if (state == bTrue)
//     SSI0->CR1 |= SSI_CR1_SSE; // Enable SSI
//   else
//     SSI0->CR1 &= ~SSI_CR1_SSE; // Disable SSI
// }

void ssi0_selectDACs(cBool state)
{
  if (state)
    GPIOA->DATA &= ~PA3_DACS_CS;
  else
    GPIOA->DATA |= PA3_DACS_CS;
}

void ssi0_ldacDACs(cBool state)
{
  if (state)
    GPIOB->DATA &= ~PB4_DACS_LDAC;
  else
    GPIOB->DATA |= PB4_DACS_LDAC;
}

void ssi0_clrDACs(cBool state)
{
  if (state)
    GPIOA->DATA &= ~PA7_DACS_CLR;
  else
    GPIOA->DATA |= PA7_DACS_CLR;
}

void ssi0_rstDACs(cBool state)
{
  if (state)
    GPIOA->DATA &= ~PA6_DACS_RST;
  else
    GPIOA->DATA |= PA6_DACS_RST;
}

void ssi0_RxOnOff(cBool ioOnOff)
{
  if (ioOnOff)
    GPIOA->DEN |= PA4_DACS_MISO0;
  else
    GPIOA->DEN &= ~PA4_DACS_MISO0;
}

void ssi0_TxOnOff(cBool ioOnOff)
{
  if (ioOnOff)
    GPIOA->DEN |= PA5_DACS_MOSI0;
  else
    GPIOA->DEN &= ~PA5_DACS_MOSI0;
}

/* ******** Shifted as common method to ssiCommon ******** */
// void ssi0_transmit(uint8_t *serializedStream, uint16_t bytes_n)
// {
//   // Has to be managed by the user!
//   // ssi0_selectDACs(bTrue); // Chip-Select

//   for (int iByte = bytes_n - 1; iByte >= 0; iByte--) // Use underflow-trick to check "-1"
//   {
//     while (ssi0_TxFifoStatus() == ssi_FIFO_Full)
//       ; // Wait when FIFO is full!

//     SSI0->DR = serializedStream[iByte];
//   }

//   // while (ssi0_SendindStatus() == ssi_sending_busy)
//   //   ;                      // Wait for send finished
//   // ssi0_selectDACs(bFalse); // Chip-Deselect
// }

// void ssi0_receive(uint8_t *serializedStream, uint16_t *nBytesCopied, uint16_t nMaxBytes)
// {
//   /* The received bytes comes in reversed order! */

//   int iCpyByte = nMaxBytes - 1; // CopyIndex for reversed order

//   for (; iCpyByte >= 0; iCpyByte--)
//   {
//     if (ssi0_RxFifoStatus() == ssi_FIFO_Empty)
//       break;

//     serializedStream[iCpyByte] = (uint8_t)SSI0->DR;
//   }

//   // Sometimes the RxFifo stores a byte more than expected. This shifts the entire response into wrong positions!
//   // Workaround: Clear RxFIFO after each read
//   ssi0_clearRxFIFO();

//   *nBytesCopied = nMaxBytes - (iCpyByte + 1); // Set amount of copied bytes
// }

// void ssi0_clearRxFIFO(void)
// {
//   while (ssi0_RxFifoStatus() != ssi_FIFO_Empty) // Force clearing RxFIFO
//     SSI0->DR;
// }
/* ------------------------------------------------------- */
