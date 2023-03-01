/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "TM4C123GH6PM.h"
#include "sysclk.h"
#include "terminal.h"
#include "ssi0_DACs.h"
#include "dacWrapper.h"
#include "adcWrapper.h"
#include "DelayTimer.h"
#include "cmdsADC.h"
#include "cmdsDAC.h"
#include "dacWrapper.h"

/*******************************\
| Local Defines
\*******************************/

/*******************************\
| Local function declarations
\*******************************/

/*******************************\
| Global variables
\*******************************/

/*******************************\
| Function definitons
\*******************************/
int main(void)
{
    terminalCmd_t *cmd;
    sys_clk_set(); // Boost Clk: 80MHz
    // Inits
    Delay_Init();
    Delay_Await(1500); // Wait 1.5s to stabilize power supply

    terminal_init();
    dacs_init();
    adcs_init();

    uint16_t adcDelayHandle = cmdsADC_GetDelayHandle();
    while (1)
    {
        cmd = terminal_fetchCmd();
        if (cmd != NULL)
            terminal_runCmd(cmd);

        if (Delay_AsyncWait(adcDelayHandle)) // When delay ran off
        {
            Delay_Reset(adcDelayHandle); // Reset delay
            cmdsADC_measVoltage2Mean();  // And do the measurements/meaning
        }
    }
}
