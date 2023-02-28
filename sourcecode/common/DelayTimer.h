#ifndef DELAYTIMER_H
#define DELAYTIMER_H

/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdint.h"
/* Wordclock */
#include "sysclk.h"

/*******************************\
| Defines
\*******************************/
#define DELAYTIMER_TICKBASEDIV (uint32_t)1e3 // SysClk / TICKBASE-Divider = Tickbase -> 1ms

/*******************************\
| Enum/Struct/Union
\*******************************/
typedef struct
{
    uint32_t DelayWait;
    uint32_t DelayCounter;
    uint16_t DelayElapsed;
} t_DelayStruct;

/*******************************\
| Function declarations
\*******************************/
void Delay_Init(void);

uint16_t Delay_AsyncNew(uint32_t ms);
void Delay_AsyncAdjust(uint16_t Hndl, uint32_t ms);
uint8_t Delay_Remove(uint16_t Hndl);
void Delay_Await(uint32_t ms);
uint8_t Delay_AsyncWait(uint16_t Hndl);
void Delay_Reset(uint16_t Hndl);
uint16_t Delay_DeltaTime(uint16_t Hndl);

#endif
