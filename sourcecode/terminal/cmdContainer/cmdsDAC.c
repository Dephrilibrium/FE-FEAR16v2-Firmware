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
    double voltage;
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
  // Convert voltage as double
  double voltage = atof(cmd->argv[CMDS_DAC_VSET_VOLTAGEINDEX]);

  // Determine which channels should be updated
  // How much separators given
  uint8_t cSgmnts = 1 + cstrCntChar(cmd->argv[CMDS_DAC_VSET_CHANNELINDEX],
                                    CMDS_DAC_VSET_CHANNELSEPARTOR[0]);

  // Split the separated "from-to" channel-strings
  uint8_t iSgmnt = 0;
  char *segStrgs[cSgmnts];
  segStrgs[0] = strtok(cmd->argv[CMDS_DAC_VSET_CHANNELINDEX],
                       CMDS_DAC_VSET_CHANNELSEPARTOR);
  for (iSgmnt++; iSgmnt < cSgmnts; iSgmnt++)
    segStrgs[iSgmnt] = strtok(NULL, CMDS_DAC_VSET_CHANNELSEPARTOR);
  // Run through "from-to" channel-strings and set channel-query and target-voltage
  int16_t fromCh;
  int16_t toCh;
  char *tmpStr = NULL;
  for (iSgmnt = 0; iSgmnt < cSgmnts; iSgmnt++)
  {
    tmpStr = strtok(segStrgs[iSgmnt], CMDS_DAC_VSET_CHANNEL_FROM_TO_SEPARTOR);
    fromCh = atoi(tmpStr);
    tmpStr = strtok(NULL, CMDS_DAC_VSET_CHANNEL_FROM_TO_SEPARTOR);
    if (tmpStr)
      toCh = atoi(tmpStr);
    else
      toCh = fromCh;

    // Check if fromCh is smaller than toCh
    if (fromCh > toCh)
    {
      uint8_t bakCh = fromCh;
      fromCh = toCh;
      toCh = bakCh;
    }
    // Assign double voltage to queried channels
    for (uint8_t iCh = fromCh; iCh <= toCh; iCh++)
    {
      // Jump over channels which not exists
      if (iCh >= _dacChRequests.nChannels)
        continue;

      _dacChRequests.ChRequests[iCh].requested = bTrue;
      _dacChRequests.ChRequests[iCh].voltage = voltage;
    }
  }
}

void cmdsDAC_setVoltage(terminalCmd_t *cmd)
{
  /* Guard clauses */
  if (cmd->argc < CMDS_DAC_VSET_ARGC)
  {
    terminal_NAK(" Not enough arguments");
    terminal_send(lineTerm.stdlineTerm);
    return;
  }
  else if (cmd->argc > CMDS_DAC_VSET_ARGC)
  {
    terminal_NAK(" Too much arguments");
    terminal_send(lineTerm.stdlineTerm);
    return;
  }

  cmdsDAC_parseArgs(cmd);
  terminal_ACK(NULL);
  terminal_send(lineTerm.stdlineTerm);

  for (uint16_t iQuery = 0; iQuery < _dacChRequests.nChannels; iQuery++)
  {
    if (_dacChRequests.ChRequests[iQuery].requested == bTrue)
    {
      _dacChRequests.ChRequests[iQuery].requested = bFalse;
      dac_setChVoltage(iQuery, _dacChRequests.ChRequests[iQuery].voltage);
    }
  }
}
