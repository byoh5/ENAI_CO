// ENAI_CO.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "shell.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	UINT32 ret = 0;
	char instr[MAX_IN_STRING] = { 0, 0 };

	while (1) {
		std::cout << PROMPT;
		std::cin.getline(instr, MAX_IN_STRING);
		ret = _DoCommand((char*)instr);
		if (ret == CMD_LINE_ERROR)
		{
			printf("Bad or not command!\n");
		}
		Sleep(1);
	}
	return 0;
}

