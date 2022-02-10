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
#define CMDS_DAC_VSET "DAC:SET"
#define CMDS_DAC_VGET "DAC:GET"

#define CMDS_DAC_VSET_ARGC 2
#define CMDS_DAC_VSET_CHANNEL_STARTINDEX 1
#define CMDS_DAC_VSET_CHNL2CHNL_SEPARTOR ","
#define CMDS_DAC_VSET_CHANNEL_FROM_TO_SEPARTOR "-"
#define CMDS_DAC_VSET_CHANNEL_VOLTAGE_SEPARATOR ":"
// #define CMDS_DAC_VSET_VOLTAGEINDEX 2

/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/
void cmdsDAC_setVoltage(terminalCmd_t *cmd);
void cmdsDAC_getVoltage(terminalCmd_t *cmd);

#endif