#ifndef CMDS_ADC_H
#define CMDS_ADC_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "terminal.h"

/*******************************\
| Defines
\*******************************/
#define CMDS_ADC_VGET "ADC:GET"

#define CMDS_ADC_VGET_ARGC 3
#define CMDS_ADC_VGET_CHAININDEX 1
#define CMDS_ADC_VGET_CHAIN_CF "CF"
#define CMDS_ADC_VGET_CHAIN_UDRP "UDRP"
#define CMDS_ADC_VGET_CHANNEL_STARTINDEX 2
#define CMDS_ADC_VGET_CHANNEL_SEPARTOR "," // Only 1 character allowed!
#define CMDS_ADC_VGET_CHANNEL_FROM_TO_SEPARTOR "-"

/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/
void cmdsADC_getVoltage(terminalCmd_t *cmd);

#endif