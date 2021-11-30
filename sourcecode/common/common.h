#ifndef COMMON_H
#define COMMON_H

/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdint.h"

/* Project specific */

/*******************************\
| Defines
\*******************************/
#define PIOSC_MHZ (uint32_t)16e6 // PIOSC is a internal high precision 16MHz (+-3%) clock source

// Common Shift-Operations. Additional paranthesis for caluclation-security, e. .g argument is an formula like "variable - 1" or something
#define SHIFTUP(x, y) (uint32_t)((x) << (y))   // Shift x up by y bits
#define SHIFTDOWN(x, y) (uint32_t)((x) >> (y)) // Shift x down by y bits
#define BIT(x) SHIFTUP(0x01, x)                // Shifts a bit to position by "x"
#define OPTION(x, y) SHIFTUP(x, y)             // Shifts an option "x" to position by "y"

/*******************************\
| Enum/Struct/Union
\*******************************/
enum cBoolean
{
  bFalse = 0,
  bOff = 0,
  bTrue = 1,
  bOn = 1,
};
typedef enum cBoolean cBool;

/*******************************\
| Function declaration
\*******************************/
void delay(uint32_t delay);

#endif
