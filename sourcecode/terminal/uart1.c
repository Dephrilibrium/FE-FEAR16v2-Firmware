/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "TM4C123GH6PM.h"
#include "uart1.h"
// #include "cString.h"
#include "dma.h"
#include "sysclk.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Hardwaredefines
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

#define UART1_IM_RX BIT(4) // Rx Interrupt Mask
#define UART1_IM_TX BIT(5) // Tx Interrupt Mask

#define UART1_DMACTL_RXDMAE BIT(0) // RX DMA Enable Bit
#define UART1_DMACTL_TXDMAE BIT(1) // TX DMA Enable Bit
#define UART1_DMACTL_DMAERR BIT(2) // Disable DMA Request on error

#define NVIC_ISER0_U1 BIT(6) // Bit for global NVIC-Interrupt

// GPIO
#define SYSCTL_RCGCGPIO_GPIOB BIT(1)
#define PB0_U1RX BIT(0)
#define PB1_U1TX BIT(1)
#define PB0_AF_U1RX OPTION(1, 0)
#define PB1_AF_U1TX OPTION(1, 4)
#pragma endregion Hardwaredefines

/*******************************\
| Local function declarations
\*******************************/
void uart1_changeBRD(uint32_t baud);
void uart1_putc(const char c);

/*******************************\
| Global variables
\*******************************/
uint32_t uart1_baudrate = 0;

/*******************************\
| Function definitons
\*******************************/
void uart1_init(uint32_t baud)
{
    /* UART1 can't handle DMA-Requests */
    // Trying to request DMA-Copy led to dma-bus-error
    // Have to use FIFOs and polling!

    // Clocks
    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_GPIOB; // Clock for GPIOB
    SYSCTL->RCGCUART |= SYSCTL_RGCGUART_UART1; // Clock for UART1

    // IOs
    // GPIOB->LOCK = 0x4C4F434B; // Write unlock-sequence
    // GPIOB->CR = 0xFF;         // Unlock all IOs
    GPIOB->AFSEL |= PB0_U1RX    // Enable AF PB0 U1RX
                    | PB1_U1TX; // Enable AF PB1 U1TX
    // GPIOB->DR8R |= PB0_U1RX     // Enable 8mA driver
    //                | PB1_U1TX;  // Enable 8mA driver
    // GPIOB->SLR |= PB0_U1RX      // Enable Slewrate-Control (only when 8mA drivers are active!)
    //               | PB1_U1TX;   // Enable Slewrate-Control (only when 8mA drivers are active!)
    GPIOB->PCTL |= PB0_AF_U1RX    // Configure UART1 as AF
                   | PB1_AF_U1TX; // Configure UART1 as AF
    GPIOB->DEN |= PB0_U1RX        // Enable digital function
                  | PB1_U1TX;     // Enable digital function

    // DMA runs into bus-error when UART1 tries to request copy
    // dma_uart1_init();
    // dma_uart1_ConfigRxBuffer(&_RxBuffer);
    //dma_uart1_ConfigTxBuffer(&_TxBuffer);

    // UART
    uart1_disable();
    uart1_changeBRD(baud);
    UART1->LCRH |= UART1_LCRH_WLEN8  // Datalength 8 bit
                   | UART1_LCRH_FEN; // Enable FIFOs (not necessary when using DMA!)
    UART1->CC = UART1_CC_CS_SYSCLK;  // Select clocksource
    //L_RXDMAE; // Enable RX DMA request
    // UART1->DMACTL |= UART1_DMACTL_RXDMAE    // Enable TX DMA request
    //                  | UART1_DMACTL_TXDMAE; // Enable TX DMA request
    // UART1->IM |= UART1_IM_RX    // Enable Rx Interrupt Mask
    //              | UART1_IM_TX; // Enable Tx Interrupt Mask
    uart1_enable();

    // Global Interrupt - Get's polled now!
    // NVIC->ISER[0] |= NVIC_ISER0_U1; // Enable global IR
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
    // long baseclk = PIOSC_MHZ;
    long baseclk = sys_clk_freq();
    double BRD = (double)baseclk / (16 * baud);
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

enum UART1_SendingStatus uart1_SendingStatus(void)
{
    if (UART1->FR &= UART1_FR_BUSY)
        return UART1_Sending_Busy;

    return UART1_Sending_Idle;
}

enum UART1_FIFO_Status uart1_RxFifoStatus(void)
{
    uint32_t FR = UART1->FR;

    if (FR &= UART1_FR_RXFE)
        return UART1_FIFO_Empty;
    if (FR &= UART1_FR_RXFF)
        return UART1_FIFO_Full;

    return UART1_FIFO_NeitherEmptyOrFull;
}

enum UART1_FIFO_Status uart1_TxFifoStatus(void)
{
    uint32_t FR = UART1->FR;

    if (FR &= UART1_FR_TXFE)
        return UART1_FIFO_Empty;
    if (FR &= UART1_FR_TXFF)
        return UART1_FIFO_Full;

    return UART1_FIFO_NeitherEmptyOrFull;
}

char uart1_getc()
{
    if (uart1_RxFifoStatus() == UART1_FIFO_Empty)
        return '\0'; // No new character

    return UART1->DR;
}

void uart1_putc(const char c)
{
    while (uart1_TxFifoStatus() != UART1_FIFO_Empty) // Check for FIFO_Full led to missing chars in output!
        ;                                            // Wait for space in TxFIFO

    UART1->DR = c;
}

enum UART1E uart1_Transmit(const char *string)
{
    /* Guard clauses */
    if (string == NULL)
        return UART1E_ArgNULL;
    /* ------------- */

    while (*string != '\0')
    {
        uart1_putc(*string);
        string++;
    }

    return UART1E_Ok;
}

// void UART1_Handler(void)
// {
// }