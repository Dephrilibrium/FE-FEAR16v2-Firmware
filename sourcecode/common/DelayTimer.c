/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdio.h"
#include "stdlib.h"
#include "TM4C123GH6PM.h"

/* WachsCode */
#include "DelayTimer.h"
#include "common.h"

/*******************************\
| Local Defines
\*******************************/
#define TIM4_DEBUG_TASTALL // Uncomment when timer should also stop when debugger stops µC

#define SYSCTL_RCGCTIMER_TIM4 BIT(4)
#define TIMx_CFG_32BIT_TIMER OPTION(0x00, 0)
#define TIMx_TAMR_PERIODIC OPTION(0x02, 0)
#define TIMx_IR_TATO BIT(0)
#define TIMx_CTL_TAEN BIT(0)
#define TIMx_CTL_TASTALL BIT(1)
#define TIMx_ICR_TATOCINT BIT(0)



#define DELAYTIMER_NDELAYS 8 // Amount of available delays

/*******************************\
| Local function declarations
\*******************************/
void __Delay_TimerStart__(void);
void __Delay_TimerStop__(void);
uint8_t __Delay_TimerActive__(void);
uint16_t __Delay_IndexOfActive__(t_DelayStruct *delay);

/*******************************\
| Global variables
\*******************************/
struct
{
    t_DelayStruct DelayStructs[DELAYTIMER_NDELAYS];
    uint8_t InUse[DELAYTIMER_NDELAYS];
    t_DelayStruct *ActiveDelays[DELAYTIMER_NDELAYS];
    const uint16_t Size;
    uint16_t Cnt;
} _delays = {.DelayStructs = {{0}},
             .InUse = {0},
             .ActiveDelays = {0},
             .Size = DELAYTIMER_NDELAYS,
             .Cnt = 0};

/*******************************\
| Function defintions
\*******************************/
void Delay_Init(void)
{

    SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_TIM4;

    TIMER4->CFG = TIMx_CFG_32BIT_TIMER;
    TIMER4->TAMR = TIMx_TAMR_PERIODIC;

#ifdef TIM4_DEBUG_TASTALL
    TIMER4->CTL |= TIMx_CTL_TASTALL;
#endif

    uint32_t tickBaseDiv = (uint32_t)(sys_clk_freq() / DELAYTIMER_TICKBASEDIV); // 1ms
    TIMER4->TAILR = tickBaseDiv;
    TIMER4->IMR = TIMx_IR_TATO;

    uint16_t nvicFlagNum = 70;
    uint16_t nvicRegNum = (uint16_t)(nvicFlagNum / 32);
    uint16_t nvicIRFlag = (uint16_t)(nvicFlagNum % 32);
    NVIC->ISER[nvicRegNum] |= BIT(nvicIRFlag);
}

void __Delay_TimerStart__(void)
{
    TIMER4->TAR = 0;
    TIMER4->CTL |= TIMx_CTL_TAEN; // Enable timer
}

void __Delay_TimerStop__(void)
{
    TIMER4->CTL &= ~TIMx_CTL_TAEN; // Enable timer
}

uint8_t __Delay_TimerActive__(void)
{
    return (TIMER4->CTL & TIMx_CTL_TAEN ? 1 : 0);
}

void Timer4A_Handler(void)
{
    TIMER4->ICR |= TIMx_ICR_TATOCINT; // Clear interrupt

    for (uint16_t i = 0; i < _delays.Cnt; i++)
    {
        if (_delays.ActiveDelays[i]->DelayCounter)
        {
            _delays.ActiveDelays[i]->DelayCounter--;
            continue;
        }

        _delays.ActiveDelays[i]->DelayElapsed = 1;
    }
}

uint16_t Delay_AsyncNew(uint32_t ms)
{
    if (_delays.Cnt >= _delays.Size)
        return 0; // No space, return NULL

    // Search for the free delay-handle
    uint16_t _iFree = 0;
    for (; _iFree < _delays.Size; _iFree++)
        if (_delays.InUse[_iFree] == 0)
            break;
    _delays.InUse[_iFree] = 1; // Mark found one as "in use"

    // Prepare delay
    t_DelayStruct *delay = &_delays.DelayStructs[_iFree];
    Delay_AsyncAdjust(_iFree, ms);
    delay->DelayCounter = delay->DelayWait;
    delay->DelayElapsed = (delay->DelayWait > 0 ? 0 : 1);

    _delays.ActiveDelays[_delays.Cnt] = delay; // Entry into decrementer for interrupt

    // Mark delay as used
    _delays.Cnt++;
    if (_delays.Cnt == 1)
        __Delay_TimerStart__(); // Start timer when first delay was added

    return _delays.Cnt - 1; // Return the handle
}

void Delay_AsyncAdjust(uint16_t Hndl, uint32_t ms)
{
    _delays.DelayStructs[Hndl].DelayWait = ms;
}

uint16_t __Delay_IndexOfActive__(t_DelayStruct *delay)
{
    for (uint16_t i = 0; i < _delays.Cnt; i++)
    {
        if (delay == _delays.ActiveDelays[i])
            return i;
    }

    return -1;
}

uint8_t Delay_Remove(uint16_t DelayNum)
{
    // Check if active
    if (_delays.InUse[DelayNum] == 0)
        return -1;

    // Remove delay from active
    t_DelayStruct *delay = &_delays.DelayStructs[DelayNum];
    _delays.InUse[DelayNum] = 0;
    uint16_t _iActiveDelay = __Delay_IndexOfActive__(delay);
    _delays.Cnt--;                                         // Decrement Cnt to match as index!
    for (uint16_t i = _iActiveDelay; i < _delays.Cnt; i++) // Cnt already -1 from line above!
        _delays.ActiveDelays[i] = _delays.ActiveDelays[i + 1];
    _delays.ActiveDelays[_delays.Cnt] = 0;

    if (!_delays.Cnt)
        __Delay_TimerStop__();

    return 0;
}

void Delay_Reset(uint16_t DelayNum)
{
    _delays.DelayStructs[DelayNum].DelayCounter = _delays.DelayStructs[DelayNum].DelayWait;
    if (_delays.DelayStructs[DelayNum].DelayWait == 0)
        return;

    _delays.DelayStructs[DelayNum].DelayElapsed = 0;
}

uint16_t Delay_DeltaTime(uint16_t DelayNum)
{
    uint32_t diff = _delays.DelayStructs[DelayNum].DelayWait - _delays.DelayStructs[DelayNum].DelayCounter;
    return diff;
}

uint8_t Delay_AsyncWait(uint16_t DelayNum)
{
    return _delays.DelayStructs[DelayNum].DelayElapsed;
}

void Delay_Await(uint32_t ms)
{
    uint16_t asyncHandle = Delay_AsyncNew(ms);
    while (!Delay_AsyncWait(asyncHandle))
        ;
    Delay_Remove(asyncHandle);
}
