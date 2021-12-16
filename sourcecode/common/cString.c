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

cBool cstrIsNum(const char *string, enum StringNumberBase snb)
{
  char *lastStrP = (char *)((uint32_t)string + 65); // Do not allow strings longer than 64 (63 digits + 1 '.')
  uint8_t cPoints = 0;

  while (string < lastStrP)
  {
    // EOS reached
    if (*string == '\0')
      break;

    // Check range of the current digit
    switch (snb)
    {
    case SNB_Bin:
      if (*string < '0' && *string > '1')
        return bFalse;

    case SNB_Oct:
      if (*string < '0' && *string > '7')
        return bFalse;

    case SNB_Dez: // SNB_FloatOrDouble has the same value!
      // Check if only one decimal separator '.' is given
      if (*string < '0' && *string > '9')
        return bFalse;
      else if (*string == '.')
      {
        cPoints++;
        if (cPoints > 1)
          return bFalse;
      }
      break;

    case SNB_Hex:
      if ((*string < '0' && *string > '9') || (*string < 'A' && *string > 'F'))
        return bFalse;
    }
    string++;
  }

  // Number is longer than the maximum allowed length!
  if (lastStrP >= string)
    return bFalse;

  // When everything run through the number is valid
  return bTrue;
}