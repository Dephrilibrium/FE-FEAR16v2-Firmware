#ifndef UART1_H
#define UART1_H

/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdint.h"

/* Project specific */
#include "helper.h"

/*******************************\
| Defines
\*******************************/
// UART
#define SYSCTL_RGCGUART_UART1 BIT(1)
#define UART1_CTL_EN BIT(0)

#define UART1_LCRH_BRK BIT(0)            // Send break
#define UART1_LCRH_PEN BIT(1)            // Enable parity
#define UART1_LCRH_EPS BIT(2)            // When set even parity is used, otherwise odd parity
#define UART1_LCRH_STP2 BIT(3)           // 2 Stopbits
#define UART1_LCRH_FEN BIT(4)            // Enable FIFOs
#define UART1_LCRH_WLEN5 OPTION(0x00, 5) // Datalen 5 bit
#define UART1_LCRH_WLEN6 OPTION(0x01, 5) // Datalen 6 bit
#define UART1_LCRH_WLEN7 OPTION(0x02, 5) // Datalen 7 bit
#define UART1_LCRH_WLEN8 OPTION(0x03, 5) // Datalen 8 bit
#define UART1_LCRH_SPS BIT(7)            // Send parity bit additionally

#define UART1_CC_CS_SYSCLK OPTION(0x00, 0) // Clocksource: SystemCoreClock
#define UART1_CC_CS_PIOSC OPTION(0x05, 0)  // Clocksource: PIOSC

#define UART1_FR_BUSY BIT(3) // UART1 Busy-Bit
#define UART1_FR_RXFE BIT(4) // UART1 RX-FIFO empty
#define UART1_FR_RXFF BIT(6) // UART1 RX-FIFO full
#define UART1_FR_TXFF BIT(5) // UART1 TX-FIFO full
#define UART1_FR_TXFE BIT(7) // UART1 TX-FIFO empty

#define UART1_DMACTL_RXDMAE BIT(0) // RX DMA Enable Bit
#define UART1_DMACTL_TXDMAE BIT(1) // TX DMA Enable Bit
#define UART1_DMACTL_DMAERR BIT(2) // Disable DMA Request on error

// GPIO
#define SYSCTL_RCGCGPIO_GPIOB BIT(1)
#define PB0_U1RX BIT(0)
#define PB1_U1TX BIT(1)
#define PB0_AF_U1RX OPTION(1, 0)
#define PB1_AF_U1TX OPTION(1, 4)

// Etc
#define UART1_BUFFERSIZE 256

/*******************************\
| Enum/Struct/Union
\*******************************/
enum UART1_Tx_Status
{
    UART1_Idle = 0x00,
    UART1_Busy = 0x01,
};
enum UART1_FIFO_Status
{
    UART1_FIFO_Empty = 0x00,
    UART1_FIFO_Full = 0x01,
    UART1_FIFO_WetherEmptyOrFull = 0x02,
};

/*******************************\
| Function declaration
\*******************************/
void uart1_init(uint32_t baud);
void uart1_setBaud(uint32_t baud);
uint32_t uart1_getBaud(void);
void uart1_enable(void);
void uart1_disable(void);

enum UART1_FIFO_Status uart1_RxFifoStatus(void);
enum UART1_FIFO_Status uart1_TxFifoStatus(void);
enum UART1_Tx_Status uart1_TxStatus(void);

void uart1_putc(char c);
void uart1_puts(char *cp);
char uart1_getc(void);
void uart1_gets(char *cp, uint32_t len);

#endif
