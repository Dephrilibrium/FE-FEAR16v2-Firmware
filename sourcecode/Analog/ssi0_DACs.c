/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "math.h"

/* Project specific */
#include "TM4C123GH6PM.h"
#include "ssi0_DACs.h"
#include "dma.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Local Defines
// Clks
#define SYSCTL_RCGCSSI_SPI0 BIT(0)
#define SYSCTL_RCGCGPIO_GPIOA BIT(0)
#define SYSCTL_RCGCGPIO_GPIOB BIT(0)

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
#pragma region CR0 Datasize
#define SSI0_CR0_DATASIZE_4 OPTION(0x03, 0)  // 4-bit data
#define SSI0_CR0_DATASIZE_5 OPTION(0x04, 0)  // 5-bit data
#define SSI0_CR0_DATASIZE_6 OPTION(0x05, 0)  // 6-bit data
#define SSI0_CR0_DATASIZE_7 OPTION(0x06, 0)  // 7-bit data
#define SSI0_CR0_DATASIZE_8 OPTION(0x07, 0)  // 8-bit data
#define SSI0_CR0_DATASIZE_9 OPTION(0x08, 0)  // 9-bit data
#define SSI0_CR0_DATASIZE_10 OPTION(0x09, 0) // 10-bit data
#define SSI0_CR0_DATASIZE_11 OPTION(0x0A, 0) // 11-bit data
#define SSI0_CR0_DATASIZE_12 OPTION(0x0B, 0) // 12-bit data
#define SSI0_CR0_DATASIZE_13 OPTION(0x0C, 0) // 13-bit data
#define SSI0_CR0_DATASIZE_14 OPTION(0x0D, 0) // 14-bit data
#define SSI0_CR0_DATASIZE_15 OPTION(0x0E, 0) // 15-bit data
#define SSI0_CR0_DATASIZE_16 OPTION(0x0F, 0) // 16-bit data
#pragma endregion CR0 Datasize
#define SSI0_CR0_FREESCALE OPTION(0x0, 4) // Freescale SPI
#define SSI0_CR0_SPO BIT(6)               // Clock idle polarity (0 = low; 1 = high)
#define SSI0_CR0_SPH BIT(7)               // Data clock phase (0 = first clock edge; 1 = second clock edge)
#define SSI0_CR0_CLR OPTION(0xFF, 8)      // Bitmask for CR0 SCR
#define SSI0_CR0_SCR(x) OPTION(x, 8)      // SCR-factor

#pragma endregion Local Defines

/*******************************\
| Local function declarations
\*******************************/
void ssi0_changeClkRate(enum ssi0_clkRate clkRate);
void ssi0_enable(cBool state);

/*******************************\
| Global variables
\*******************************/
enum ssi0_clkRate _clkRate = ssi0_clkRate_1MHz; // Default 1 MHz
                                                // uint8_t DACBuffer[]

/*******************************\
| Function definitons
\*******************************/
void ssi0_init(enum ssi0_clkRate clkRate)
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

  // IO Enable
  GPIOA->DEN |= PA2_DACS_SCLK0   // Enable CLK
                | PA3_DACS_CS    // Enable ~CS
                | PA4_DACS_MISO0 // Enable MISO
                | PA5_DACS_MOSI0 // Enable MOSI
                | PA6_DACS_RST   // Enable ~RST
                | PA7_DACS_CLR;  // Enable ~CLR

  GPIOB->DEN |= PB4_DACS_LDAC; // Enable ~LDAC

  // SPI0
  ssi0_enable(bOff);        // Ensure SSI0 is off
  SSI0->CR1 &= SSI0_CR_MS;  // Make SSI0 Master
  SSI0->CC = SSI0_CC_PIOSC; // Use PIOSC (16MHz)
  ssi0_changeClkRate(clkRate);
  SSI0->CR0 |= SSI0_CR0_DATASIZE_8  // 8-bit data
               | SSI0_CR0_FREESCALE // No SPI format
               | SSI0_CR0_SPH;      // Take Data on second clock edge
  SSI0->CR0 &= ~SSI0_CR0_SPO;       // Clock Polarity low
  ssi0_enable(bOn);
}

void ssi0_setClkRate(enum ssi0_clkRate clkRate)
{
  ssi0_enable(bOff);
  ssi0_changeClkRate(clkRate);
  ssi0_enable(bOn);
}

void ssi0_changeClkRate(enum ssi0_clkRate clkRate)
{
  if (clkRate > PIOSC_MHZ)
    return;

  uint8_t CPSR;
  uint8_t SCR = 0; // Is 0 in all cases!

  switch (clkRate)
  {
  case ssi0_clkRate_125kHz:
    CPSR = 128;
    break;

  case ssi0_clkRate_250kHz:
    CPSR = 64;
    break;

  case ssi0_clkRate_500kHz:
    CPSR = 32;
    break;

  default:
    clkRate = ssi0_clkRate_1MHz;
    CPSR = 16;
    break;
  }

  _clkRate = clkRate;
  SSI0->CPSR = CPSR;
  SSI0->CR0 &= ~SSI0_CR0_CLR;
  SSI0->CR0 |= SSI0_CR0_SCR(SCR);
}

void ssi0_enable(cBool state)
{
  if (state == bTrue)
    SSI0->CR1 |= SSI0_CR1_SSE; // Enable SSI
  else
    SSI0->CR1 &= ~SSI0_CR1_SSE; // Disable SSI
}

void ssi0_selectDACs(cBool state)
{
  if (state)
    GPIOA->DATA &= ~PA3_DACS_CS;
  else
    GPIOA->DATA |= PA3_DACS_CS;
}

void ssi0_nTransmit(char *data)
{
}