/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "TM4C123GH6PM.h"

/* Project specific */
#include "uart1.h"

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
    static char buffer[1024] = "123";

    uart1_init(921600);

    int i = 0;
    while (1)
    {
    }
}
