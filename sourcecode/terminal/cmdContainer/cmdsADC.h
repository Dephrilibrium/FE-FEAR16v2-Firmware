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
#define CMDS_ADC_VGET_CHAIN_SHNT "SHNT"
#define CMDS_ADC_VGET_CHAIN_DROP "DROP"
#define CMDS_ADC_VGET_CHANNEL_STARTINDEX 2
#define CMDS_ADC_VGET_CHANNEL_SEPARTOR "," // Only 1 character allowed!
#define CMDS_ADC_VGET_CHANNEL_FROM_TO_SEPARTOR "-"

#define CMDS_ADC_NMEAN "ADC:NMEAN" // n mean points
#define CMDS_ADC_NMEAN_MIN 1       // Minimal mean-values
#define CMDS_ADC_NMEAN_DEFAULT 10   // Minimal mean-values              * CMDS_ADC_MDELT_DEFAULT (=25ms) = 100ms
#define CMDS_ADC_NMEAN_MAX 100     // Also default nmean-value
#define CMDS_ADC_NMEAN_ARGC 2
#define CMDS_ADC_NMEAN_NPNTS_INDEX 1

#define CMDS_ADC_MDELT "ADC:MDELT" // measurement delta time in ms
#define CMDS_ADC_MDELT_MIN 5       // ms
#define CMDS_ADC_MDELT_DEFAULT 10  // ms                                * CMDS_ADC_NMEAN_DEFAULT (=4 Pnts) = 100ms
#define CMDS_ADC_MDELT_MAX 20000   // ms
#define CMDS_ADC_MEAS_ARGC 2
#define CMDS_ADC_MEAS_NPNTS_INDEX 1

/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/
void cmdsADC_InitializeMeanStruct(void);
void cmdsADC_AdjustNMean(terminalCmd_t *cmd);

void cmdsADC_InitializeMeasurementDelta(void);
void cmdsADC_AdjustMeasurementDelta(terminalCmd_t *cmd);
uint16_t cmdsADC_GetDelayHandle(void);

void cmdsADC_getVoltage(terminalCmd_t *cmd);
void cmdsADC_measVoltage2Mean(void);

#endif