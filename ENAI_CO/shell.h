#ifndef SHELL_H
#define SHELL_H
// Command structure

#include <windows.h>


typedef struct {
	char* name;
	UINT32(*func)(int, char**);
	const char** helphelp;
} tMonCmd;

//-------------------------------------------------------------------------------------------------
// Constants
#define	CMD_SUCCESS			 0
#define	CMD_FAILURE			-1
#define	CMD_PARAM_ERROR     -2
#define	CMD_LINE_ERROR      -3
#define	CMD_ULVL_DENIED     -4
#define	CMD_NOT_FOUND       -5
#define	CMD_PREFILL         -6


#define	CMDLINESIZE			256
#define	ARGCNT				100

#define	CTLC				0x3

#define MAX_IN_STRING		256
//-------------------------------------------------------------------------------------------------
//
#define PROMPT				"ENAI>"
#define PROMPT_SIZE			30

extern UINT32 _DoCommand(char* cmdline);

#endif