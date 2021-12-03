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
void dacs_wipeDACData(void);
void dacs_setupSequence(void);
void dacs_setToAllConfPacks(uint8_t cmdByte, uint16_t dataWord);
void dacs_setToAllVoltPacks(uint8_t cmdByte, uint16_t dataWord);
void dacs_setToPacks(uint8_t cmdByte, uint16_t dataWord, DAC_StructuralDataPack_t *packCollection, uint8_t nPacks);

/*******************************\
| Global variables
\*******************************/
DAC_Data_t _DAC_DataOut = {.nConfPacks = DAC_NALLCONFPACKS, .nVoltPacks = DAC_NALLVOLTPACKS, .nPacks = DAC_NALLPACKS};
DAC_Data_t _DAC_DataIn = {.nConfPacks = DAC_NALLCONFPACKS, .nVoltPacks = DAC_NALLVOLTPACKS, .nPacks = DAC_NALLPACKS};
ssiStream_t _outputStream = {.nPacks DAC_NDACS, .nBytes = DAC_NDACS * DAC_PACK_NBYTES};
ssiStream_t _inpStream = {.nPacks DAC_NDACS, .nBytes = DAC_NDACS * DAC_PACK_NBYTES};
uint16_t _RxNBytesReceived = 0; // Have to be global to be able to "abort" a receive

/*******************************\
| Function definitons
\*******************************/
void dacs_init(void)
{
  ssi0_init(DAC_SSI_CLKRATE);
  dacs_wipeDACData();
  dacs_setupSequence();
}

void dacs_wipeDACData(void)
{
  /* Wipe structure */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  dacs_setToPacks(0, 0, _DAC_DataOut.Packs, _DAC_DataOut.nPacks);
  dacs_setToPacks(0, 0, _DAC_DataIn.Packs, _DAC_DataIn.nPacks);
#pragma GCC diagnostic pop
}

void dacs_setupSequence(void)
{
  uint8_t cmdByte = 0;
  uint16_t dataWord = 0;

  // Enable SDO
  cmdByte = DAC_WRITE | DAC_REG_ADDR_SPICONFIG;
  dataWord = DAC_SPICONF_SDOEN;
  dacs_setToAllConfPacks(cmdByte, dataWord);
  dac_RxListenBlocking(bOff);
  dac_transmitTxPackageBlocking(dac_pack_ctrlDAC1, _DAC_DataOut.nConfPacks);
  dac_RxListenBlocking(bOn);
  cmdByte &= ~DAC_WRITE;
  dacs_setToAllConfPacks(cmdByte, dataWord);
  dac_queryPackageBlocking(dac_pack_ctrlDAC1, _DAC_DataOut.nConfPacks);

  // Select Channel range +-1V
  cmdByte = DAC_WRITE | DAC_REG_ADDR_DACRANGE0;
  dataWord = DAC_DACRANGE1_DAC0(DAC_DACRANGE_10to10)    // Ch0, Ch8  range: -10V to 10V
             | DAC_DACRANGE1_DAC0(DAC_DACRANGE_10to10)  // Ch1, Ch9  range: -10V to 10V
             | DAC_DACRANGE1_DAC0(DAC_DACRANGE_10to10)  // Ch2, Ch10 range: -10V to 10V
             | DAC_DACRANGE1_DAC0(DAC_DACRANGE_10to10); // Ch3, Ch11 range: -10V to 10V
  dacs_setToAllConfPacks(cmdByte, dataWord);
  dac_RxListenBlocking(bOff);
  dac_transmitTxPackageBlocking(dac_pack_ctrlDAC1, _DAC_DataOut.nConfPacks);
  dac_RxListenBlocking(bOn);
  cmdByte &= ~DAC_WRITE;
  dacs_setToAllConfPacks(cmdByte, dataWord);
  dac_queryPackageBlocking(dac_pack_ctrlDAC1, _DAC_DataOut.nConfPacks);
}

void dacs_setToAllConfPacks(uint8_t cmdByte, uint16_t dataWord)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  dacs_setToPacks(cmdByte, dataWord, _DAC_DataOut.ConfPack, _DAC_DataOut.nConfPacks);
#pragma GCC diagnostic pop
}

void dacs_setToAllVoltPacks(uint8_t cmdByte, uint16_t dataWord)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  dacs_setToPacks(cmdByte, dataWord, _DAC_DataOut.VoltPack, _DAC_DataOut.nVoltPacks);
#pragma GCC diagnostic pop
}

void dacs_setToPacks(uint8_t cmdByte, uint16_t dataWord, DAC_StructuralDataPack_t *packCollection, uint8_t nPacks)
{
  uint16_t swappedWord = swap_word(dataWord);

  // uint16_t swappedWord = swap_word(dataWord);
  for (uint8_t iPack = 0; iPack < nPacks; iPack++)
  {
    packCollection[iPack].CmdByte = cmdByte;
    packCollection[iPack].Data = swappedWord;
  }
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

cBool dac_RxListen(cBool listenState)
{
  if (ssi0_SendindStatus() == ssi0_sending_busy)
    return bFalse; // Busy, nothin happened!

  ssi0_RxOnOff(listenState);
  return bTrue;
}

void dac_RxListenBlocking(cBool listenState)
{
  while (dac_RxListen(listenState) == bFalse)
    ; // Wait until RxListen was done correctly
}

void dac_abortRxPackage(void)
{
  _RxNBytesReceived = 0;
}

uint8_t dac_receiveRxPackage(enum dac_packIndex iPack, uint8_t nPacksExpected)
{
  uint16_t nBytesExpected = nPacksExpected * DAC_PACK_NBYTES;
  uint16_t tmpRcvdBytes = 0;

  ssi0_receive((uint8_t *)&_DAC_DataIn.Packs[iPack], &tmpRcvdBytes, nBytesExpected);
  _RxNBytesReceived += tmpRcvdBytes; // Sum up both numbers
  tmpRcvdBytes = _RxNBytesReceived;  //  and copy back to reuse the

  if (_RxNBytesReceived >= nBytesExpected) // Check if pack received completely
    _RxNBytesReceived = 0;                 //  and reset the static counter

  return tmpRcvdBytes / DAC_PACK_NBYTES; // Return numnber of complete packs received
}

void dac_receiveRxPackageBlocking(enum dac_packIndex iPack, uint8_t nPacksAwait)
{
  while (dac_receiveRxPackage(iPack, nPacksAwait) < nPacksAwait)
    ; // wait until all packBytes received!
}

void dac_transmitTxPackage(enum dac_packIndex iPack, uint8_t nPacksSend)
{
  uint16_t nBytesSend = nPacksSend * DAC_PACK_NBYTES;
  ssi0_transmit((uint8_t *)&_DAC_DataOut.Packs[iPack], nBytesSend);
}

void dac_transmitTxPackageBlocking(enum dac_packIndex iPack, uint8_t nPacksSend)
{
  dac_chipselectBlocking(bOn);
  dac_transmitTxPackage(iPack, nPacksSend);
  dac_chipselectBlocking(bOff);
}

void dac_queryPackageBlocking(enum dac_packIndex iPack, uint8_t nPacks)
{
  dac_RxListenBlocking(bFalse);
  dac_transmitTxPackageBlocking(iPack, nPacks);
  dac_RxListenBlocking(bTrue);

  // Use a dummy to receive the response of the previous command
  dacs_setToAllConfPacks(DAC_WRITE | DAC_REG_ADDR_NOP, DAC_NOP_DATA);
  dac_chipselectBlocking(bOn);
  dac_transmitTxPackage(dac_pack_ctrlDAC1, nPacks);
  dac_chipselectBlocking(bOff);
  dac_receiveRxPackageBlocking(iPack, nPacks);
}