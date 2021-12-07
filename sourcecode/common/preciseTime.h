#ifndef pTime_H
#define pTime_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "TM4C123GH6PM.h"
#include "common.h"

/*******************************\
| Defines
\*******************************/

/*******************************\
| Enum/Struct/Union
\*******************************/
enum pTime_tickbase
{
  pTime_tickbase_1us,
  pTime_tickbase_10us,
  pTime_tickbase_100us,
  pTime_tickbase_1ms,
  pTime_tickbase_10ms,
  pTime_tickbase_100ms,
};

/*******************************\
| Function declaration
\*******************************/
void pTime_init(enum pTime_tickbase tickbase);
void pTime_changeTickbase(enum pTime_tickbase tickbase);

void pTime_start(uint32_t tickbaseCount);
void pTime_stop(cBool reset);
cBool pTime_elapsed(void);
void pTime_wait(uint32_t tickbaseCount);

#endif