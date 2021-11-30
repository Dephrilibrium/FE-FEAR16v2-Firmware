/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "swap.h"
#include "common.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Hardwaredefines

#pragma endregion Hardwaredefines

/*******************************\
| Local function declarations
\*******************************/

/*******************************\
| Global variables
\*******************************/

/*******************************\
| Function definitons
\*******************************/
uint8_t rbit_byte(uint8_t byte)
{
  uint32_t rbitVar = byte;
  __asm("rbit %[in], %[out]"
        : [out] "=r"(rbitVar)
        : [in] "r"(rbitVar));
  return SHIFTDOWN(rbitVar, 24); // rbit always rotates with 32-bit -> Shift highbyte to lowbyte
}

uint16_t rbit_word(uint16_t word)
{
  uint32_t rbitVar = word;
  __asm("rbit %[in], %[out]"
        : [out] "=r"(rbitVar)
        : [in] "r"(rbitVar));

  return SHIFTDOWN(rbitVar, 16); // rbit always rotates with 32-bit -> Shift highword to lowword
}

uint32_t rbit_dword(uint32_t dword)
{
  __asm("rbit %[in], %[out]"
        : [out] "=r"(dword)
        : [in] "r"(dword));
  return dword;
}

uint8_t swap_byte(uint8_t byte)
{
  // Shift automatically throw out the "wrong" nibbles
  return (SHIFTUP(byte, 4) | SHIFTDOWN(byte, 4));
}

uint16_t swap_word(uint16_t word)
{
  // Shift automatically throw out the "wrong" bytes
  return (SHIFTUP(word, 8) | SHIFTDOWN(word, 8));
}

uint32_t swap_dword(uint32_t dword)
{
  // Shift automatically throw out the "wrong" words
  return (SHIFTUP(dword, 16) | SHIFTDOWN(dword, 16));
}
