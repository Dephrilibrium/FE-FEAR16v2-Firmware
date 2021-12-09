/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "TM4C123GH6PM.h"
#include "sysclk.h"
#include "uart1.h"
#include "terminal.h"
#include "ssi0_DACs.h"
#include "dacWrapper.h"
#include "preciseTime.h"

#include "swap.h"

/*******************************\
| Local Defines
\*******************************/

/*******************************\
| Local function declarations
\*******************************/

/*******************************\
| Global variables
\*******************************/
char blubb = 'i';
uint8_t abc[] = "lksjdafsjd\0";

/*******************************\
| Function definitons
\*******************************/
int main(void)
{
    terminalCmd_t *cmd;

    // Inits
    sys_clk_set(); // Set 80MHz
    pTime_init(pTime_tickbase_10us);
    terminal_init();
    dacs_init();

    // dac_setChVoltage(dac_voltPack_CH0, 0.0f);
    // dac_setChVoltage(dac_voltPack_CH1, 0.0f);
    // dac_setChVoltage(dac_voltPack_CH0, 1.0f);
    // dac_setChVoltage(dac_voltPack_CH1, 1.0f);
    // dac_setChVoltage(dac_voltPack_CH0, 2.0f);
    // dac_setChVoltage(dac_voltPack_CH1, 2.0f);
    // dac_setChVoltage(dac_voltPack_CH0, -1.0f);
    // dac_setChVoltage(dac_voltPack_CH1, -1.0f);

    while (1)
    {
        cmd = terminal_fetchCmd();
        if (cmd != NULL)
            terminal_runCmd(cmd);
    }
}

void delay(uint32_t delay)
{
    while (delay--)
        ;
}