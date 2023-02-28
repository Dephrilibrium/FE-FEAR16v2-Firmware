#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdint.h"

/* Wordclock */

/*******************************\
| Defines
\*******************************/

/*******************************\
| Enum/Struct/Union
\*******************************/
typedef struct
{
    uint16_t nSize;
    uint16_t nFill;
    uint16_t iPos;
    double BuffMean;
    double *Buffer;
} MovingAverage_t;

/*******************************\
| Function declarations
\*******************************/
double Average(MovingAverage_t *averStruct, double latestVal);

#endif
