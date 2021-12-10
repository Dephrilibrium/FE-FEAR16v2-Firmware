#ifndef CMDS_DAC_H
#define CMDS_DAC_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "terminal.h"

/*******************************\
| Defines
\*******************************/
#define CMDS_DAC_VSET "VSET"

#define CMDS_DAC_VSET_ARGC 3
#define CMDS_DAC_VSET_CHANNELINDEX 1
#define CMDS_DAC_VSET_CHANNELSEPARTOR ","
#define CMDS_DAC_VSET_CHANNEL_FROM_TO_SEPARTOR "-"
#define CMDS_DAC_VSET_VOLTAGEINDEX 2

/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/
void cmdsDAC_setVoltage(terminalCmd_t *cmd);

#endif