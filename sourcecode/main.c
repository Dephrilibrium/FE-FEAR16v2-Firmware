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
#include "preciseTime.h"

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

    // Inits
    pTime_init(pTime_tickbase_1ms);
    pTime_wait(500); // Wait 0.5s to be sure the supply-voltages could built up before starting init

    sys_clk_set(); // Set 80MHz
    pTime_changeTickbase(pTime_tickbase_1us);
    pTime_wait((uint32_t)500e3); // Wait another 500ms to stabilize supply (µC needs more current when boosted up)

    terminal_init();
    dacs_init();
    adcs_init();

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