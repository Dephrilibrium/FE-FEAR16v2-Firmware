
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include <TM4C123GH6PM.h>

/*******************************\
| Local Defines
\*******************************/
#pragma region Hardwaredefines

#pragma endregion Hardwaredefines

/*******************************\
| Local function declarations
\*******************************/

/*******************************\
| Global variables
\*******************************/
static unsigned long clk_freq = 16000000UL;

/*******************************\
| Function definitons
\*******************************/
unsigned long sys_clk_freq()
{
	return clk_freq;
}

void sys_clk_set()
{
	SYSCTL->RCC |= (1 << 11);	 /* Enable BYPASS */
	SYSCTL->RCC2 |= (1 << 11); /* Enable BYPASS2 */

	SYSCTL->RCC &= ~(1 << 22); /* Clr USESYSDIV */

	for (int i = 0; i < 10000; i++)
	{
		__asm("nop");
	}

	/* Enable OSC */
	SYSCTL->RCC &= ~(1 << 0); //MOSCDIS

	while ((SYSCTL->RIS & (1 << 8)) == 0)
	{
		__asm("nop");
	}

	SYSCTL->RCC &= ~(0x1f << 6); //CLR XTAL
	SYSCTL->RCC |= (0x15 << 6);	 //XTAL
	SYSCTL->RCC &= ~(3 << 4);		 //OSCSRC
	SYSCTL->RCC2 &= ~(7 << 4);	 //OSCSRC2

	SYSCTL->RCC &= ~(1 << 13);	//PWRDN
	SYSCTL->RCC2 &= ~(1 << 13); /* Clr PWRDN2 */

	/* Select SYSDIV2 */

	/* Use RCC2 */
	SYSCTL->RCC2 |= (1 << 31);

	SYSCTL->RCC2 |= (1 << 30);			 /* DIV400 */
	SYSCTL->RCC2 &= ~(63 << 23);		 /* Clr SYSDIV2 */
	SYSCTL->RCC2 &= ~(1 << 22);			 /* Clr SYSDIV2LSB */
	SYSCTL->RCC2 |= (2 << 23);			 /* Set SYSDIV2 */
	/* SYSCTL->RCC2 |= (1 << 22); */ /* Set SYSDIV2LSB */

	SYSCTL->RCC |= (1 << 22); /* Re-enable USESYSDIV */

	/* Wait for PLL lock by polling PLLLRIS in RIS */
	while ((SYSCTL->RIS & (1 << 6)) == 0)
	{
		__asm("nop");
	}

	/* Clear BYPASS */
	SYSCTL->RCC2 &= ~(1 << 11);
	SYSCTL->RCC &= ~(1 << 11);

	clk_freq = 80000000UL;
}