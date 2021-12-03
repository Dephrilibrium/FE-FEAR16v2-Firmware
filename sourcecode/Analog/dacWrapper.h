#ifndef DACWRAPPER_H
#define DACWRAPPER_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "common.h"
#include "ssi0_DACs.h"

/*******************************\
| Defines
\*******************************/
// DAC Datastructure
#define DAC_NDACS 2       // Amount of DACs in daisy-chain
#define DAC_CONFPACKS 1   // Amoutn of config-packs
#define DAC_VOLTPACKS 8   // Amount of outputvoltage-packs
#define DAC_PACK_NBYTES 3 // Bytes per data-pack

#define DAC_NALLCONFPACKS (DAC_NDACS * DAC_CONFPACKS)
#define DAC_NALLVOLTPACKS (DAC_NDACS * DAC_VOLTPACKS)
#define DAC_NALLPACKS (DAC_NALLCONFPACKS + DAC_NALLVOLTPACKS) // Amount of all packs
#define DAC_ALLPACKS_NBYTES (DAC_NALLPACKS * DAC_PACK_NBYTES) // Amount of bytes for all data-packs. Be carefull when updating code for more than 2 daisychain-DACs. Struct not using this calculation to modify the entire struct!

// DAC
#define DAC_SSI_CLKRATE ssi0_clkRate_1MHz

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
#define DAC_REG_ADDR_DAC0 OPTION(0x14, 0)        // R/W
#define DAC_REG_ADDR_DAC1 OPTION(0x15, 0)        // R/W
#define DAC_REG_ADDR_DAC2 OPTION(0x16, 0)        // R/W
#define DAC_REG_ADDR_DAC3 OPTION(0x17, 0)        // R/W
#define DAC_REG_ADDR_DAC4 OPTION(0x18, 0)        // R/W
#define DAC_REG_ADDR_DAC5 OPTION(0x19, 0)        // R/W
#define DAC_REG_ADDR_DAC6 OPTION(0x1A, 0)        // R/W
#define DAC_REG_ADDR_DAC7 OPTION(0x1B, 0)        // R/W
#define DAC_REG_ADDR_OFFSET0 OPTION(0x21, 0)     // R/W
#define DAC_REG_ADDR_OFFSET1 OPTION(0x22, 0)     // R/W
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

#define DAC_DACRANGE_0to5 0b0000            // Range: 0V to 5V
#define DAC_DACRANGE_0to10 0b0001           // Range: 0V to 10V
#define DAC_DACRANGE_0to20 0b0010           // Range: 0V to 20V
#define DAC_DACRANGE_0to40 0b0100           // Range: 0V to 40V
#define DAC_DACRANGE_5to5 0b1001            // Range: -5V to 5V
#define DAC_DACRANGE_10to10 0b1010          // Range: -10V to 10V
#define DAC_DACRANGE_20to20 0b1100          // Range: -20V to 20V
#define DAC_DACRANGE_2p5to2p5 0b1110        // Range: -2.5V to 2.5V
#define DAC_DACRANGE1_DAC0(x) OPTION(x, 0)  // Set the DAC-Range for the corresponding DAC-Ch
#define DAC_DACRANGE1_DAC1(x) OPTION(x, 4)  // Set the DAC-Range for the corresponding DAC-Ch
#define DAC_DACRANGE1_DAC2(x) OPTION(x, 8)  // Set the DAC-Range for the corresponding DAC-Ch
#define DAC_DACRANGE1_DAC3(x) OPTION(x, 12) // Set the DAC-Range for the corresponding DAC-Ch
#define DAC_DACRANGE0_DAC4(x) OPTION(x, 0)  // Set the DAC-Range for the corresponding DAC-Ch
#define DAC_DACRANGE0_DAC5(x) OPTION(x, 4)  // Set the DAC-Range for the corresponding DAC-Ch
#define DAC_DACRANGE0_DAC6(x) OPTION(x, 8)  // Set the DAC-Range for the corresponding DAC-Ch
#define DAC_DACRANGE0_DAC7(x) OPTION(x, 12) // Set the DAC-Range for the corresponding DAC-Ch

#define DAC_TRIGGER_SOFTRST OPTION(0b1010, 0) // When written into TRIGGER the device resets to default state
#define DAC_TRIGGER_LDAC BIT(4)               // '1' = Channels configurated in SYNCCONFIG are updated sychronously
#define DAC_TRIGGER_TOG0 BIT(5)               // '1' = Toggles the corresponding Channels configurated in TOGGCONF
#define DAC_TRIGGER_TOG1 BIT(6)               // '1' = Toggles the corresponding Channels configurated in TOGGCONF
#define DAC_TRIGGER_TOG2 BIT(7)               // '1' = Toggles the corresponding Channels configurated in TOGGCONF
#define DAC_TRIGGER_ALMRST BIT(8)             // '1' = Resets the event-alarm output

#pragma endregion DAC Register Bits

/*******************************\
| Enum/Struct/Union
\*******************************/
enum dac_packIndex
{
  // Indicies for "ConfPacks"
  dac_confPack_DAC1_0 = 0, // Pack-Offset for confPack-Array: DAC1 & DAC0

  // Indicies for "VoltPacks"
  dac_voltPack_CH8_0 = 0,   // Pack-Offset for voltPack-Array: Ch 8 & Ch0
  dac_voltPack_CH9_1 = 2,   // Pack-Offset for voltPack-Array: Ch 9 & Ch1
  dac_voltPack_CH10_2 = 4,  // Pack-Offset for voltPack-Array: Ch10 & Ch2
  dac_voltPack_CH11_3 = 6,  // Pack-Offset for voltPack-Array: Ch11 & Ch3
  dac_voltPack_CH12_4 = 8,  // Pack-Offset for voltPack-Array: Ch12 & Ch4
  dac_voltPack_CH13_5 = 10, // Pack-Offset for voltPack-Array: Ch13 & Ch5
  dac_voltPack_CH14_6 = 12, // Pack-Offset for voltPack-Array: Ch14 & Ch6
  dac_voltPack_CH15_7 = 14, // Pack-Offset for voltPack-Array: Ch15 & Ch7

  // Indicies for "Packs"
  dac_pack_CH8_0 = 0,       // Pack-Offset for (All-)Pack-ArrayCh 8 & Ch0
  dac_pack_CH9_1 = 2,       // Pack-Offset for (All-)Pack-ArrayCh 9 & Ch1
  dac_pack_CH10_2 = 4,      // Pack-Offset for (All-)Pack-ArrayCh10 & Ch2
  dac_pack_CH11_3 = 6,      // Pack-Offset for (All-)Pack-ArrayCh11 & Ch3
  dac_pack_CH12_4 = 8,      // Pack-Offset for (All-)Pack-ArrayCh12 & Ch4
  dac_pack_CH13_5 = 10,     // Pack-Offset for (All-)Pack-ArrayCh13 & Ch5
  dac_pack_CH14_6 = 12,     // Pack-Offset for (All-)Pack-ArrayCh14 & Ch6
  dac_pack_CH15_7 = 14,     // Pack-Offset for (All-)Pack-ArrayCh15 & Ch7
  dac_pack_ctrlDAC1_0 = 16, // Pack-Offset for (All-)Pack-ArrayDAC1 & DAC0

};

/* Structure of Cmd-Byte
* Bit[23]: RW - RW = 0 sets a write operation; RW = 1 sets a read operation
* Bit[22]: Don't care
* Bit[21:16]: Register-Address N
* Bit[15:0]: Data-Bits
* Every other following 16 Bits: Additional Data-Bits for N+1, N+2, ...
*/
union DAC_StructuralDataPack
{
  uint8_t serialized[DAC_PACK_NBYTES];
  struct
  {
    uint8_t CmdByte;
    uint16_t Data;
  } __attribute__((packed));
};
typedef union DAC_StructuralDataPack DAC_StructuralDataPack_t;

struct DAC_Data
{
  uint8_t nVoltPacks;
  uint8_t nConfPacks;
  uint8_t nPacks;

  union
  {
    struct
    {
      DAC_StructuralDataPack_t VoltPack[DAC_NALLVOLTPACKS];
      DAC_StructuralDataPack_t ConfPack[DAC_NALLCONFPACKS];
    } __attribute__((packed));

    struct
    {
      DAC_StructuralDataPack_t Pack[DAC_NALLPACKS];
    } __attribute__((packed));
  } __attribute__((packed));
};
typedef struct DAC_Data DAC_Data_t;

struct ssiStream
{
  uint16_t nBytes;
  uint16_t nPacks;

  union
  {
    uint8_t serializedStream[DAC_NDACS * DAC_PACK_NBYTES];
    struct
    {
      DAC_StructuralDataPack_t pack[DAC_NDACS];
    } __attribute__((packed));
  } __attribute__((packed));
};
typedef struct ssiStream ssiStream_t;

/*******************************\
| Function declaration
\*******************************/
void dacs_init(void);

cBool dac_chipselect(cBool csState);        // Nonblocking chipselect
void dac_chipselectBlocking(cBool csState); // Blocking core until chipselect can be done

// Tx & Rx
// cBool dac_RxListen(cBool listenState);                                            // Dis- or enables Rx-Pin (non-blocking)
// void dac_RxListenBlocking(cBool listenState);                                     // Dis- or enables Rx-Pin (blocking)

DAC_Data_t *dac_grabRxDataStruct(void); // Return the Rx-Datastructure
DAC_Data_t *dac_grabTxDataStruct(void); // Return the Tx-Datastructure

uint8_t dac_sendReceivePacks(enum, dac)

    // // void dac_abortRxPackage(void);                                                    // Cancel current receive-querry (only for non-blocking!)
    // uint8_t dac_receiveRxPackage(enum dac_packIndex iPack, uint8_t nPacksExpected);   // Nonblocking Rx-receive
    // void dac_receiveRxPackageBlocking(enum dac_packIndex iPack, uint8_t nPacksAwait); // Blocking Rx-receive

    // // Tx-Functions
    // void dac_transmitTxPackage(enum dac_packIndex iPack, uint8_t nPacksSend);         // Nonblocking Tx-transmit
    // void dac_transmitTxPackageBlocking(enum dac_packIndex iPack, uint8_t nPacksSend); // Blocking Tx-Transmit until it's finished

    // Query-Functions
    void dac_queryPackageBlocking(enum dac_packIndex iPack, uint8_t nPacks); // Sends a pack and query the response (changes confPack to NOP-Write)

#endif