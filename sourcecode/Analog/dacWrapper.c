/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "dacWrapper.h"
#include "swap.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Hardwaredefines

#pragma endregion Hardwaredefines

/*******************************\
| Local function declarations
\*******************************/
void dac_wipeDACData(void);
void dac_setupSequence(void);
void dac_setToAllConfPacks(uint8_t cmdByte, uint16_t dataWord);
void dac_setToAllVoltPacks(uint8_t cmdByte, uint16_t dataWord);
void dac_setToPacks(uint8_t cmdByte, uint16_t dataWord, DAC_StructuralDataPack_t *packCollection, uint8_t nPacks);

void dac_prepareTxData(enum dac_packIndex packIndex);
void dac_fetchRxData(enum dac_packIndex packIndex);

/*******************************\
| Global variables
\*******************************/
DAC_Data_t _DAC_DataOut = {.nConfPacks = DAC_NALLCONFPACKS, .nVoltPacks = DAC_NALLVOLTPACKS, .nPacks = DAC_NALLPACKS};
DAC_Data_t _DAC_DataIn = {.nConfPacks = DAC_NALLCONFPACKS, .nVoltPacks = DAC_NALLVOLTPACKS, .nPacks = DAC_NALLPACKS};
ssiStream_t _outputStream = {.nPacks = DAC_NDACS, .nBytes = DAC_BYTES_PER_SEND, .nSize = DAC_BYTES_PER_SEND, .targetPack = -1};
ssiStream_t _inputStream = {.nPacks = DAC_NDACS, .nBytes = 0, .nSize = DAC_BYTES_PER_SEND, .targetPack = -1, .nSize = DAC_NDACS * DAC_PACK_NBYTES};
uint16_t _RxNBytesReceived = 0; // Have to be global to be able to "abort" a receive

/*******************************\
| Function definitons
\*******************************/
void dacs_init(void)
{
  ssi0_init(DAC_SSI_CLKRATE);
  dac_wipeDACData();
  dac_setupSequence();
}

void dac_wipeDACData(void)
{
  /* Wipe structure */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  dac_setToPacks(0, 0, _DAC_DataOut.Packs, _DAC_DataOut.nPacks);
  dac_setToPacks(0, 0, _DAC_DataIn.Packs, _DAC_DataIn.nPacks);
#pragma GCC diagnostic pop
}

void dac_setupSequence(void)
{
  uint8_t cmdByte = 0;
  uint16_t dataWord = 0;

  // Enable SDO
  cmdByte = DAC_WRITE | DAC_REG_ADDR_SPICONFIG;
  dataWord = DAC_SPICONF_SDOEN;
  dac_setToAllConfPacks(cmdByte, dataWord);
  dac_queryPack(dac_pack_ctrlDAC1);

  // Select Channel range +-1V
  cmdByte = DAC_WRITE | DAC_REG_ADDR_DACRANGE0;
  dataWord = DAC_DACRANGE1_DAC0(DAC_DACRANGE_10to10)    // Ch0, Ch8  range: -10V to 10V
             | DAC_DACRANGE1_DAC0(DAC_DACRANGE_10to10)  // Ch1, Ch9  range: -10V to 10V
             | DAC_DACRANGE1_DAC0(DAC_DACRANGE_10to10)  // Ch2, Ch10 range: -10V to 10V
             | DAC_DACRANGE1_DAC0(DAC_DACRANGE_10to10); // Ch3, Ch11 range: -10V to 10V
  dac_setToAllConfPacks(cmdByte, dataWord);
  dac_queryPack(dac_pack_ctrlDAC1);

  // Select Channel range +-1V
  cmdByte = DAC_WRITE | DAC_REG_ADDR_NOP;
  dataWord = DAC_NOP_DATA;
  dac_setToAllConfPacks(cmdByte, dataWord);
  dac_queryPack(dac_pack_ctrlDAC1);
}

void dac_setToAllConfPacks(uint8_t cmdByte, uint16_t dataWord)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  dac_setToPacks(cmdByte, dataWord, _DAC_DataOut.ConfPacks, _DAC_DataOut.nConfPacks);
#pragma GCC diagnostic pop
}

void dac_setToAllVoltPacks(uint8_t cmdByte, uint16_t dataWord)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  dac_setToPacks(cmdByte, dataWord, _DAC_DataOut.VoltPacks, _DAC_DataOut.nVoltPacks);
#pragma GCC diagnostic pop
}

void dac_setToPacks(uint8_t cmdByte, uint16_t dataWord, DAC_StructuralDataPack_t *packCollection, uint8_t nPacks)
{
  for (uint8_t iPack = 0; iPack < nPacks; iPack++)
  {
    packCollection[iPack].CmdByte = cmdByte;
    packCollection[iPack].Data = dataWord;
  }
}

void dac_prepareTxData(enum dac_packIndex packIndex)
{
  // Prepare Tx
  _outputStream.targetPack = packIndex;
  for (uint8_t iPack = 0; iPack < _outputStream.nPacks; iPack++)
  {
    _outputStream.Packs[iPack].CmdByte = _DAC_DataOut.Packs[_outputStream.targetPack + iPack].CmdByte;
    _outputStream.Packs[iPack].Data = swap_word(_DAC_DataOut.Packs[_outputStream.targetPack + iPack].Data);
  }
}

void dac_fetchRxData(enum dac_packIndex packIndex)
{
  // Get and convert Rx when not the first input!
  if (_inputStream.targetPack >= 0)
  {
    // Receive into input-Buffer
    // HINT! Debugger reads out SSI0->DR (removes values from FIFO!)
    while (_inputStream.nBytes < _inputStream.nSize) // Wait for the full package
      ssi0_receive(_inputStream.SerializedStream, &_inputStream.nBytes, _inputStream.nSize);
    _inputStream.nBytes = 0; // Clear received bytes for next round

    // Reconvert into datastructure
    for (uint8_t iPack = 0; iPack < _inputStream.nPacks; iPack++)
    {
      _DAC_DataIn.Packs[_inputStream.targetPack + iPack].CmdByte = _inputStream.Packs[iPack].CmdByte;
      _DAC_DataIn.Packs[_inputStream.targetPack + iPack].Data = swap_word(_inputStream.Packs[iPack].Data);
    }
  }

  _inputStream.targetPack = packIndex;
}

DAC_Data_t *dac_grabRxDataStruct(void)
{
  return &_DAC_DataIn;
}

DAC_Data_t *dac_grabTxDataStruct(void)
{
  return &_DAC_DataOut;
}

cBool dac_chipselect(cBool csState)
{
  if (ssi0_SendindStatus() == ssi0_sending_busy)
    return bFalse; // Busy, nothin happened!

  ssi0_selectDACs(csState); // Chipselect the DACs
  return bTrue;
}

void dac_chipselectBlocking(cBool csState)
{
  while (dac_chipselect(csState) == bFalse)
    ; // Wait until chip-select was done correctly
}

void dac_queryPack(enum dac_packIndex packIndex)
{
  // Prepare send-data
  dac_prepareTxData(packIndex);

  // Send current; receive previous
  dac_chipselectBlocking(bTrue);
  ssi0_transmit(_outputStream.SerializedStream, _outputStream.nBytes);
  while (ssi0_SendindStatus() == ssi0_sending_busy)
    ; // Wait for fully transmitted
  dac_chipselectBlocking(bFalse);

  // Get received-data and set the next package index
  dac_fetchRxData(packIndex);
}

// cBool dac_RxListen(cBool listenState)
// {
//   if (ssi0_SendindStatus() == ssi0_sending_busy)
//     return bFalse; // Busy, nothin happened!

//   ssi0_RxOnOff(listenState);
//   return bTrue;
// }

// void dac_RxListenBlocking(cBool listenState)
// {
//   while (dac_RxListen(listenState) == bFalse)
//     ; // Wait until RxListen was done correctly
// }

// void dac_abortRxPackage(void)
// {
//   _RxNBytesReceived = 0;
// }

// uint8_t dac_receiveRxPackage(enum dac_packIndex iPack, uint8_t nPacksExpected)
// {
//   uint16_t nBytesExpected = nPacksExpected * DAC_PACK_NBYTES;
//   uint16_t tmpRcvdBytes = 0;

//   ssi0_receive((uint8_t *)&_DAC_DataIn.Packs[iPack], &tmpRcvdBytes, nBytesExpected);
//   _RxNBytesReceived += tmpRcvdBytes; // Sum up both numbers
//   tmpRcvdBytes = _RxNBytesReceived;  //  and copy back to reuse the

//   if (_RxNBytesReceived >= nBytesExpected) // Check if pack received completely
//     _RxNBytesReceived = 0;                 //  and reset the static counter

//   return tmpRcvdBytes / DAC_PACK_NBYTES; // Return numnber of complete packs received
// }

// void dac_receiveRxPackageBlocking(enum dac_packIndex iPack, uint8_t nPacksAwait)
// {
//   while (dac_receiveRxPackage(iPack, nPacksAwait) < nPacksAwait)
//     ; // wait until all packBytes received!
// }

// void dac_transmitTxPackage(enum dac_packIndex iPack, uint8_t nPacksSend)
// {
//   uint16_t nBytesSend = nPacksSend * DAC_PACK_NBYTES;
//   ssi0_transmit((uint8_t *)&_DAC_DataOut.Packs[iPack], nBytesSend);
// }

// void dac_transmitTxPackageBlocking(enum dac_packIndex iPack, uint8_t nPacksSend)
// {
//   dac_chipselectBlocking(bOn);
//   dac_transmitTxPackage(iPack, nPacksSend);
//   dac_chipselectBlocking(bOff);
// }

// void dac_queryPackageBlocking(enum dac_packIndex iPack, uint8_t nPacks)
// {
//   dac_RxListenBlocking(bFalse);
//   dac_transmitTxPackageBlocking(iPack, nPacks);
//   dac_RxListenBlocking(bTrue);

//   // Use a dummy to receive the response of the previous command
//   dacs_setToAllConfPacks(DAC_WRITE | DAC_REG_ADDR_NOP, DAC_NOP_DATA);
//   dac_chipselectBlocking(bOn);
//   dac_transmitTxPackage(dac_pack_ctrlDAC1, nPacks);
//   dac_chipselectBlocking(bOff);
//   dac_receiveRxPackageBlocking(iPack, nPacks);
// }