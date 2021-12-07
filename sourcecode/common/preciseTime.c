/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "preciseTime.h"
#include "sysclk.h"

/*******************************\
| Local Defines
\*******************************/
#define TIM5_DEBUG_TASTALL // Uncomment when timer should also stop when debugger stops µC

#pragma region Hardwaredefines
#define SYSCTL_RCGC_TIM5 BIT(5)

#define TIM5_CFG_32BIT_TIMER OPTION(0x00, 0)
#define TIM5_TAMR_ONESHOT OPTION(0x01, 0)

#define TIM5_IR_TATO BIT(0)

#define TIM5_CTL_TAEN BIT(0)
#define TIM5_CTL_TASTALL BIT(1)
#pragma endregion Hardwaredefines

/*******************************\
| Local Enum/Struct/Union
\*******************************/

/*******************************\
| Local function declarations
\*******************************/
void pTime_reset(uint32_t tickbaseCount);

/*******************************\
| Global variables
\*******************************/
uint32_t _tickbaseDivider = 0;
uint32_t _resetTickbaseCount = 0;

/*******************************\
| Function definitons
\*******************************/
void pTime_init(enum pTime_tickbase tickbase)
{
  SYSCTL->RCGCTIMER |= SYSCTL_RCGC_TIM5;

  TIMER5->CFG = TIM5_CFG_32BIT_TIMER;
  TIMER5->TAMR = TIM5_TAMR_ONESHOT;
#ifdef TIM5_DEBUG_TASTALL
  TIMER5->CTL |= TIM5_CTL_TASTALL;
#endif

  pTime_changeTickbase(tickbase);
}

void pTime_changeTickbase(enum pTime_tickbase tickbase)
{
  switch (tickbase)
  {
  case pTime_tickbase_1us:
    _tickbaseDivider = sys_clk_freq() / (uint32_t)1e6;
    break;

  case pTime_tickbase_10us:
    _tickbaseDivider = sys_clk_freq() / (uint32_t)1e5;
    break;

  case pTime_tickbase_100us:
    _tickbaseDivider = sys_clk_freq() / (uint32_t)1e4;
    break;

  case pTime_tickbase_1ms:
    _tickbaseDivider = sys_clk_freq() / (uint32_t)1e3;
    break;

  case pTime_tickbase_10ms:
    _tickbaseDivider = sys_clk_freq() / (uint32_t)1e2;
    break;

  case pTime_tickbase_100ms:
    _tickbaseDivider = sys_clk_freq() / (uint32_t)1e1;
    break;

  default:
    _tickbaseDivider = 0;
  }
}

void pTime_reset(uint32_t tickbaseCount)
{
  _resetTickbaseCount = tickbaseCount;
  TIMER5->TAILR = _resetTickbaseCount * _tickbaseDivider;
}

void pTime_start(uint32_t tickbaseCount)
{
  pTime_reset(tickbaseCount);
  TIMER5->CTL |= TIM5_CTL_TAEN;
}

void pTime_stop(cBool resetCounter)
{
  if (resetCounter)
    pTime_reset(_resetTickbaseCount);
}

cBool pTime_elapsed(void)
{
  if (TIMER5->RIS & TIM5_IR_TATO)
  {
    TIMER5->ICR |= TIM5_IR_TATO;
    return bTrue;
  }

  return bFalse;
}

void pTime_wait(uint32_t tickbaseCount)
{
  pTime_start(tickbaseCount);
  while (pTime_elapsed() == bFalse)
    ; // Wait for given time elapsed
}
