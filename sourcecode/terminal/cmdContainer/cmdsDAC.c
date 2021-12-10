/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "uart1.h"
#include "cmdsDAC.h"
#include "dacWrapper.h"
#include "cString.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Hardwaredefines

#pragma endregion Hardwaredefines

/*******************************\
| Local Enum/Struct/Union
\*******************************/
struct dacChUpdRequest
{
  uint16_t nChannels;
  struct
  {
    cBool requested;
    float voltage;
  } ChRequests[DAC_NALLVOLTPACKS];
};

/*******************************\
| Local function declarations
\*******************************/
void cmdsDAC_parseArgs(terminalCmd_t *cmd);

/*******************************\
| Global variables
\*******************************/
struct dacChUpdRequest _dacChRequests = {.nChannels = DAC_NALLVOLTPACKS, .ChRequests = {{0}}};

/*******************************\
| Function definitons
\*******************************/
void cmdsDAC_parseArgs(terminalCmd_t *cmd)
{
  // Convert voltage as float
  float voltage = atof(cmd->argv[CMDS_DAC_VSET_VOLTAGEINDEX]);

  // Determine which channels should be updated
  // How much separators given
  uint8_t cSep = cstrCntChar(cmd->argv[CMDS_DAC_VSET_CHANNELINDEX],
                             CMDS_DAC_VSET_CHANNELSEPARTOR[0]);
  // Split the separated "from-to" channel-strings
  uint8_t iSep = 0;
  char *sepStr[cSep];
  sepStr[0] = strtok(cmd->argv[CMDS_DAC_VSET_CHANNELINDEX],
                     CMDS_DAC_VSET_CHANNELSEPARTOR);
  for (iSep++; iSep < cSep; iSep++)
    sepStr[iSep] = strtok(NULL, CMDS_DAC_VSET_CHANNELSEPARTOR);
  // Run through "from-to" channel-strings and set channel-query and target-voltage
  int16_t fromCh;
  int16_t toCh;
  char *tmpStr = NULL;
  for (iSep = 0; iSep < cSep; iSep++)
  {
    tmpStr = strtok(sepStr[iSep], CMDS_DAC_VSET_CHANNELSEPARTOR);
    fromCh = atoi(tmpStr);
    tmpStr = strtok(NULL, CMDS_DAC_VSET_CHANNELSEPARTOR);
    if (tmpStr)
      toCh = fromCh;
    else
      toCh = atoi(tmpStr);

    // Assign float voltage to queried channels
    for (uint8_t iCh = fromCh; iCh <= toCh; iCh++)
    {
      _dacChRequests.ChRequests[iCh].requested = bTrue;
      _dacChRequests.ChRequests[iCh].requested = voltage;
    }
  }
}

void cmdsDAC_setVoltage(terminalCmd_t *cmd)
{
  /* Guard clauses */
  if (cmd->argc > CMDS_DAC_VSET_ARGC)
    terminal_NAK(" Not enough arguments");

  cmdsDAC_parseArgs(cmd);
  terminal_ACK(NULL);

  for (uint16_t iQuery = 0; iQuery < _dacChRequests.nChannels; iQuery++)
  {
    if (_dacChRequests.ChRequests[iQuery].requested == bTrue)
    {
      _dacChRequests.ChRequests[iQuery].requested = bFalse;
      dac_setChVoltage(iQuery, _dacChRequests.ChRequests[iQuery].voltage);
    }
  }
}
