#ifndef UART1_H
#define UART1_H

/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdint.h"
#include "stddef.h"

/* Project specific */
#include "common.h"

/*******************************\
| Defines
\*******************************/

/*******************************\
| Enum/Struct/Union
\*******************************/
enum UART1_SendingStatus
{
    UART1_Sending_Idle = 0,
    UART1_Sending_Busy,
};

enum UART1_FIFO_Status
{
    UART1_FIFO_Empty = 0x00,
    UART1_FIFO_Full,
    UART1_FIFO_NeitherEmptyOrFull,
};
enum UART1E
{
    UART1E_Ok = 0,
    UART1E_ArgNULL,
    UART1E_InternalBuffer_NotEnoughSpace,
    UART1E_NoTxLength,
};

/*******************************\
| Function declaration
\*******************************/
void uart1_init(uint32_t baud);
void uart1_setBaud(uint32_t baud);
uint32_t uart1_getBaud(void);
void uart1_enable(void);
void uart1_disable(void);

enum UART1_SendingStatus uart1_SendingStatus(void);
enum UART1_FIFO_Status uart1_RxFifoStatus(void);
enum UART1_FIFO_Status uart1_TxFifoStatus(void);

char uart1_getc();
void uart1_putc(const char c);
enum UART1E uart1_Transmit(const char *string);

// void uart1_putc(char c);
// void uart1_puts(char *cp);
// char uart1_getc(void);
// void uart1_gets(char *cp, uint32_t len);

#endif
