/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdio.h"

/* Project specific */
#include "uart1.h"
#include "cmdsADC.h"
#include "adcWrapper.h"
#include "cString.h"
#include "MovingAverage.h"
#include "DelayTimer.h"

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
#define CMDS_ADC_OUTPUTSTRINGBUFFER_SIZE (1 + 2 + 1 + 4 + 1) * 16 // ADC_NALLCHPACKS

/*******************************\
| Local Enum/Struct/Union
\*******************************/
typedef struct
{
  enum adcChain targetChain;
  uint16_t nADCs;
  uint16_t nChPerADC;
  uint16_t nAllChannels;
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
void cmdsADC_RecalculateMean(void);

/*******************************\
| Global variables
\*******************************/
adcChUpdRequest_t _adcChRequests = {.nADCs = ADC_NADCS, .nChPerADC = ADC_CHANNELS, .nAllChannels = ADC_NALLCHPACKS, .ChRequests = {{0}}};
double _adcMeanBuffers[ADC_NCHAINS][ADC_NALLCHPACKS][CMDS_ADC_NMEAN_MAX];
MovingAverage_t _adcAverageStructs[ADC_NCHAINS][ADC_NALLCHPACKS] = {0}; // Real setup in init
adcOutputString_t _adcOutputString = {.nSize = CMDS_ADC_OUTPUTSTRINGBUFFER_SIZE, .nFill = 0, .string = {0}};
uint16_t _delayHandle;

/*******************************\
| Function definitons
\*******************************/
void cmdsADC_InitializeMeanStruct(void)
{
  for (int iChain = 0; iChain < ADC_NCHAINS; iChain++)
  {
    for (int iChnl = 0; iChnl < ADC_NALLCHPACKS; iChnl++)
    {
      _adcAverageStructs[iChain][iChnl].nSize = CMDS_ADC_NMEAN_DEFAULT;
      // _adcAverageStructs[i].nFill = 0;        // Already 0
      // _adcAverageStructs[i].iPos = 0;         // Already 0
      // _adcAverageStructs[i].BuffMean = 0.0;   // Already 0
      _adcAverageStructs[iChain][iChnl].Buffer = _adcMeanBuffers[iChain][iChnl];
    }
  }
}

void cmdsADC_InitializeMeasurementDelta(void)
{
  _delayHandle = Delay_AsyncNew(CMDS_ADC_MDELT_DEFAULT);
}

void cmdsADC_AdjustMeasurementDelta(terminalCmd_t *cmd)
{
  if (cmd->argc < CMDS_ADC_NMEAN_ARGC)
  {
    terminal_NAK("Not enough arguments");
    return;
  }

  if (cmd->argc > CMDS_ADC_NMEAN_ARGC)
  {
    terminal_NAK("Too much arguments");
    return;
  }

  uint32_t msWait = atoi(cmd->argv[CMDS_ADC_NMEAN_NPNTS_INDEX]);
  if (msWait < CMDS_ADC_MDELT_MIN || msWait > CMDS_ADC_MDELT_MAX)
  {
    char nakMsg[64];
    sprintf(nakMsg, "%d <= msWait <= %d or conversion error.", CMDS_ADC_MDELT_MIN, CMDS_ADC_MDELT_MAX);
    terminal_NAK(nakMsg);
    return;
  }
  Delay_AsyncAdjust(_delayHandle, msWait);
  terminal_ACK(NULL);
}

uint16_t cmdsADC_GetDelayHandle(void)
{
  return _delayHandle;
}

void cmdsADC_AdjustNMean(terminalCmd_t *cmd)
{
  if (cmd->argc < CMDS_ADC_NMEAN_ARGC)
  {
    terminal_NAK("Not enough arguments");
    return;
  }

  if (cmd->argc > CMDS_ADC_NMEAN_ARGC)
  {
    terminal_NAK("Too much arguments");
    return;
  }

  int32_t nPnts = atoi(cmd->argv[CMDS_ADC_NMEAN_NPNTS_INDEX]);
  if (nPnts < CMDS_ADC_NMEAN_MIN || nPnts > CMDS_ADC_NMEAN_MAX)
  {
    char nakMsg[64];
    sprintf(nakMsg, "%d <= NMEAN-Points <= %d or conversion error.", CMDS_ADC_NMEAN_MIN, CMDS_ADC_NMEAN_MAX);
    terminal_NAK(nakMsg);
    return;
  }

  for (uint16_t iChain = adcChain_CF; iChain < ADC_NCHAINS; iChain++)
  {
    for (uint16_t iChnl = 0; iChnl < ADC_NALLCHPACKS; iChnl++)
    {
      _adcAverageStructs[iChain][iChnl].nSize = nPnts;
      // When nMean is changed -> Reset all values and begin again
      _adcAverageStructs[iChain][iChnl].nFill = 0;
      _adcAverageStructs[iChain][iChnl].iPos = 0;
      _adcAverageStructs[iChain][iChnl].BuffMean = 0.0;
    }
  }

  terminal_ACK(NULL);
}

void cmdsADC_parseArgs(terminalCmd_t *cmd)
{
  // Get target chain
  if (strcmp(cmd->argv[CMDS_ADC_VGET_CHAININDEX], CMDS_ADC_VGET_CHAIN_CF) == 0)
    _adcChRequests.targetChain = adcChain_CF;
  else if (strcmp(cmd->argv[CMDS_ADC_VGET_CHAININDEX], CMDS_ADC_VGET_CHAIN_UDRP) == 0)
    _adcChRequests.targetChain = adcChain_UDrp;
  else // Unknown Chain-Identifier -> Avoid residual code to save time -> return
  {
    _adcChRequests.targetChain = -1; // No matching chain -> Deselect!
    return;
  }

  // Determine which channels are requested
  // How much separators given
  uint8_t cSgmnts = 1 + cstrCntChar(cmd->argv[CMDS_ADC_VGET_CHANNEL_STARTINDEX],
                                    CMDS_ADC_VGET_CHANNEL_SEPARTOR[0]);

  // Split the separated "from-to" channel-strings
  uint8_t iSgmnt = 0;
  char *segChStrgs[cSgmnts];
  segChStrgs[0] = strtok(cmd->argv[CMDS_ADC_VGET_CHANNEL_STARTINDEX],
                         CMDS_ADC_VGET_CHANNEL_SEPARTOR);
  for (iSgmnt++; iSgmnt < cSgmnts; iSgmnt++)
    segChStrgs[iSgmnt] = strtok(NULL, CMDS_ADC_VGET_CHANNEL_SEPARTOR);
  // Run through "from-to" channel-strings and set channel-query and target-voltage
  int16_t fromCh;
  int16_t toCh;
  char *tmpStr = NULL;
  for (iSgmnt = 0; iSgmnt < cSgmnts; iSgmnt++)
  {
    tmpStr = strtok(segChStrgs[iSgmnt], CMDS_ADC_VGET_CHANNEL_FROM_TO_SEPARTOR);
    fromCh = atoi(&tmpStr[2]); // Remove "CH" from "CHx" and convert x
    tmpStr = strtok(NULL, CMDS_ADC_VGET_CHANNEL_FROM_TO_SEPARTOR);
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
    for (uint8_t iCh = fromCh; iCh <= toCh; iCh++)
    {
      // Jump over channels which not exists
      if (iCh >= _adcChRequests.nAllChannels)
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
    terminal_NAK("Not enough arguments");
    terminal_send(termOptions.lineTerm.stdlineTerm);
    return;
  }
  else if (cmd->argc > CMDS_ADC_VGET_ARGC)
  {
    terminal_NAK("Too much arguments");
    terminal_send(termOptions.lineTerm.stdlineTerm);
    return;
  }

  cmdsADC_parseArgs(cmd);
  if (_adcChRequests.targetChain < adcChain_CF || _adcChRequests.targetChain > adcChain_UDrp)
  {
    terminal_NAK("Unknown ADC-chain");
    return;
  }
  // terminal_ACK(NULL);
  // terminal_send(termOptions.lineTerm.stdlineTerm);
  // adc_takeMeasurement(_adcChRequests.targetChain);

  // adc_measurementValues_t *measurements = adc_grabMeasurements();
  _adcOutputString.nFill = 0;
  _adcOutputString.string[0] = '\0';
  for (uint16_t iQuery = 0; iQuery < _adcChRequests.nAllChannels; iQuery++)
  {
    if (_adcChRequests.ChRequests[iQuery].requested == bTrue)
    {
      _adcChRequests.ChRequests[iQuery].requested = bFalse;
      // _adcChRequests.ChRequests[iQuery].voltage = measurements->chains[_adcChRequests.targetChain].voltages[iQuery];
      // sprintf(&_adcOutputString.string[_adcOutputString.nFill], "%.4f,", measurements->chains[_adcChRequests.targetChain].voltages[iQuery]);
      sprintf(&_adcOutputString.string[_adcOutputString.nFill], "%.4f,", _adcAverageStructs[_adcChRequests.targetChain][iQuery].BuffMean);
      _adcOutputString.nFill += strlen(&_adcOutputString.string[_adcOutputString.nFill]);
    }
  }
  _adcOutputString.nFill--;
  _adcOutputString.string[_adcOutputString.nFill] = '\0'; // Remove tailing ','
  terminal_ACK(_adcOutputString.string);
}

void cmdsADC_measVoltage2Mean(void)
{
  for (uint16_t iChain = adcChain_CF; iChain < ADC_NCHAINS; iChain++)
  {
    adc_takeMeasurement(iChain);
    adc_measurementValues_t *measurements = adc_grabMeasurements();

    for (uint16_t iChnl = 0; iChnl < _adcChRequests.nAllChannels; iChnl++)
      Average(&_adcAverageStructs[iChain][iChnl], measurements->chains[iChain].voltages[iChnl]);
  }
}