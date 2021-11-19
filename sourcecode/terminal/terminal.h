#ifndef TERMINAL_H
#define TERMINAL_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "string.h"
#include "stdint.h"
#include "stdio.h"

/* Project specific */

/*******************************\
| Defines
\*******************************/
#define TERMINAL_CMD_AMOUNT 16
#define TERMINAL_CMD_BUFFSIZE 32
#define TERMINAL_INPUTLINE_BUFFSIZE ((TERMINAL_CMD_AMOUNT + 1) * TERMINAL_CMD_AMOUNT) // +1 added for a little bit extra space like spaces

#pragma region Commands
#define CMD_HANDLE_INDEX 0
//#define CMD_ARG_START_INDEX 1
#define CMD_ECHO "echo"
#pragma endregion Commands

#define CMD_ERR_HANDLE_INDEX CMD_HANDLE_INDEX
#define CMD_ERRMSG_INDEX 1
#pragma region Error + Messages
#define CMD_ERR_HANDLE "err;"

#define CMD_ERRMSG_UART1_BUFFEROVERFLOW "UART1: Rx bufferoverflow"
#define CMD_ERRMSG_CMDSTRUCT_ARGC_EXEEDED "Cmd-Struct: argc exceeded maxArgc"
#pragma endregion Error + Messages

/*******************************\
| Enum/Struct/Union
\*******************************/
struct terminalInputLine
{
  const uint16_t Size;
  uint16_t Filled;
  char LineBuffer[TERMINAL_INPUTLINE_BUFFSIZE];
};
typedef struct terminalInputLine inpLine_t;

struct termCmd
{
  uint16_t maxArgc;
  uint16_t argvSize;
  uint16_t argc;
  char argv[TERMINAL_CMD_AMOUNT][TERMINAL_CMD_BUFFSIZE];
};
typedef struct termCmd terminalCmd_t;

struct terminal_lineTerm
{
  char CR[2];
  char LF[2];
  char CRLF[3];
  char *stdlineTerm;
};
extern struct terminal_lineTerm lineTerm;

enum terminalError
{
  terminal_Ok = 0x00,
};

/*******************************\
| Function declaration
\*******************************/
void terminal_init(void);

terminalCmd_t *terminal_fetchCmd(void);
enum terminalError terminal_runCmd(terminalCmd_t *cmd);

#endif