/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "uart1.h"
#include "cmdsADC.h"
#include "adcWrapper.h"
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
#define CMDS_ADC_OUTPUTSTRINGBUFFER_SIZE (1 + 2 + 1 + 4 + 1) * 1 // ADC_NALLCHPACKS

/*******************************\
| Local Enum/Struct/Union
\*******************************/
typedef struct
{
  enum adcChain targetChain;
  uint16_t nChannels;
  struct
  {
    cBool requested;
    // double voltage; // It seems that keeping the value withing the request-structure is not necessary
  } ChRequests[ADC_NALLCHPACKS];
} adcChUpdRequest_t;

typedef struct
{
  const uint16_t nSize;
  uint16_t nFill;
  char string[CMDS_ADC_OUTPUTSTRINGBUFFER_SIZE];
} adcOutputString_t;

/*******************************\
| Local function declarations
\*******************************/
void cmdsDAC_parseArgs(terminalCmd_t *cmd);

/*******************************\
| Global variables
\*******************************/
adcChUpdRequest_t _adcChRequests = {.nChannels = ADC_NALLCHPACKS, .ChRequests = {{0}}};
// adcOutputString_t _outputString = {.nSize = CMDS_ADC_OUTPUTSTRINGBUFFER_SIZE, .nFill = 0, .string = {0}};

/*******************************\
| Function definitons
\*******************************/
void cmdsADC_parseArgs(terminalCmd_t *cmd)
{
  // Get target chain
  if (!strcmp(cmd->argv[CMDS_ADC_VGET_CHAININDEX], CMDS_ADC_VGET_CHAIN_CF))
    _adcChRequests.targetChain = adcChain_CF;
  else if (!strcmp(cmd->argv[CMDS_ADC_VGET_CHAININDEX], CMDS_ADC_VGET_CHAIN_UDRP))
    _adcChRequests.targetChain = adcChain_UDrp;
  else
    _adcChRequests.targetChain = -1; // No matching chain -> Deselect!

  // Determine which channels are requested
  // How much separators given
  uint8_t cSgmnts = 1 + cstrCntChar(cmd->argv[CMDS_ADC_VGET_CHANNELINDEX],
                                    CMDS_ADC_VGET_CHANNELSEPARTOR[0]);

  // Split the separated "from-to" channel-strings
  uint8_t iSgmnt = 0;
  char *segStrgs[cSgmnts];
  segStrgs[0] = strtok(cmd->argv[CMDS_ADC_VGET_CHANNELINDEX],
                       CMDS_ADC_VGET_CHANNELSEPARTOR);
  for (iSgmnt++; iSgmnt < cSgmnts; iSgmnt++)
    segStrgs[iSgmnt] = strtok(NULL, CMDS_ADC_VGET_CHANNELSEPARTOR);
  // Run through "from-to" channel-strings and set channel-query and target-voltage
  int16_t fromCh;
  int16_t toCh;
  char *tmpStr = NULL;
  for (iSgmnt = 0; iSgmnt < cSgmnts; iSgmnt++)
  {
    tmpStr = strtok(segStrgs[iSgmnt], CMDS_ADC_VGET_CHANNEL_FROM_TO_SEPARTOR);
    fromCh = atoi(tmpStr);
    tmpStr = strtok(NULL, CMDS_ADC_VGET_CHANNEL_FROM_TO_SEPARTOR);
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
      if (iCh >= _adcChRequests.nChannels)
        continue;

      _adcChRequests.ChRequests[iCh].requested = bTrue;
    }
  }
}

void cmdsADC_getVoltage(terminalCmd_t *cmd)
{
  /* Guard clauses */
  if (cmd->argc < CMDS_ADC_VGET_ARGC)
  {
    terminal_NAK(" Not enough arguments");
    terminal_send(lineTerm.stdlineTerm);
    return;
  }
  else if (cmd->argc > CMDS_ADC_VGET_ARGC)
  {
    terminal_NAK(" Too much arguments");
    terminal_send(lineTerm.stdlineTerm);
    return;
  }

  // cmdsADC_parseArgs(cmd);
  // if (_adcChRequests.targetChain < adcChain_CF || _adcChRequests.targetChain > adcChain_UDrp)
  // {
  //   terminal_NAK(" Unknown ADC-chain");
  //   terminal_send(lineTerm.stdlineTerm);
  //   return;
  // }
  // terminal_ACK(NULL);
  // terminal_send(lineTerm.stdlineTerm);
  adc_takeMeasurement(_adcChRequests.targetChain);

  // _outputString.nFill = 0;
  adc_measurementValues_t *measurements = adc_grabMeasurements();
  for (uint16_t iQuery = 0; iQuery < _adcChRequests.nChannels; iQuery++)
  {
    if (_adcChRequests.ChRequests[iQuery].requested == bTrue)
    {
      _adcChRequests.ChRequests[iQuery].requested = bFalse;
      // _adcChRequests.ChRequests[iQuery].voltage = measurements->chains[_adcChRequests.targetChain].voltages[iQuery];
      // sprintf(&_outputString.string[_outputString.nFill], "%.4f", measurements->chains[_adcChRequests.targetChain].voltages[iQuery]);
    }
  }
}
