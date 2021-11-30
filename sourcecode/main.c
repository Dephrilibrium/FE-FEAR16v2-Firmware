/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "TM4C123GH6PM.h"
#include "sysclk.h"
#include "uart1.h"
#include "terminal.h"
#include "ssi0_DACs.h"
#include "dacWrapper.h"

#include "swap.h"

/*******************************\
| Local Defines
\*******************************/

/*******************************\
| Local function declarations
\*******************************/

/*******************************\
| Global variables
\*******************************/
char blubb = 'i';
uint8_t abc[] = "lksjdafsjd\0";

/*******************************\
| Function definitons
\*******************************/
int main(void)
{
    terminalCmd_t *cmd;

    // Inits
    sys_clk_set(); // Set 80MHz
    uart1_init(921600);
    terminal_init();
    ssi0_init(ssi0_clkRate_1MHz);
    // dacs_init();

    // Other stuff
    // uart1_Transmit("Hallo welt! Ich schreibe einen längeren Text");
    // uart1_Transmit(lineTerm.stdlineTerm);

    /* Setup dummy */
    static DAC_DataOut_t *DACout;
    DACout = DAC_getDataOut();

    // DAC Enable SDO
    DACout->CmdByte0 = DAC_WRITE | DAC_REG_ADDR_SPICONFIG;
    DACout->ChannelData0 = swap_word(DAC_SPICONF_SDOEN);
    DACout->Filled = 3;
    ssi0_transmit();
    delay(300000);
    // DAC Read back Config
    DACout->CmdByte0 = DAC_READ | DAC_REG_ADDR_SPICONFIG;
    DACout->ChannelData0 = DAC_NOP_DATA;
    DACout->Filled = 3;
    ssi0_transmit();
    delay(30);
    // DAC Dummy
    DACout->CmdByte0 = DAC_WRITE | DAC_REG_ADDR_NOP;
    DACout->ChannelData0 = DAC_NOP_DATA;
    DACout->Filled = 3;
    ssi0_transmit();
    delay(30);

    // DAC Read DevID
    DACout->CmdByte0 = DAC_READ | DAC_REG_ADDR_DEVICEID;
    DACout->ChannelData0 = DAC_NOP_DATA;
    DACout->Filled = 3;
    ssi0_transmit();
    delay(30);
    // DAC Dummy
    DACout->CmdByte0 = DAC_WRITE | DAC_REG_ADDR_NOP;
    DACout->ChannelData0 = DAC_NOP_DATA;
    DACout->Filled = 3;
    ssi0_transmit();
    delay(30);

    // DAC Range
    DACout->CmdByte0 = DAC_WRITE | DAC_REG_ADDR_DACRANGE0;
    DACout->ChannelData0 = swap_word(OPTION(0xC, 0) | OPTION(0xC, 4) | OPTION(0xC, 8) | OPTION(0xC, 12));
    DACout->Filled = 3;
    ssi0_transmit();
    delay(30);
    DACout->CmdByte0 = DAC_WRITE | DAC_REG_ADDR_DACRANGE1;
    DACout->ChannelData0 = swap_word(OPTION(0xC, 0) | OPTION(0xC, 4) | OPTION(0xC, 8) | OPTION(0xC, 12));
    DACout->Filled = 3;
    ssi0_transmit();
    delay(30);

    // DAC Output1
    DACout->CmdByte0 = DAC_WRITE | DAC_REG_ADDR_DAC0;
    DACout->ChannelData0 = swap_word(0x1234);
    DACout->Filled = 3;
    ssi0_transmit();
    delay(30);
    // DACout->CmdByte0 = DAC_WRITE | DAC_REG_ADDR_DAC0;
    DACout->ChannelData0 = swap_word(0x4321);
    DACout->Filled = 3;
    ssi0_transmit();
    delay(30);
    // DACout->CmdByte0 = DAC_WRITE | DAC_REG_ADDR_DAC0;
    DACout->ChannelData0 = swap_word(0x0000);
    DACout->Filled = 3;
    ssi0_transmit();
    delay(30);
    // DACout->CmdByte0 = DAC_WRITE | DAC_REG_ADDR_DAC0;
    DACout->ChannelData0 = swap_word(0xFFFF);
    DACout->Filled = 3;
    ssi0_transmit();
    delay(30);

    while (1)
    {
        cmd = terminal_fetchCmd();
        if (cmd != NULL)
            terminal_runCmd(cmd);
    }
}

void delay(uint32_t delay)
{
    while (delay--)
        ;
}