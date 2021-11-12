/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "TM4C123GH6PM.h"

/* Project specific */
#include "uart1.h"
#include "dma.h"
#include "sysclk.h"
#include "helper.h"

/*******************************\
| Local function declarations
\*******************************/
void uart1_changeBRD(uint32_t baud);

/*******************************\
| Global variables
\*******************************/
uint32_t uart1_baudrate = 0;
char RxBuffer[UART1_BUFFERSIZE];
char TxBuffer[UART1_BUFFERSIZE];

/*******************************\
| Function definitons
\*******************************/
void uart1_init(uint32_t baud)
{
    // Clocks
    SYSCTL->RCGCUART |= SYSCTL_RGCGUART_UART1; // Clock for UART1
    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_GPIOB; // Clock for GPIOB

    // IOs
    GPIOB->LOCK = 0x4C4F434B;                 // Write unlock-sequence
    GPIOB->CR = 0xFF;                         // Unlock all IOs
    GPIOB->AFSEL |= PB0_U1RX | PB1_U1TX;      // Select alternative function
    GPIOB->DR8R |= PB0_U1RX | PB1_U1TX;       // Enable 8mA driver
    GPIOB->SLR |= PB0_U1RX | PB1_U1TX;        // Enable Slewrate-Control (only when 8mA drivers are active!)
    GPIOB->PCTL |= PB0_AF_U1RX | PB1_AF_U1TX; // Configure UART1 as AF
    GPIOB->DEN |= PB0_U1RX | PB1_U1TX;        // Enable digital function

    // UART
    uart1_disable();
    uart1_changeBRD(baud);
    UART1->LCRH = UART1_LCRH_WLEN8;        // Datalength 8 bit
                                           //| UART1_LCRH_FEN;   // Enable FIFOs (not necessary when using DMA!)
    UART1->CC = UART1_CC_CS_SYSCLK;        // Select clocksource
    UART1->DMACTL = UART1_DMACTL_RXDMAE    // Enable RX DMA request
                    | UART1_DMACTL_TXDMAE; // Enable TX DMA request

    // DMA
    dma_uart1_init();

    uart1_enable();
}

void uart1_enable(void)
{
    UART1->CTL |= UART1_CTL_EN;
}

void uart1_disable(void)
{
    UART1->CTL &= ~UART1_CTL_EN;
}

void uart1_changeBRD(uint32_t baud)
{
    long sysclk = sys_clk_freq();
    double BRD = (double)sysclk / (16 * baud);
    uint32_t IBRD = (uint32_t)BRD;
    uint32_t FBRD = (uint32_t)((BRD - IBRD) * 64 + 0.5);

    UART1->IBRD = IBRD;
    UART1->FBRD = FBRD;

    uart1_baudrate = baud;
}

void uart1_setBaud(uint32_t baud)
{
    uart1_disable();
    uart1_changeBRD(baud);
    uart1_enable();
}

uint32_t uart1_getBaud(void)
{
    return uart1_baudrate;
}

enum UART1_FIFO_Status uart1_RxFifoStatus(void)
{
    uint32_t FR = UART1->FR;
    if (FR & UART1_FR_RXFE)
        return UART1_FIFO_Empty;
    if (FR & UART1_FR_RXFF)
        return UART1_FIFO_Full;

    return UART1_FIFO_WetherEmptyOrFull;
}

enum UART1_FIFO_Status uart1_TxFifoStatus(void)
{
    uint32_t FR = UART1->FR;
    if (FR & UART1_FR_TXFE)
        return UART1_FIFO_Empty;
    if (FR & UART1_FR_TXFF)
        return UART1_FIFO_Full;

    return UART1_FIFO_WetherEmptyOrFull;
}

enum UART1_Tx_Status uart1_TxStatus(void)
{
    return (enum UART1_Tx_Status)((UART1->FR >> 3) & 0x01);
}

void uart1_putc(char c)
{
    while (uart1_TxStatus() == UART1_Busy)
        ; // Wait until free space in TxFIFO
    UART1->DR = c;
}

void uart1_puts(char *cp)
{
    for (; *cp != '\0'; cp++)
        uart1_putc(*cp);
}

char uart1_getc(void)
{
    if (uart1_RxFifoStatus() != UART1_FIFO_Empty)
        return UART1->DR;

    return 0x00;
}

void uart1_gets(char *cp, uint32_t len)
{
    uint32_t cpIndex = 0;
    for (; cpIndex < len; cpIndex++)
    {
        cp[cpIndex] = uart1_getc();
    }

    cp[cpIndex] = '\0'; // Termiante
}
