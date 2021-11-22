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
    uart1_init(921600);
    terminal_init();
    // ssi0_init(ssi0_clkRate_1MHz);

    // Other stuff
    uart1_Transmit("Hallo welt! Ich schreibe einen längeren Text");
    uart1_Transmit(lineTerm.stdlineTerm);

    while (1)
    {
        cmd = terminal_fetchCmd();
        if (cmd != NULL)
            terminal_runCmd(cmd);
    }
}
