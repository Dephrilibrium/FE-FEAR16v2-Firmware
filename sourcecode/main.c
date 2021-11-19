/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "TM4C123GH6PM.h"

/* Project specific */
#include "sysclk.h"
#include "uart1.h"
#include "terminal.h"

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

    // Other stuff
    uart1_Transmit("Hallo welt! Ich schreibe einen längeren Text");
    uart1_Transmit(lineTerm.stdlineTerm);
    uart1_Transmit("mit ein paar transmits, damit ich sehe was passiert");
    uart1_Transmit(lineTerm.stdlineTerm);

    while (1)
    {
        cmd = terminal_fetchCmd();
        if (cmd != NULL)
            terminal_runCmd(cmd);
    }
}
