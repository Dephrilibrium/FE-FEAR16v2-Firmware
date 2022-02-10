/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */
#include "terminal.h"
#include "uart1.h"
#include "cmdsDAC.h"
#include "cmdsADC.h"
#include "cString.h"

/*******************************\
| Local Defines
\*******************************/

/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Local function declarations
\*******************************/
void terminal_clearLineBuffer(void); // Clear linebuffer ('\0' to first char)
void terminal_wipeLineBuffer(void);  // Wipe linebuffer (with '\0')
void terminal_clearCmdStruct(void);  // Clear linebuffer ('\0' to first char)
void terminal_wipeCmdStruct(void);   // Wipe linebuffer (with '\0')
void terminal_clearBuffers(void);    // Clear linebuffer ('\0' to first char)
void terminal_wipeBuffers(void);     // Wipe linebuffer (with '\0')

terminalCmd_t *terminal_decodeLineBuffer(void);

void terminal_echo(terminalCmd_t *cmd);
void terminal_idn(terminalCmd_t *cmd);
void terminal_changeBaud(terminalCmd_t *cmd);
// void terminal_option_echo(terminalCmd_t *cmd); // Maybe for future use
void terminal_lineterm(terminalCmd_t *cmd);
// void terminal_setVoltageDAC(terminalCmd_t *cmd); // Moved to cmdsDAC

void terminal_err(terminalCmd_t *cmd);
void terminal_unknown(terminalCmd_t *cmd);

/*******************************\
| Global variablesH
\*******************************/
terminalOptions_t termOptions = {
    .echo = bFalse,
    .lineTerm = {
        .CR = "\r",
        .LF = "\n",
        .CRLF = "\r\n",
        .stdlineTerm = termOptions.lineTerm.CRLF,
    }};
// struct terminal_lineTerm_t lineTerm = {"\r", "\n", "\r\n", lineTerm.CRLF};

// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wmissing-braces"
inpLine_t _inputLine = {TERMINAL_INPUTLINE_BUFFSIZE, 0};
// #pragma GCC diagnostic pop
terminalCmd_t _command = {TERMINAL_CMD_AMOUNT, TERMINAL_CMD_BUFFSIZE, 0};

/*******************************\
| Function definitons
\*******************************/
#pragma region Terminal init / configs
void terminal_init(void)
{
    uart1_init(TERMINAL_BAUD);
    terminal_wipeBuffers(); // Wipe buffers
}
#pragma endregion Terminal init / configs

#pragma region Terminal input + Buffer clear / wipe
terminalCmd_t *terminal_fetchCmd(void)
{
    static char c;

    while (uart1_RxFifoStatus() != UART1_FIFO_Empty)
    {
        if (_inputLine.Filled >= _inputLine.Size) // Check Bufferoverflow
        {
            // When somethings wrong, reset buffers and return error-cmd
            terminal_clearBuffers();
            strcpy(_command.argv[CMD_ERR_HANDLE_INDEX], CMD_ERR_HANDLE);
            strcpy(_command.argv[CMD_ERRMSG_INDEX], CMD_ERRMSG_UART1_BUFFEROVERFLOW);
            _command.argc = 2;
            return &_command;
        }
        c = uart1_getc();

        // Catch \r\n from the previous input!
        if (_inputLine.Filled == 0      // When trying on start to enter
            && (c == '\r' || c == '\n') //  and CR or LF do not copy it and return
        )
            break;

        _inputLine.LineBuffer[_inputLine.Filled] = c; // Copy content
        // Check line finished
        if (_inputLine.LineBuffer[_inputLine.Filled] == '\r'     // Check CR
            || _inputLine.LineBuffer[_inputLine.Filled] == '\n') // or LF
        {
            _inputLine.LineBuffer[_inputLine.Filled] = '\0'; // Terminate string on last position

            terminal_clearCmdStruct(); // Prepare cmd-struct
            terminal_decodeLineBuffer();
            terminal_clearLineBuffer(); // Parsed inputLine copied to Cmd -> Clear
            return &_command;
        }

        // Handle backspace
        if (_inputLine.Filled == 0 && c == '\b')
            continue;
        else if (_inputLine.Filled > 0 && c == '\b')
            _inputLine.Filled--;
        else if (c != '\b')
            _inputLine.Filled++;
        //_inputLine.LineBuffer[_inputLine.Filled] = '\0'; // Be sure string is always terminated

        // Add option to echo on/off
        // uart1_putc(c);
    }

    return NULL; // Nothing new here, give NULL
}

terminalCmd_t *terminal_decodeLineBuffer(void)
{
    const char delim[] = " ";
    // strtok causes jump into default handler!
    // char *tokStart = strtok(_inputLine.LineBuffer, delim); // Initiate strtok and search for space-character ' '
    char *tokStart = strtok(_inputLine.LineBuffer, delim); // Initiate strtok and search for space-character ' '
    while (tokStart != NULL)
    {
        strcpy(_command.argv[_command.argc], tokStart);
        _command.argc++;
        if (_command.argc > _command.maxArgc)
        {
            terminal_clearCmdStruct();
            strcpy(_command.argv[CMD_ERR_HANDLE_INDEX], CMD_ERR_HANDLE);
            strcpy(_command.argv[CMD_ERRMSG_INDEX], CMD_ERRMSG_CMDSTRUCT_ARGC_EXEEDED);
            return &_command;
        }
        tokStart = strtok(NULL, delim);
    }

    return &_command; // Return adress when ok, otherwise can optionally be used to indicate an error
}

void terminal_clearBuffers(void)
{
    terminal_clearLineBuffer();
    terminal_clearCmdStruct();
}

void terminal_wipeBuffers(void)
{
    terminal_wipeLineBuffer();
    terminal_wipeCmdStruct();
}

void terminal_clearLineBuffer(void)
{
    // Internal line buffer
    _inputLine.LineBuffer[0] = '\0';
    _inputLine.Filled = 0;
}

void terminal_wipeLineBuffer(void)
{
    uint16_t i;
    // Internal line buffer
    for (i = 0; i < _inputLine.Size; i++)
        _inputLine.LineBuffer[i] = '\0';
    _inputLine.Filled = 0;
}

void terminal_clearCmdStruct(void)
{
    // Command-structure
    for (uint16_t iArg = 0; iArg < _command.maxArgc; iArg++)
        _command.argv[iArg][0] = '\0'; // Clear argv
    _command.argc = 0;                 // Clear argc
}

void terminal_wipeCmdStruct(void)
{
    uint16_t i, j;
    // Command-structure
    for (i = 0; i < _command.maxArgc; i++)
        for (j = 0; j < _command.argvSize; j++)
            _command.argv[i][j] = '\0'; // Clear argv
    _command.argc = 0;                  // Clear argc
}
#pragma region Terminal input + Buffer clear / wipe

#pragma region Terminal run commands
enum terminalError terminal_runCmd(terminalCmd_t *cmd)
{
    /***************** Check all "real" commands first *****************\
    | Checking first the typic used commands to reduce the comparison   |
    | effort.                                                           |
    \*******************************************************************/
    // DAC commands
    if (strcmp(CMDS_DAC_VSET, cmd->argv[CMD_HANDLE_INDEX]) == 0)
        cmdsDAC_setVoltage(cmd);

    else if (strcmp(CMDS_DAC_VGET, cmd->argv[CMD_HANDLE_INDEX]) == 0)
        cmdsDAC_getVoltage(cmd);

    // ADC commands
    else if (strcmp(CMDS_ADC_VGET, cmd->argv[CMD_HANDLE_INDEX]) == 0)
        cmdsADC_getVoltage(cmd);

    // Terminal/Options and so on
    else if (strcmp(CMD_IDN, cmd->argv[CMD_HANDLE_INDEX]) == 0)
        terminal_idn(cmd);

    // Device Options
    // else if (strcmp(CMD_TERMINAL_ECHO_SET, cmd->argv[CMD_HANDLE_INDEX]) == 0)
    //     terminal_echo_(cmd);

    else if (strcmp(CMD_TERMINAL_LINETERM_SET, cmd->argv[CMD_HANDLE_INDEX]) == 0)
        terminal_lineterm(cmd);

    else if (strcmp(CMD_TERMINAL_BAUD_SET, cmd->argv[CMD_HANDLE_INDEX]) == 0)
        terminal_changeBaud(cmd);
    /********************************************************************/

    /***************** Check all common cmds as last  *****************\
    | Checking the common and not so often used commands as last saves |
    | a little bit comparison effort of the µC.                        |
    \******************************************************************/
    else if (strcmp(CMD_ECHO, cmd->argv[CMD_HANDLE_INDEX]) == 0)
        terminal_echo(cmd);
    else if (strcmp(CMD_ERR_HANDLE, cmd->argv[CMD_HANDLE_INDEX]) == 0) // Error
        terminal_err(cmd);
    else
        terminal_unknown(cmd);
    /******************************************************************/

    return terminal_Ok;
}

void terminal_echo(terminalCmd_t *cmd)
{
    // uart1_Transmit(termOptions.lineTerm.stdlineTerm);
    uint16_t lastArgc = cmd->argc - 1;
    for (uint16_t argi = 1; argi <= lastArgc; argi++)
    {
        terminal_send(cmd->argv[argi]);
        if (argi < lastArgc)
            terminal_send(" ");
    }
    terminal_sendline(NULL);
}

void terminal_idn(terminalCmd_t *cmd)
{
    terminal_send(TERMINAL_ACKNOWLEDGED);
    terminal_send(" Device,");
    terminal_send(IDN_DEVICETYPE);
    terminal_send(",Firmware-Build,");
    terminal_sendline(IDN_FIRMWARE);
}

void terminal_changeBaud(terminalCmd_t *cmd)
{
    if (cmd->argc < 2)
    {
        terminal_NAK("No argument given");
        return;
    }
    if (cmd->argc > 2)
    {
        terminal_NAK("Too much arguments");
        return;
    }

    if (cstrIsNum(cmd->argv[1], SNB_Dez) == bFalse)
    {
        terminal_NAK("Argument is no number");
        return;
    }

    uint32_t baud = atoi(cmd->argv[1]);
    terminal_ACK(NULL);
    uart1_setBaud(baud);
}

// void terminal_option_echo(terminalCmd_t *cmd)
// {
// }

void terminal_lineterm(terminalCmd_t *cmd)
{
    if (cmd->argc < 2)
        terminal_NAK("Missing argument");

    if (cmd->argc > 2)
        terminal_NAK("Too much arguments");

    if (strcmp(cmd->argv[1], termOptions.lineTerm.CR))
        termOptions.lineTerm.stdlineTerm = termOptions.lineTerm.CR;
    else if (strcmp(cmd->argv[1], termOptions.lineTerm.LF))
        termOptions.lineTerm.stdlineTerm = termOptions.lineTerm.LF;
    else if (strcmp(cmd->argv[1], termOptions.lineTerm.CRLF))
        termOptions.lineTerm.stdlineTerm = termOptions.lineTerm.CRLF;
    else
    {
        termOptions.lineTerm.stdlineTerm = termOptions.lineTerm.CRLF;
        terminal_NAK(NULL);
        return;
    }

    terminal_ACK(NULL);
}

void terminal_err(terminalCmd_t *cmd)
{
    terminal_NAK("Error");
    terminal_send(",");
    terminal_send(cmd->argv[CMD_ERR_HANDLE_INDEX]);
    terminal_send(",");
    terminal_sendline(cmd->argv[CMD_ERRMSG_INDEX]);
    // uart1_Transmit(termOptions.lineTerm.stdlineTerm);
}

void terminal_unknown(terminalCmd_t *cmd)
{
    terminal_NAK("Unknown command");
    if (cmd->argv[0] != NULL)
    {
        terminal_send(",");
        terminal_send(cmd->argv[CMD_HANDLE_INDEX]);
    }
    terminal_sendline(NULL);
}
#pragma endregion Terminal run commands

void terminal_send(char *msg)
{
    if (msg != NULL)
        uart1_Transmit(msg);
}

void terminal_sendline(char *msg)
{
    if (msg != NULL)
        uart1_Transmit(msg);
    uart1_Transmit(termOptions.lineTerm.stdlineTerm);
}

void terminal_ACK(char *msg)
{
    terminal_send(TERMINAL_ACKNOWLEDGED);
    if (msg != NULL)
    {
        terminal_send(" ");
        terminal_send(msg);
    }
    terminal_sendline(NULL);
}

void terminal_NAK(char *msg)
{
    terminal_send(TERMINAL_NOT_ACKNOWLEDGED);
    if (msg != NULL)
    {
        terminal_send(" ");
        terminal_sendline(msg);
    }
    terminal_sendline(NULL);
}