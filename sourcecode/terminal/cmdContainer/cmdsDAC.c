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
/* Buffersize is
 * Digits per Channel = 9 Digits
 *                    = 1* + or - sign
 *                    + 2 * Mantis
 *                    + 1 * Decimal separator (.)
 *                    + 4 * Decimal places
 *                    + 1 * Number separator (;)
 * Size = Digits per Channel * 16 (Number of Channels) * 2 (Security-space!)
 */
#define CMDS_DAC_OUTPUTSTRINGBUFFER_SIZE (1 + 2 + 1 + 4 + 1) * 16 // ADC_NALLCHPACKS

/*******************************\
| Local Enum/Struct/Union
\*******************************/
struct dacChUpdRequest
{
  uint16_t nDACs;
  uint16_t nChPerDAC;
  uint16_t nAllChannels;
  struct
  {
    cBool requested;
    double voltage;
  } ChRequests[DAC_NALLVOLTPACKS];
};

typedef struct
{
  const uint16_t nSize;
  uint16_t nFill;
  char string[CMDS_DAC_OUTPUTSTRINGBUFFER_SIZE];
} dacOutputString_t;

/*******************************\
| Local function declarations
\*******************************/
void cmdsDAC_parseArgs(terminalCmd_t *cmd);

/*******************************\
| Global variables
\*******************************/
struct dacChUpdRequest _dacChRequests = {.nDACs = DAC_NDACS, .nChPerDAC = DAC_VOLTPACKS, .nAllChannels = DAC_NALLVOLTPACKS, .ChRequests = {{0}}};
dacOutputString_t _dacOutputString = {.nSize = CMDS_DAC_OUTPUTSTRINGBUFFER_SIZE, .nFill = 0, .string = {0}};

/*******************************\
| Function definitons
\*******************************/
void cmdsDAC_parseArgs(terminalCmd_t *cmd)
{
  // Determine which channels should be updated
  // How much separators given
  uint8_t cSgmnts = 1 + cstrCntChar(cmd->argv[CMDS_DAC_VSET_CHANNEL_STARTINDEX],
                                    CMDS_DAC_VSET_CHNL2CHNL_SEPARTOR[0]);

  // Split the voltage-combined "from-to" channel-strings
  uint8_t iSgmnt = 0;
  char *segChStrgs[cSgmnts];
  char *segValStrgs[cSgmnts];
  segChStrgs[0] = strtok(cmd->argv[CMDS_DAC_VSET_CHANNEL_STARTINDEX],
                         CMDS_DAC_VSET_CHNL2CHNL_SEPARTOR);
  for (iSgmnt++; iSgmnt < cSgmnts; iSgmnt++)
    segChStrgs[iSgmnt] = strtok(NULL, CMDS_DAC_VSET_CHNL2CHNL_SEPARTOR);

  // Separate voltage from channels
  for (iSgmnt = 0; iSgmnt < cSgmnts; iSgmnt++)
  {
    segChStrgs[iSgmnt] = strtok(segChStrgs[iSgmnt],
                                CMDS_DAC_VSET_CHANNEL_VOLTAGE_SEPARATOR);
    segValStrgs[iSgmnt] = strtok(NULL,
                                 CMDS_DAC_VSET_CHANNEL_VOLTAGE_SEPARATOR);
  }

  // Run through "from-to" channel-strings and set channel-query and target-voltage
  int16_t fromCh;
  int16_t toCh;
  double voltage;
  char *tmpStr = NULL;
  for (iSgmnt = 0; iSgmnt < cSgmnts; iSgmnt++)
  {
    tmpStr = strtok(segChStrgs[iSgmnt], CMDS_DAC_VSET_CHANNEL_FROM_TO_SEPARTOR);
    fromCh = atoi(&tmpStr[2]); // Remove "CH" from "CHx" and convert x
    tmpStr = strtok(NULL, CMDS_DAC_VSET_CHANNEL_FROM_TO_SEPARTOR);
    if (tmpStr)
      toCh = atoi(&tmpStr[2]); // Remove "CH" from "CHx" and convert x
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
    voltage = atof(segValStrgs[iSgmnt]);
    for (uint8_t iCh = fromCh; iCh <= toCh; iCh++)
    {
      // Jump over channels which not exists
      if (iCh >= _dacChRequests.nAllChannels)
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
    terminal_NAK("Not enough arguments");
    return;
  }
  else if (cmd->argc > CMDS_DAC_VSET_ARGC)
  {
    terminal_send(TERMINAL_NOT_ACKNOWLEDGED);
    terminal_send("Too much arguments from: ");
    terminal_sendline(cmd->argv[CMDS_DAC_VSET_ARGC]);
    return;
  }

  cmdsDAC_parseArgs(cmd);
  uint16_t iQuery = 0;
  for (uint16_t iCh = 0; iCh < _dacChRequests.nAllChannels; iCh++)
  {
    iQuery = iCh;
    if (_dacChRequests.ChRequests[iCh].requested == bTrue) // Check if channel is requested
    {
      // Run through all DACs of a chain, update values and reset request
      for (uint8_t iDAC = 0; iDAC < _dacChRequests.nDACs; iDAC++)
      {
        iQuery = (iCh % _dacChRequests.nChPerDAC) + (_dacChRequests.nChPerDAC * iDAC);
        _dacChRequests.ChRequests[iQuery].requested = bFalse;
        dac_updateChVoltage(iQuery, _dacChRequests.ChRequests[iQuery].voltage);
      }
      dac_sendChVoltage(iCh);
    }
  }
  terminal_ACK(NULL);
}

void cmdsDAC_getVoltage(terminalCmd_t *cmd)
{
  /* Guard clauses */
  if (cmd->argc < CMDS_DAC_VSET_ARGC)
  {
    terminal_NAK("Not enough arguments");
    return;
  }
  else if (cmd->argc > CMDS_DAC_VSET_ARGC)
  {
    terminal_NAK("Too much arguments");
    return;
  }

  cmdsDAC_parseArgs(cmd);

  _dacOutputString.nFill = 0;
  _dacOutputString.string[0] = '\0';
  for (uint16_t iQuery = 0; iQuery < _dacChRequests.nAllChannels; iQuery++)
  {
    if (_dacChRequests.ChRequests[iQuery].requested == bTrue)
    {
      _dacChRequests.ChRequests[iQuery].requested = bFalse;
      sprintf(&_dacOutputString.string[_dacOutputString.nFill], "%.4f,", _dacChRequests.ChRequests[iQuery].voltage);
      _dacOutputString.nFill += strlen(&_dacOutputString.string[_dacOutputString.nFill]);
    }
  }
  _dacOutputString.nFill--;
  _dacOutputString.string[_dacOutputString.nFill] = '\0'; // Remove tailing ','
  terminal_ACK(_dacOutputString.string);
}
