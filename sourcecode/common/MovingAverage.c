/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* WachsCode */
#include "MovingAverage.h"

/*******************************\
| Local function declarations
\*******************************/

/*******************************\
| Global variables
\*******************************/

/*******************************\
| Function defintions
\*******************************/

double Average(MovingAverage_t *averStruct, double latestVal)
{
    // Buffer management
    double throwOutVal = averStruct->Buffer[averStruct->iPos];
    averStruct->Buffer[averStruct->iPos] = latestVal;
    averStruct->iPos = (averStruct->iPos + 1) % averStruct->nSize;

    // Mean value
    double sum = averStruct->BuffMean * averStruct->nFill; // Reconstruct sum for nFilled bufferpositions
    sum += latestVal;                                     // Add latest
    sum -= throwOutVal;                                   // Remove oldest
    if (averStruct->nFill < averStruct->nSize)            // Is buffer completely filled?
        averStruct->nFill++;                              //  No: Increment nFill
    averStruct->BuffMean = sum / averStruct->nFill;       // Build new mean

    return averStruct->BuffMean;
}