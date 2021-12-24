#ifndef SSI3_ADCs_H
#define SSI3_ADCs_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */

/*******************************\
| Defines
\*******************************/
#define SSI0_BUFFERSIZE (DAC_NDACS * DAC_PACK_NBYTES) // Max-Transfer can be "DACs * Bytes per DAC" in Bytes

/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/
void ssi3_init_ADCs(void);

#endif