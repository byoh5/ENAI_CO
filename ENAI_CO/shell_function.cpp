#include "stdafx.h"
#include "shell.h"
#include "shell_function.h"
#include "squeezeNet.h"
#include "analyzer.h"
#include "float2int_conv.h"

#include <fstream>
#include <iostream>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)

 const char* sHelpDisp[] = { "Shell command list (! : Repeat command)", (char*)0 };
 const char* sSqueeze[] = { "Squeeze", (char*)0 };
 const char* sWeightAnalyer[] = { "WeightAnalyer", (char*)0 };
 const char* sfloat2int_conv[] = { "float 2 int converter", (char*)0 };
 const char* sScript[] = { "Script", (char*)0 };
 tMonCmd gCmdList[] =
 {
	 { (char*) "?", HelpDisp, sHelpDisp },
	 { (char*) "sqz", Squeeze, sSqueeze },
	 { (char*) "wa", WeightAnalyzer, sWeightAnalyer },
	 { (char*) "f2i", Float2int_converter, sfloat2int_conv },
	 { (char*) "run", RunScript, sScript },
	 { 0, 0, 0 }
 };

UINT32 HelpDisp(int argc, char** argv)
{
	tMonCmd* cmdptr;
	UINT8 i = 0;

	cmdptr = gCmdList;

	while (1) {
		printf("[%02d]:[%-8s] - [%-50s]\n", i, cmdptr->name, *cmdptr->helphelp);
		cmdptr++;
		i++;
		if (cmdptr->name == 0)	return 0;
	}
}

UINT32 RunScript(int argc, char** argv){
//	streampos size;
	UINT32 ret;

	char cmd[1024];
	char fileName[512];
	char *token = NULL;
	char *p = NULL;

	UINT32 test_round = 1; //default is 1 time
	UINT32 err_count = 0;

	if (argc == 1){
		printf("Use (default:script.ctg) file : \n");
		strcpy(fileName, "script.cfg");
	}
	else if (argc == 2){
		printf("Use (%s) file \n", argv[1]);
		strcpy(fileName, argv[1]);
	}
	else if (argc == 3){
		printf("Test mode \n", argv[1]);
		if (strcmp("test", argv[1]) == 0){
			strcpy(fileName, "script.cfg");
			test_round = atoi(argv[2]);
		}
		else{
			printf("this is for test mode ex) run test 100\n");
			return 0;
		}
	}
	else{
		printf("error : ex) run (default==script.cfg)\n");
		return 0;
	}

	UINT32 i = 0;
	ifstream linkfile(fileName);
	if (linkfile.is_open())
	{
		printf("test_round %d\n", test_round);
		for (i = 0; i<test_round; i++){
			printf("Round %d, Errcnt %d \n", i, err_count);
			linkfile.seekg(0, ifstream::beg);
			while (!linkfile.eof()){
				linkfile.getline(cmd, 1024);
				//			cout << cmd << endl;

				if ((cmd[0] == '#') || ((cmd[0] == '/') && (cmd[1] == '/')) || ((cmd[0] == 'r') && (cmd[1] == 'u') && (cmd[2] == 'n'))){
					//	cout <<"skip cmd"<< endl;
				}
				else{

					strtok_s(cmd, "#", &token);
					p = strstr(cmd, "//");
					if (p != NULL){
						*p = 0;
						p = NULL;
					}
					p = strstr(cmd, "run");
					if (p != NULL){
						*p = 0;
						p = NULL;
					}
					cout << "cmd>" << cmd << endl;
					//				cout << strlen(cmd) << endl;
					if (strlen(cmd) != 0){
						ret = _DoCommand((char*)cmd);
						if (ret != 0){
							err_count++;
							printf("Err:%08x\n", ret);
						}
					}
				}
			}
		}
		linkfile.close();
	}
	else
	{
		cout << "Unable to open link.cfg file";
		return 0;
	}
	return 0;
}


UINT32 Squeeze(int argc, char** argv)
{
	squeezeNet();

	return 0;
}

UINT32 WeightAnalyzer(int argc, char** argv)
{
	printf("argc %d\r\n", argc);
	int i = 0;
	for (i = 0; i<argc; i++){
		printf("%d : %s \r\n", i, argv[i]);
	}
	if (argc == 2){
		weightAnalyzerOfFile(argv[1]);
	}

	return 0;
}

UINT32 Float2int_converter(int argc, char** argv)
{
	float max = 0;
	max = absolute_max_value(argv[1]);
	Convert_INT(argv[1], argv[2], max, 8, STRING);
	return 0;
}