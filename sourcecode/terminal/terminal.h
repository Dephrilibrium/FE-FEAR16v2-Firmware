#ifndef TERMINAL_H
#define TERMINAL_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

/* Project specific */
#include "common.h"

/*******************************\
| Defines
\*******************************/
#define TERMINAL_BAUD 115200
#define TERMINAL_CMD_AMOUNT 16
#define TERMINAL_CMD_BUFFSIZE 32
#define TERMINAL_INPUTLINE_BUFFSIZE ((TERMINAL_CMD_AMOUNT + 1) * TERMINAL_CMD_AMOUNT) // +1 added for a little bit extra space like spaces

#pragma region Commands
#define CMD_HANDLE_INDEX 0
//#define CMD_ARG_START_INDEX 1
#define CMD_ECHO "echo"
#define CMD_IDN "IDN?"
#define IDN_DEVICETYPE "FEAR16v2"
#define IDN_FIRMWARE "1.0.0.0"
// #define CMD_TERMINAL_ECHO_SET "TERM:ECHO" // Was planned for Echo ON/OFF, but not used
#define CMD_TERMINAL_LINETERM_SET "TERM:LINETERM"
#define CMD_TERMINAL_BAUD_SET "TERM:BAUD"

// Further commands are outsourced to:
// - terminalCmdsDAC
//
#pragma endregion Commands

#pragma region Common Messages
#define TERMINAL_ACKNOWLEDGED "ack"
#define TERMINAL_NOT_ACKNOWLEDGED "nak"
#pragma endregion Common Messages

#define CMD_ERR_HANDLE_INDEX CMD_HANDLE_INDEX
#define CMD_ERRMSG_INDEX 1
#pragma region Error + Error Messages
#define CMD_ERR_HANDLE "err"

#define CMD_ERRMSG_UART1_BUFFEROVERFLOW "UART1: Rx bufferoverflow"
#define CMD_ERRMSG_CMDSTRUCT_ARGC_EXEEDED "Cmd-Struct: argc exceeded maxArgc"
#pragma endregion Error + Error Messages

/*******************************\
| Enum/Struct/Union
\*******************************/
typedef struct // terminalInputLine
{
  const uint16_t Size;
  uint16_t Filled;
  char LineBuffer[TERMINAL_INPUTLINE_BUFFSIZE];
} inpLine_t;
// typedef struct terminalInputLine inpLine_t;

typedef struct // termCmd
{
  uint16_t maxArgc;
  uint16_t argvSize;
  uint16_t argc;
  char argv[TERMINAL_CMD_AMOUNT][TERMINAL_CMD_BUFFSIZE];
} terminalCmd_t;
// typedef struct termCmd terminalCmd_t;

// typedef struct
// {
//   char CR[2];
//   char LF[2];
//   char CRLF[3];
//   char *stdlineTerm;
// } terminal_lineTerm_t;
// extern terminal_lineTerm_t lineTerm;

typedef struct
{
  cBool echo; // Implemented for future used (maybe)
  struct
  {
    char CR[2];
    char LF[2];
    char CRLF[3];
    char *stdlineTerm;
  } lineTerm;
} terminalOptions_t;
extern terminalOptions_t termOptions;

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
void terminal_send(char *msg);
void terminal_sendline(char *msg); // Attaches the standard linetermination
void terminal_ACK(char *msg);
void terminal_NAK(char *msg);

#endif