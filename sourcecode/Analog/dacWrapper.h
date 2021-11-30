#ifndef DACWRAPPER_H
#define DACWRAPPER_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "common.h"

/*******************************\
| Defines
\*******************************/
// Data and packages
//#define DACS_IN_DAISY_CHAIN 2 // Amount of daisy-chained DACs
#define DACS_BYTES_PER_DAC 3 // 1 Control-Byte, 2 Data-Bytes
#ifdef DACS_IN_DAISY_CHAIN
#define DAC_BYTESIZE (DACS_IN_DAISY_CHAIN * (DAC_CTRL_BYTE + DAC_DATA_WORDS * 2)) // Each Channel has 2 DataBytes
#else
#define DAC_BYTESIZE DACS_BYTES_PER_DAC // Each Channel has 2 DataBytes
#endif

// DAC
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

#pragma endregion DAC Register Bits

/*******************************\
| Enum/Struct/Union
\*******************************/
/* Structure of Cmd-Byte
* Bit[23]: RW - RW = 0 sets a write operation; RW = 1 sets a read operation
* Bit[22]: Don't care
* Bit[21:16]: Register-Address N
* Bit[15:0]: Data-Bits
* Every other following 16 Bits: Additional Data-Bits for N+1, N+2, ...
*/

struct DAC_Out
{
  uint16_t Size;
  uint16_t Filled;
  union
  {
    uint8_t SerializedData[DAC_BYTESIZE];
    struct __attribute__((packed)) // Pack to avoid padding between uint8_t and uint16_t
    {
#ifdef DACS_IN_DAISY_CHAIN
      // Add the second datastructure
      uint8_t CmdByte1;
      uint16_t ChannelData1;
#endif
      uint8_t CmdByte0;
      uint16_t ChannelData0;
    };
  };
};
typedef struct DAC_Out DAC_DataOut_t;

// struct DAC_In
// {
//   uint16_t Size;
//   uint16_t Filled;
//   union
//   {
//     uint8_t *SerializedData;
//     struct
//     {
//       uint8_t StreamWriteCmd;
//       uint16_t Data[];
//     };
//   };
// };

/*******************************\
| Function declaration
\*******************************/
void dacs_init(void);
void dac_prepareTxPackage(uint8_t ctrlByte, uint16_t data);

#endif