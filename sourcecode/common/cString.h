#ifndef CSTRING_H
#define CSTRING_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdint.h"

/* Project specific */
#include "common.h"

/*******************************\
| Defines
\*******************************/

/*******************************\
| Enum/Struct/Union
\*******************************/
enum StringNumberBase
{
  SNB_Bin = 2,
  SNB_Oct = 8,
  SNB_Dez = 10,
  SNB_Hex = 16,
};

/*******************************\
| Function declaration
\*******************************/
uint32_t cstrCntChar(const char *string, char cSearch);
// cBool cstrIsNum(const char *string, enum StringNumberBase snb); // Needs to be implemented

#endif