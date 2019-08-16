#include "stdafx.h"
#include <iostream>

#include "shell.h"
#include "shell_function.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)

//-------------------------------------------------------------------------------------------------
// Take the incoming string and create an argv[] array from that.
// The incoming string is assumed to be writeable.
// The argv[] array is simple a set of pointers into that string, where the whitespace delimited character sets are each NULL terminated.

UINT32 _tokenize(char* string, char* argv[])
{
	UINT32	argc, done;

	for (argc = 0; argc < ARGCNT; argc++)	argv[argc] = (char*)0;

	argc = 0;

	while (1) {
		while ((*string == ' ') || (*string == '\t'))		string++;

		if (*string == 0)			break;

		argv[argc] = string;

		while ((*string != ' ') && (*string != '\t')) {
			if ((*string == '\\') && (*(string + 1) == '"')) {
				strcpy(string, string + 1);
			}
			else if (*string == '"') {
				strcpy(string, string + 1);
				while (*string != '"') {
					if ((*string == '\\') && (*(string + 1) == '"'))		strcpy(string, string + 1);
					if (*string == 0)		return(-1);
					string++;
				}

				strcpy(string, string + 1);
				continue;
			}

			if (*string == 0)		break;
			string++;
		}

		if (*string == 0) { done = 1; }
		else { done = 0;	*string++ = 0; }

		argc++;
		if (done)			break;

		if (argc >= ARGCNT) {
			argc = -1;
			break;
		}
	}
	return(argc);
}


UINT32 _DoCommand(char* cmdline)
{
	UINT32		ret, argc;
	tMonCmd* cmdptr;
	char* argv[ARGCNT], cmdcpy[CMDLINESIZE];


	if (cmdline[0] == '_')		cmdline++;
	//	if (cmdline[0] == '!')		strcpy(cmdline, gcHistoryBuf);

	strcpy(cmdcpy, cmdline);

	//	if(cmdline[0]!='!')		strcpy(gcHistoryBuf, cmdline);


	argc = _tokenize(cmdcpy, argv);

	if ((argc == 0) && (argc < 0))		return(CMD_FAILURE);


	cmdptr = gCmdList;

	if (argv[0] == NULL)				return(CMD_SUCCESS);

	while (cmdptr->name) {
		if (strcmp(argv[0], cmdptr->name) == 0)		break;
		cmdptr++;

		if (cmdptr->name == 0)		return (CMD_LINE_ERROR);
	}

	//	if(strcmp("md", cmdptr->name) != 0)		jprintf("%s\n", *cmdptr->helphelp);		// for fast memory display

	ret = cmdptr->func(argc, argv);

	printf("\n");


	return(ret);
}