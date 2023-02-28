/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "math.h"
#include "stdio.h"

/* Project specific */
#include "common.h"
#include "dacWrapper.h"
#include "ssi0_DACs.h"
// #include "preciseTime.h"
#include "DelayTimer.h"
#include "swap.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region DAC Hardwaredefines

#define DAC_READ BIT(7) // Do nothing when reading from DAC
#define DAC_WRITE 0x00  // Set when writing to DAC-Registers
#pragma region DAC Register Addresses
#define DAC_REG_ADDR_NOP OPTION(0x00, 0)         // Write-Only
#define DAC_REG_ADDR_DEVICEID OPTION(0x01, 0)    // Read-Only
#define DAC_REG_ADDR_STATUS OPTION(0x02, 0)      // Read-Only
#define DAC_REG_ADDR_SPICONFIG OPTION(0x03, 0)   // R/W
#define DAC_REG_ADDR_GENCONFIG OPTION(0x04, 0)   // R/W
#define DAC_REG_ADDR_BRDCONFIG OPTION(0x05, 0)   // R/W
#define DAC_REG_ADDR_SYNCCONFIG OPTION(0x06, 0)  // R/W
#define DAC_REG_ADDR_TOGGCONFIG0 OPTION(0x07, 0) // R/W
#define DAC_REG_ADDR_TOGGCONFIG1 OPTION(0x08, 0) // R/W
#define DAC_REG_ADDR_DACPWDWN OPTION(0x09, 0)    // R/W
#define DAC_REG_ADDR_DACRANGE0 OPTION(0x0B, 0)   // Write-Only
#define DAC_REG_ADDR_DACRANGE1 OPTION(0x0C, 0)   // Write-Only
#define DAC_REG_ADDR_TRIGGER OPTION(0x0E, 0)     // Write-Only
#define DAC_REG_ADDR_BRDCAST OPTION(0x0F, 0)     // R/W

#define DAC_REG_ADDR_CH_OFFSET 0x14
#define DAC_REG_ADDR_DAC0 OPTION(DAC_REG_ADDR_CH_OFFSET + 0, 0) // R/W
#define DAC_REG_ADDR_DAC1 OPTION(DAC_REG_ADDR_CH_OFFSET + 1, 0) // R/W
#define DAC_REG_ADDR_DAC2 OPTION(DAC_REG_ADDR_CH_OFFSET + 2, 0) // R/W
#define DAC_REG_ADDR_DAC3 OPTION(DAC_REG_ADDR_CH_OFFSET + 3, 0) // R/W
#define DAC_REG_ADDR_DAC4 OPTION(DAC_REG_ADDR_CH_OFFSET + 4, 0) // R/W
#define DAC_REG_ADDR_DAC5 OPTION(DAC_REG_ADDR_CH_OFFSET + 5, 0) // R/W
#define DAC_REG_ADDR_DAC6 OPTION(DAC_REG_ADDR_CH_OFFSET + 6, 0) // R/W
#define DAC_REG_ADDR_DAC7 OPTION(DAC_REG_ADDR_CH_OFFSET + 7, 0) // R/W
#define DAC_REG_ADDR_OFFSET0 OPTION(0x21, 0)                    // R/W
#define DAC_REG_ADDR_OFFSET1 OPTION(0x22, 0)                    // R/W
#pragma endregion DAC Register Addresses

#pragma region DAC Register Bits
#define DAC_NOP_DATA 0x00 // Always use this when writing to NOP (e.g. reading data)

#define DAC_DEVICEID_VERSIONID(x) (x & 0x3)      // Parse out Version-ID
#define DAC_DEVICEID_DEVICEID(x) SHIFTDOWN(x, 2) // Parse out Device-ID

#define DAC_SR_TEMPALM BIT(0) // Temperature-Alarm bit
#define DAC_SR_DACBUSY BIT(1) // DAC-Busy bit
#define DAC_SR_CRCALM BIT(2)  // CRC-Alarm bit

#define DAC_SPICONF_FSDO BIT(1)       // Fast SDO bit (1 = update SDO on rising edge; 0 = update SDO on falling edge)
#define DAC_SPICONF_SDOEN BIT(2)      // DAC SDO enable
#define DAC_SPICONF_STREN BIT(3)      // Streaming-Mode enable
#define DAC_SPICONF_CRCEN BIT(4)      // CRC-Check enable
#define DAC_SPICONF_DEVPWDWN BIT(5)   // Device-PowerDown (1 = device in power-down; 0 = device active)
#define DAC_SPICONF_SOFTTGLEN BIT(6)  // Soft-Toggle enable
#define DAC_SPICONF_CRCALMEN BIT(9)   // CRC-Alarm enable
#define DAC_SPICONF_DACBUSYEN BIT(10) // DAC-Busy enable
#define DAC_SPICONF_TEMPALMEN BIT(11) // DAC Temperature-Alarm enable

#define DAC_GENCONFIG_DAC_0_1_DIFF_EN BIT(2) // Enable output 0 & 1 as differential pair
#define DAC_GENCONFIG_DAC_2_3_DIFF_EN BIT(3) // Enable output 2 & 3 as differential pair
#define DAC_GENCONFIG_DAC_4_5_DIFF_EN BIT(4) // Enable output 4 & 5 as differential pair
#define DAC_GENCONFIG_DAC_6_7_DIFF_EN BIT(5) // Enable output 6 & 7 as differential pair
#define DAC_GENCONFIG_REF_PWDWN BIT(14)      // '1' = Disable internal reference

#define DAC_BRDCONFIG_DAC0_BRD_EN BIT(4)  // Enable DAC-Ch0 to be broadcastet by writing a value to BRDCAST
#define DAC_BRDCONFIG_DAC1_BRD_EN BIT(5)  // Enable DAC-Ch1 to be broadcastet by writing a value to BRDCAST
#define DAC_BRDCONFIG_DAC2_BRD_EN BIT(6)  // Enable DAC-Ch2 to be broadcastet by writing a value to BRDCAST
#define DAC_BRDCONFIG_DAC3_BRD_EN BIT(7)  // Enable DAC-Ch3 to be broadcastet by writing a value to BRDCAST
#define DAC_BRDCONFIG_DAC4_BRD_EN BIT(8)  // Enable DAC-Ch4 to be broadcastet by writing a value to BRDCAST
#define DAC_BRDCONFIG_DAC5_BRD_EN BIT(9)  // Enable DAC-Ch5 to be broadcastet by writing a value to BRDCAST
#define DAC_BRDCONFIG_DAC6_BRD_EN BIT(10) // Enable DAC-Ch6 to be broadcastet by writing a value to BRDCAST
#define DAC_BRDCONFIG_DAC7_BRD_EN BIT(11) // Enable DAC-Ch7 to be broadcastet by writing a value to BRDCAST

#define DAC_SYNCCONFIG_DAC0_SYNC_EN BIT(4)  // '1' = Enable DAC-Ch0 to update synchroniously by triggering LDAC; '0' = Update asynchrone
#define DAC_SYNCCONFIG_DAC1_SYNC_EN BIT(5)  // '1' = Enable DAC-Ch1 to update synchroniously by triggering LDAC; '0' = Update asynchrone
#define DAC_SYNCCONFIG_DAC2_SYNC_EN BIT(6)  // '1' = Enable DAC-Ch2 to update synchroniously by triggering LDAC; '0' = Update asynchrone
#define DAC_SYNCCONFIG_DAC3_SYNC_EN BIT(7)  // '1' = Enable DAC-Ch3 to update synchroniously by triggering LDAC; '0' = Update asynchrone
#define DAC_SYNCCONFIG_DAC4_SYNC_EN BIT(8)  // '1' = Enable DAC-Ch4 to update synchroniously by triggering LDAC; '0' = Update asynchrone
#define DAC_SYNCCONFIG_DAC5_SYNC_EN BIT(9)  // '1' = Enable DAC-Ch5 to update synchroniously by triggering LDAC; '0' = Update asynchrone
#define DAC_SYNCCONFIG_DAC6_SYNC_EN BIT(10) // '1' = Enable DAC-Ch6 to update synchroniously by triggering LDAC; '0' = Update asynchrone
#define DAC_SYNCCONFIG_DAC7_SYNC_EN BIT(11) // '1' = Enable DAC-Ch7 to update synchroniously by triggering LDAC; '0' = Update asynchrone

#define DAC_TOGGCONFIG_TOGGLEMODE_DISABLED 0x00          // Toggle disabled
#define DAC_TOGGCONFIG_TOGGLEMODE_TOGGLE0 0x01           // Toggle assigned to Toggle0 bit
#define DAC_TOGGCONFIG_TOGGLEMODE_TOGGLE1 0x02           // Toggle assigned to Toggle1 bit
#define DAC_TOGGCONFIG_TOGGLEMODE_TOGGLE2 0x03           // Toggle assigned to Toggle2 bit
#define DAC_TOGGCONFIG1_DAC0_TOGGLEMODE(x) OPTION(x, 8)  // Enable Toggle mode to corresponding Toggle-bit in TRIGGER-Register
#define DAC_TOGGCONFIG1_DAC1_TOGGLEMODE(x) OPTION(x, 10) // Enable Toggle mode to corresponding Toggle-bit in TRIGGER-Register
#define DAC_TOGGCONFIG1_DAC2_TOGGLEMODE(x) OPTION(x, 12) // Enable Toggle mode to corresponding Toggle-bit in TRIGGER-Register
#define DAC_TOGGCONFIG1_DAC3_TOGGLEMODE(x) OPTION(x, 14) // Enable Toggle mode to corresponding Toggle-bit in TRIGGER-Register
#define DAC_TOGGCONFIG0_DAC4_TOGGLEMODE(x) OPTION(x, 0)  // Enable Toggle mode to corresponding Toggle-bit in TRIGGER-Register
#define DAC_TOGGCONFIG0_DAC5_TOGGLEMODE(x) OPTION(x, 2)  // Enable Toggle mode to corresponding Toggle-bit in TRIGGER-Register
#define DAC_TOGGCONFIG0_DAC6_TOGGLEMODE(x) OPTION(x, 4)  // Enable Toggle mode to corresponding Toggle-bit in TRIGGER-Register
#define DAC_TOGGCONFIG0_DAC7_TOGGLEMODE(x) OPTION(x, 6)  // Enable Toggle mode to corresponding Toggle-bit in TRIGGER-Register

#define DAC_DACPWDWN_DAC0 BIT(4)  // '1' = Enable / '0' = Disable corresponding DAC-output
#define DAC_DACPWDWN_DAC1 BIT(5)  // '1' = Enable / '0' = Disable corresponding DAC-output
#define DAC_DACPWDWN_DAC2 BIT(6)  // '1' = Enable / '0' = Disable corresponding DAC-output
#define DAC_DACPWDWN_DAC3 BIT(7)  // '1' = Enable / '0' = Disable corresponding DAC-output
#define DAC_DACPWDWN_DAC4 BIT(8)  // '1' = Enable / '0' = Disable corresponding DAC-output
#define DAC_DACPWDWN_DAC5 BIT(9)  // '1' = Enable / '0' = Disable corresponding DAC-output
#define DAC_DACPWDWN_DAC6 BIT(10) // '1' = Enable / '0' = Disable corresponding DAC-output
#define DAC_DACPWDWN_DAC7 BIT(11) // '1' = Enable / '0' = Disable corresponding DAC-output

#define DAC_DACRANGE_0to5 0b0000             // Range: 0V to 5V
#define DAC_DACRANGE_0to10 0b0001            // Range: 0V to 10V
#define DAC_DACRANGE_0to20 0b0010            // Range: 0V to 20V
#define DAC_DACRANGE_0to40 0b0100            // Range: 0V to 40V
#define DAC_DACRANGE_5to5 0b1001             // Range: -5V to 5V
#define DAC_DACRANGE_10to10 0b1010           // Range: -10V to 10V
#define DAC_DACRANGE_20to20 0b1100           // Range: -20V to 20V
#define DAC_DACRANGE_2p5to2p5 0b1110         // Range: -2.5V to 2.5V
#define DAC_DACRANGE_DAC0_4(x) OPTION(x, 0)  // Set the DAC-Range for the corresponding DAC-Ch
#define DAC_DACRANGE_DAC1_5(x) OPTION(x, 4)  // Set the DAC-Range for the corresponding DAC-Ch
#define DAC_DACRANGE_DAC2_6(x) OPTION(x, 8)  // Set the DAC-Range for the corresponding DAC-Ch
#define DAC_DACRANGE_DAC3_7(x) OPTION(x, 12) // Set the DAC-Range for the corresponding DAC-Ch

#define DAC_TRIGGER_SOFTRST OPTION(0b1010, 0) // When written into TRIGGER the device resets to default state
#define DAC_TRIGGER_LDAC BIT(4)               // '1' = Channels configurated in SYNCCONFIG are updated sychronously
#define DAC_TRIGGER_TOG0 BIT(5)               // '1' = Toggles the corresponding Channels configurated in TOGGCONF
#define DAC_TRIGGER_TOG1 BIT(6)               // '1' = Toggles the corresponding Channels configurated in TOGGCONF
#define DAC_TRIGGER_TOG2 BIT(7)               // '1' = Toggles the corresponding Channels configurated in TOGGCONF
#define DAC_TRIGGER_ALMRST BIT(8)             // '1' = Resets the event-alarm output

#pragma endregion DAC Register Bits

#pragma endregion DAC Hardwaredefines

/*******************************\
| Local Enum/Struct/Union
\*******************************/

/*******************************\
| Local function declarations
\*******************************/
void dac_wipeDACData(void);
void dac_setupSequence(void);
void dac_setCPacks(uint8_t cmdByte, uint16_t dataWord);
void dac_setVPacks(uint8_t cmdByte, uint16_t dataWord);
// void dac_setToPacks(uint8_t cmdByte, uint16_t dataWord, DAC_StructuralDataPack_t *packCollection, uint8_t nPacks);

enum dac_packIndex dac_IndexInt2EnumPackIndex(uint16_t iPackIndex);
uint16_t dac_IndexEnumPackIndex2Int(enum dac_packIndex packIndex);
void dac_prepareTxData(enum dac_packIndex packIndex);
void dac_fetchRxData(enum dac_packIndex packIndex);

uint16_t dac_selectVoltRange(uint16_t chRange);
uint16_t dac_convertFloatTo16BitRange(float voltage);

/*******************************\
| Global variables
\*******************************/
DAC_Data_t _DAC_DataOut = {.nCPacks = DAC_NALLCONFPACKS, .nVPacks = DAC_NALLVOLTPACKS, .nPacks = DAC_NALLPACKS};
DAC_Data_t _DAC_DataIn = {.nCPacks = DAC_NALLCONFPACKS, .nVPacks = DAC_NALLVOLTPACKS, .nPacks = DAC_NALLPACKS};
dac_ssiStream_t _outputStream = {.nPacks = DAC_NDACS, .nBytes = DAC_BYTES_PER_SEND, .nSize = DAC_BYTES_PER_SEND, .targetPack = -1};
dac_ssiStream_t _inputStream = {.nPacks = DAC_NDACS, .nBytes = 0, .nSize = DAC_BYTES_PER_SEND, .targetPack = -1, .nSize = DAC_NDACS * DAC_PACK_NBYTES};
uint16_t _RxNBytesReceived = 0; // Have to be global to be able to "abort" a receive

float _dacVoltSpan = 0; // Used to determine the DAC-register-value

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
  // #pragma GCC diagnostic push
  // #pragma GCC diagnostic ignored "-Waddress-of-packed-member"

  dac_setCPacks(0, 0);
  dac_setVPacks(0, 0);
  // #pragma GCC diagnostic pop
}

void dac_setCPacks(uint8_t cmdByte, uint16_t dataWord)
{
  for (uint8_t iConfPack = 0; iConfPack < _DAC_DataOut.nCPacks; iConfPack++)
  {
    _DAC_DataOut.CPacks[iConfPack].CmdByte = cmdByte;
    _DAC_DataOut.CPacks[iConfPack].Data = dataWord;
  }
  // #pragma GCC diagnostic push
  // #pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  //   dac_setToPacks(cmdByte, dataWord, _DAC_DataOut.ConfPacks, _DAC_DataOut.nCPacks);
  // #pragma GCC diagnostic pop
}

/* ATTENTION
 * This method grabs the full cmdByte (uint8_t) but only uses the R/W flag of the byte!
 */
void dac_setVPacks(uint8_t rwFlag, uint16_t dataWord)
{
  uint8_t cmdByte;
  rwFlag &= DAC_READ;
  for (uint16_t iVoltCh = 0; iVoltCh < DAC_VOLTPACKS; iVoltCh++)
  {
    cmdByte = rwFlag | (DAC_REG_ADDR_CH_OFFSET + iVoltCh);
    for (uint8_t iDAC = 0; iDAC < DAC_NDACS; iDAC++)
    {
      _DAC_DataOut.VPacks[iVoltCh + iDAC * DAC_VOLTPACKS].CmdByte = cmdByte;
      _DAC_DataOut.VPacks[iVoltCh + iDAC * DAC_VOLTPACKS].Data = dataWord;
    }
  }
  // #pragma GCC diagnostic push
  // #pragma GCC diagnostic ignored "-Waddress-of-packed-member"
  //   dac_setToPacks(cmdByte, dataWord, _DAC_DataOut.VoltPacks, _DAC_DataOut.nVPacks);
  // #pragma GCC diagnostic pop
}

// void dac_setToPacks(uint8_t cmdByte, uint16_t dataWord, DAC_StructuralDataPack_t *packCollection, uint8_t nPacks)
// {
//   for (uint8_t iPack = 0; iPack < nPacks; iPack++)
//   {
//     packCollection[iPack].CmdByte = cmdByte;
//     packCollection[iPack].Data = dataWord;
//   }
// }

void dac_setupSequence(void)
{
  uint8_t cmdByte = 0;
  uint16_t dataWord = 0;


  // pTime_wait(2000); // POR needs 1ms to "boot" up; Wait >1ms
  Delay_Await(2); // POR needs 1ms to "boot" up; Wait >1ms

  // // Soft-Reset
  // cmdByte = DAC_WRITE | DAC_REG_ADDR_TRIGGER;
  // dataWord = DAC_TRIGGER_SOFTRST;
  // dac_setToAllConfPacks(cmdByte, dataWord);
  // dac_queryPack(dac_pack_ctrlDAC0);

  // pTime_wait(200); // SoftReset triggers a POR (wait >1ms)
  // Delay_Await(2); // SoftReset triggers a POR (wait >1ms)

  // Enable SDO; No Alarms; No powerdown
  cmdByte = DAC_WRITE | DAC_REG_ADDR_SPICONFIG;
  dataWord = DAC_SPICONF_SDOEN;
  dac_setCPacks(cmdByte, dataWord);
  dac_queryPack(dac_cPack_DAC0);

  // Internal VRef on; No Differntial Pairs
  cmdByte = DAC_WRITE | DAC_REG_ADDR_GENCONFIG;
  dataWord = 0x00;
  dac_setCPacks(cmdByte, dataWord);
  dac_queryPack(dac_cPack_DAC0);

  // No broadcasts
  cmdByte = DAC_WRITE | DAC_REG_ADDR_BRDCONFIG;
  dataWord = DAC_NOP_DATA; // = 0x00; Setting bits to 1 disables the outputs (10k resistor to GND)
  dac_setCPacks(cmdByte, dataWord);
  dac_queryPack(dac_cPack_DAC0);

  // No synced DACs
  cmdByte = DAC_WRITE | DAC_REG_ADDR_SYNCCONFIG;
  dataWord = DAC_NOP_DATA; // = 0x00; Setting bits to 1 disables the outputs (10k resistor to GND)
  dac_setCPacks(cmdByte, dataWord);
  dac_queryPack(dac_cPack_DAC0);

  // Disable all AB-Toggles
  cmdByte = DAC_WRITE | DAC_REG_ADDR_TOGGCONFIG0;
  dataWord = DAC_NOP_DATA; // = 0x00; Setting bits to 1 disables the outputs (10k resistor to GND)
  dac_setCPacks(cmdByte, dataWord);
  dac_queryPack(dac_cPack_DAC0);
  cmdByte = DAC_WRITE | DAC_REG_ADDR_TOGGCONFIG1;
  dataWord = DAC_NOP_DATA; // = 0x00; Setting bits to 1 disables the outputs (10k resistor to GND)
  dac_setCPacks(cmdByte, dataWord);
  dac_queryPack(dac_cPack_DAC0);

  // Select Channel range +-10V
  uint16_t range = dac_selectVoltRange(DAC_DACRANGE_10to10);
  // Register RANGE1 (DACs 0-3)
  cmdByte = DAC_WRITE | DAC_REG_ADDR_DACRANGE1;
  dataWord = DAC_DACRANGE_DAC0_4(range)    // Ch0, Ch8  range
             | DAC_DACRANGE_DAC1_5(range)  // Ch1, Ch9  range
             | DAC_DACRANGE_DAC2_6(range)  // Ch2, Ch10 range
             | DAC_DACRANGE_DAC3_7(range); // Ch3, Ch11 range
  dac_setCPacks(cmdByte, dataWord);
  dac_queryPack(dac_cPack_DAC0);
  // Register RANGE0 (DACs 4-7)
  cmdByte = DAC_WRITE | DAC_REG_ADDR_DACRANGE0;
  dac_setCPacks(cmdByte, dataWord);
  dac_queryPack(dac_cPack_DAC0);

  // Prepare 0V to all DACs
  dac_setVPacks(DAC_WRITE, dac_convertFloatTo16BitRange(0.0f));
  dac_queryPack(dac_vPack_Ch0); // Sends also the DAC1 package (daisy-chain)
  dac_queryPack(dac_vPack_Ch1); // Sends also the DAC1 package (daisy-chain)
  dac_queryPack(dac_vPack_Ch2); // Sends also the DAC1 package (daisy-chain)
  dac_queryPack(dac_vPack_Ch3); // Sends also the DAC1 package (daisy-chain)
  dac_queryPack(dac_vPack_Ch4); // Sends also the DAC1 package (daisy-chain)
  dac_queryPack(dac_vPack_Ch5); // Sends also the DAC1 package (daisy-chain)
  dac_queryPack(dac_vPack_Ch6); // Sends also the DAC1 package (daisy-chain)
  dac_queryPack(dac_vPack_Ch7); // Sends also the DAC1 package (daisy-chain)

  // Disable powerdown of DAC-Outputs
  cmdByte = DAC_WRITE | DAC_REG_ADDR_DACPWDWN;
  dataWord = DAC_NOP_DATA; // = 0x00; Setting bits to 1 disables the outputs (10k resistor to GND)
  dac_setCPacks(cmdByte, dataWord);
  dac_queryPack(dac_cPack_DAC0);
}

uint16_t dac_selectVoltRange(uint16_t chRange)
{
  switch (chRange)
  {
  case DAC_DACRANGE_0to5:
    _dacVoltSpan = 5.0f;
    break;

  case DAC_DACRANGE_0to10:
    _dacVoltSpan = 10.0f;
    break;

  case DAC_DACRANGE_0to20:
    _dacVoltSpan = 20.0f;
    break;

  case DAC_DACRANGE_0to40:
    _dacVoltSpan = 40.0f;
    break;

  case DAC_DACRANGE_5to5:
    _dacVoltSpan = 10.0f;
    break;

    // case DAC_DACRANGE_10to10: // Used as default!
    //   _dacVoltSpan = 20.0f;
    //   break;

  case DAC_DACRANGE_20to20:
    _dacVoltSpan = 40.0f;
    break;

  case DAC_DACRANGE_2p5to2p5:
    _dacVoltSpan = 5.0f;
    break;

  default: // +-10V
    _dacVoltSpan = 20.0f;
    chRange = DAC_DACRANGE_10to10;
    break;
  }

  return chRange;
}

void dac_prepareTxData(enum dac_packIndex packIndex)
{
  // Determine package depending offset values
  int16_t dataOffMul = 0;
  int16_t dataOffAdd = 0;
  if (packIndex < dac_vPacks_Count)
  {
    dataOffMul = DAC_VOLTPACKS;
    dataOffAdd = 0;
  }
  else
  {
    dataOffMul = DAC_CONFPACKS;
    dataOffAdd = dac_cPack_DAC0;
  }

  // Prepare Tx
  // Packs 0-7 has to be stored at last in the stream, because thats what the first DAC will get as data
  _outputStream.targetPack = packIndex;
  enum dac_packIndex dataStartPack = dataOffAdd + (packIndex % dataOffMul); // Determine starterpackage
  uint8_t iMaxPack = _outputStream.nPacks - 1;
  for (int iPack = iMaxPack; iPack >= 0; iPack--)
  // for (int iPack = 0; iPack < _outputStream.nPacks; iPack++)
  {
    enum dac_packIndex dataPack = dataStartPack + (iMaxPack - iPack) * dataOffMul;
    _outputStream.Packs[iPack].CmdByte = _DAC_DataOut.Packs[dataPack].CmdByte;
    // _outputStream.Packs[iPack].Data = swap_word(_DAC_DataOut.Packs[_outputStream.targetPack + iPack].Data);
    _outputStream.Packs[iPack].Data = swap_word(_DAC_DataOut.Packs[dataPack].Data);
  }
}

void dac_fetchRxData(enum dac_packIndex packIndex)
{
  // Get and convert Rx by one package shifted! (Sending new package leads to the answer of the previous one!)
  // Target-pack assignment at function-end (collapse/expand following if!)
  if (_inputStream.targetPack < _DAC_DataIn.nPacks)
  {
    // Receive into input-Buffer
    // HINT! Debugger reads out SSI0->DR (removes values from FIFO!)
    // while (_inputStream.nBytes < _inputStream.nSize) // Wait for the full package
    //   ssi_receive8Bit(SSI0, _inputStream.SerializedStream, &_inputStream.nBytes, _inputStream.nSize);

    // Determine package depending offset values
    int16_t dataOffMul = 0;
    int16_t dataOffAdd = 0;
    if (packIndex < dac_vPacks_Count)
    {
      dataOffMul = DAC_VOLTPACKS;
      dataOffAdd = 0;
    }
    else
    {
      dataOffMul = DAC_CONFPACKS;
      dataOffAdd = dac_cPack_DAC0;
    }

    enum dac_packIndex dataStartPack = dataOffAdd + (packIndex % dataOffMul); // Determine starterpackage
    // Reconvert into datastructure
    for (int iPack = 0; iPack < _inputStream.nPacks; iPack++)
    {
      enum dac_packIndex dataPack = dataStartPack + iPack * dataOffMul;
      // uint8_t iDataInPack = (_inputStream.nPacks - 1) - iPack;
      _DAC_DataIn.Packs[dataPack].CmdByte = _inputStream.Packs[iPack].CmdByte;
      // _DAC_DataIn.Packs[_inputStream.targetPack + iPack].Data = swap_word(_inputStream.Packs[iPack].Data);
      _DAC_DataIn.Packs[dataPack].Data = swap_word(_inputStream.Packs[iPack].Data);
    }

    _inputStream.nBytes = 0; // Everything fetched -> Clear received bytes for next round
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
  if (ssi_SendindStatus(SSI0) == ssi_sending_busy)
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
  // ssi_transmit8Bit(SSI0, _outputStream.SerializedStream, _outputStream.nBytes);
  ssi_transceive8Bit(SSI0, _outputStream.SerializedStream, _inputStream.SerializedStream, _outputStream.nBytes);
  // while (ssi_SendindStatus(SSI0) == ssi_sending_busy)
  //   ; // Wait for fully transmitted
  dac_chipselectBlocking(bFalse);

  // Get received-data and set the next package index
  dac_fetchRxData(packIndex);
}

void dac_setChVoltage(uint16_t channel, float voltage)
{
  dac_updateChVoltage(channel, voltage);
  dac_sendChVoltage(channel);
}

void dac_updateChVoltage(uint16_t channel, float voltage)
{
  if (channel >= DAC_NALLVOLTPACKS) // Out of range
    return;

  DAC_Data_t *tx = dac_grabTxDataStruct();

  tx->VPacks[channel].CmdByte = DAC_WRITE | ((channel % DAC_VOLTPACKS) + DAC_REG_ADDR_CH_OFFSET); // Keep channel in Range and add the ch-offset
  tx->VPacks[channel].Data = dac_convertFloatTo16BitRange(voltage);
}

void dac_sendChVoltage(uint16_t channel)
{
  dac_queryPack(channel);
}

uint16_t dac_convertFloatTo16BitRange(float voltage)
{
  float _dacVoltPeak = _dacVoltSpan / 2; // PeakPeak to single-side Peak

  // Check is given voltage out of range and keep in range
  if (fabs(voltage) > _dacVoltPeak)
  {
    if (voltage < 0)
      voltage = -_dacVoltPeak;
    else
      voltage = _dacVoltPeak;
  }

  /* Conversion-formula
   * DAC-Value 0 results in minimum value (-RANGE) -> Simple half-span shift and conversion by the rule of proportion
   *
   * Therefore when 0V is given it has to be shifted by the half range-span
   * VShift = voltage + (VSpan/2)
   *
   * normalized by the span
   * VNorm = VShift / VSpan
   *
   * and converted into a 16-bit value
   * 16Bit-DAC-Val = VNorm * 0xFFFF
   */
  return (uint16_t)(((voltage + _dacVoltPeak) / _dacVoltSpan) * 0xFFFF); // convert into 16bit-value
}

// cBool dac_RxListen(cBool listenState)
// {
//   if (ssi0_SendindStatus() == ssi_sending_busy)
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
