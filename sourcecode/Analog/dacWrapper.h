#ifndef DACWRAPPER_H
#define DACWRAPPER_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "common.h"
// #include "ssi0_DACs.h"

/*******************************\
| Defines
\*******************************/
// DAC
#define DAC_SSI_CLKRATE ssi_clkRate_1MHz // Default-Clockrate

// DAC Datastructure
#define DAC_NDACS 2       // Amount of DACs in daisy-chain
#define DAC_CONFPACKS 1   // Amoutn of config-packs
#define DAC_VOLTPACKS 8   // Amount of outputvoltage-packs
#define DAC_PACK_NBYTES 3 // Bytes per data-pack

#define DAC_BYTES_PER_SEND (DAC_NDACS * DAC_PACK_NBYTES)

#define DAC_NALLCONFPACKS (DAC_NDACS * DAC_CONFPACKS)
#define DAC_NALLVOLTPACKS (DAC_NDACS * DAC_VOLTPACKS)
#define DAC_NALLPACKS (DAC_NALLCONFPACKS + DAC_NALLVOLTPACKS) // Amount of all packs
#define DAC_ALLPACKS_NBYTES (DAC_NALLPACKS * DAC_PACK_NBYTES) // Amount of bytes for all data-packs. Be carefull when updating code for more than 2 daisychain-DACs. Struct not using this calculation to modify the entire struct!

/*******************************\
| Enum/Struct/Union
\*******************************/
enum dac_packIndex
{
  dac_vPack_Ch0,
  dac_vPack_Ch1,
  dac_vPack_Ch2,
  dac_vPack_Ch3,
  dac_vPack_Ch4,
  dac_vPack_Ch5,
  dac_vPack_Ch6,
  dac_vPack_Ch7,
  dac_vPack_Ch8,
  dac_vPack_Ch9,
  dac_vPack_Ch10,
  dac_vPack_Ch11,
  dac_vPack_Ch12,
  dac_vPack_Ch13,
  dac_vPack_Ch14,
  dac_vPack_Ch15,
  dac_vPacks_Count,
  dac_cPack_DAC0 = dac_vPacks_Count,
  dac_cPack_DAC1,
  dac_allPacks_Count,
};

/* Structure of Cmd-Byte
 * Bit[23]: RW - RW = 0 sets a write operation; RW = 1 sets a read operation
 * Bit[22]: Don't care
 * Bit[21:16]: Register-Address N
 * Bit[15:0]: Data-Bits
 * Every other following 16 Bits: Additional Data-Bits for N+1, N+2, ...
 */
// union DAC_StructuralDataPack
// {
// uint8_t Serialized[DAC_PACK_NBYTES];
typedef struct
{
  uint16_t Data;
  uint8_t CmdByte;
} __attribute__((packed)) DAC_StructuralDataPack_t;
// typedef union DAC_StructuralDataPack DAC_StructuralDataPack_t;

typedef struct
{
  const uint8_t nVPacks;
  const uint8_t nCPacks;
  const uint8_t nPacks;

  union
  {
    struct
    {
      DAC_StructuralDataPack_t VPacks[DAC_NALLVOLTPACKS];
      DAC_StructuralDataPack_t CPacks[DAC_NALLCONFPACKS];
    };

    struct
    {
      DAC_StructuralDataPack_t Packs[DAC_NALLPACKS];
    };
  };
} DAC_Data_t;

typedef struct
{
  uint16_t nBytes;
  const uint16_t nSize;
  const uint16_t nPacks;
  enum dac_packIndex targetPack;

  union
  {
    uint8_t SerializedStream[DAC_BYTES_PER_SEND];
    struct
    {
      DAC_StructuralDataPack_t Packs[DAC_NDACS];
    } __attribute__((packed));
  } __attribute__((packed)); // To be able to iterate through serializedStream it has to be packed
} dac_ssiStream_t;

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

void dac_queryPack(enum dac_packIndex packIndex);

// // void dac_abortRxPackage(void);                                                    // Cancel current receive-querry (only for non-blocking!)
// uint8_t dac_receiveRxPackage(enum dac_packIndex iPack, uint8_t nPacksExpected);   // Nonblocking Rx-receive
// void dac_receiveRxPackageBlocking(enum dac_packIndex iPack, uint8_t nPacksAwait); // Blocking Rx-receive

// // Tx-Functions
// void dac_transmitTxPackage(enum dac_packIndex iPack, uint8_t nPacksSend);         // Nonblocking Tx-transmit
// void dac_transmitTxPackageBlocking(enum dac_packIndex iPack, uint8_t nPacksSend); // Blocking Tx-Transmit until it's finished

// Query-Functions
// void dac_queryPackageBlocking(enum dac_packIndex iPack, uint8_t nPacks); // Sends a pack and query the response (changes confPack to NOP-Write)

void dac_setChVoltage(uint16_t channel, float voltage);
void dac_updateChVoltage(uint16_t channel, float voltage);
void dac_sendChVoltage(uint16_t channel);

void testvalues(void);
#endif