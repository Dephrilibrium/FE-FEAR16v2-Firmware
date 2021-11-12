#ifndef HELPER_H
#define HELPER_H

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

#define BIT(x) (uint32_t)(1<<x) // Shifts a bit to position by "x"
#define OPTION(x, y) (uint32_t)(x<<y) // Shifts an option "x" to position by "y"

/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/


#endif
