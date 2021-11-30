#ifndef INCLUDE_PROTECTION_H
#define INCLUDE_PROTECTION_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdint.h"

/* Project specific */

/*******************************\
| Defines
\*******************************/

/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/
uint8_t rbit_byte(uint8_t byte);
uint16_t rbit_word(uint16_t word);
uint32_t rbit_dword(uint32_t dword);

uint8_t swap_byte(uint8_t byte);
uint16_t swap_word(uint16_t word);
uint32_t swap_dword(uint32_t dword);

#endif