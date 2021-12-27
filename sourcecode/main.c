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
    sys_clk_set(); // Set 80MHz
    pTime_init(pTime_tickbase_10us);
    terminal_init();
    // dacs_init();
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