/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stddef.h"

/* Project specific */
#include "cString.h"

/*******************************\
| Local Defines
\*******************************/
#pragma region Hardwaredefines

#pragma endregion Hardwaredefines

/*******************************\
| Local Enum/Struct/Union
\*******************************/

/*******************************\
| Local function declarations
\*******************************/

/*******************************\
| Global variables
\*******************************/

/*******************************\
| Function definitons
\*******************************/
uint32_t cstrCntChar(const char *string, char cSearch)
{
  if (string == NULL)
    return 0;

  uint32_t cCnt = 0;
  while (*string != '\0')
  {
    if (*string == cSearch)
      cCnt++;

    string++;
  }

  return cCnt;
}

// cBool cstrIsNum(const char *string, enum StringNumberBase snb)
// {
//   cBool isNum = bTrue;
//   char *lastStrP = string + 4096; // Do not allow strings longer than 4094
//   char upper = '0' +

//   while (string <= lastStrP)
//   {
//     if (*string == '\0')
//       break;
//       if(*string < '0' || *string > )
//   }
// }