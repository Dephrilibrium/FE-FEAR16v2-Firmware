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

    uart1_Transmit("Hello from the FEAR-16. What can I do for you?\r\n");

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