#include "stdafx.h"
#include "shell.h"
#include "shell_function.h"
#include "squeezeNet.h"
#include "analyzer.h"
#include "float2int_conv.h"
#include "image_cv.h"
#include "layer.h"

#include <fstream>
#include <iostream>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)

 const char* sHelpDisp[] = { "Shell command list (! : Repeat command)", (char*)0 };
 const char* sSqueeze[] = { "Squeeze", (char*)0 };
 const char* sWeightAnalyer[] = { "WeightAnalyer", (char*)0 };
 const char* sfloat2int_conv[] = { "Quantization", (char*)0 };
 const char* sScript[] = { "Script", (char*)0 };
 const char* sImgView[] = { "img ", (char*)0 };
 const char* sResize[] = { "resize ", (char*)0 };
 const char* sConvolution[] = { "convolution ", (char*)0 };
 const char* sRelu[]		= { "relu ", (char*)0 };
 const char* sMaxpooling[] = { "Maxpooling ", (char*)0 };

 tMonCmd gCmdList[] =
 {
	 { (char*) "?", HelpDisp, sHelpDisp },
	 { (char*) "sqz", Squeeze, sSqueeze },
	 { (char*) "wa", WeightAnalyzer, sWeightAnalyer },
	 { (char*) "qunt", Float2int_converter, sfloat2int_conv },
	 { (char*) "run", RunScript, sScript },
	 { (char*) "img", Image_view, sImgView },
	 { (char*) "resize", Image_resize, sResize },
	 { (char*) "conv", Convolution, sConvolution },
	 { (char*) "relu", Relu, sRelu },
	 { (char*) "maxp", MaxPooling, sMaxpooling },
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
		printf("Test mode %s \n", argv[1]);
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
	if (argc != 1){
		printf("ex) sqz\n");
		return -1;
	}
	squeezeNet();

	return 0;
}

UINT32 WeightAnalyzer(int argc, char** argv)
{
	if (argc != 2){
		printf("ex) wa filename\n");
		return -1;
	}
	weightAnalyzerOfFile(argv[1]);
	return 0;
}

UINT32 Float2int_converter(int argc, char** argv)
{
	if (argc != 3){
		printf("ex) f2i in-filename out-filename\n");
		return -1;
	}
	float max = 0;
	max = absolute_max_value(argv[1]);
	Convert_INT(argv[1], argv[2], max, 8, STRING);
	return 0;
}

UINT32 Image_view(int argc, char** argv)
{
	if (argc != 2){
		printf("ex) img filename\n");
		return -1;
	}
	ImageViewer(argc, argv);
	return 0;
}

UINT32 Image_resize(int argc, char** argv)
{
	if (argc != 4){
		printf("ex)resize filename H-size W-size out-filename\n");
		return -1;
	}
	dataResize(argc, argv);
	return 0;
}


UINT32 Convolution(int argc, char** argv)
{
	if (argc != 14){
		printf("ex)conv in-filename in-H in-W in-ch kernel-filename k-H k-W k-ch out-filename o-H o-W stride padding \n");
		//                   1        2    3    4         5          6    7   8       9        10  11   12      13 
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	int k_H = atoi(argv[6]);
	int k_W = atoi(argv[7]);
	int k_ch = atoi(argv[8]);

	int o_H = atoi(argv[10]);
	int o_W = atoi(argv[11]);

	int stride = atoi(argv[12]);
	int padding = atoi(argv[13]);


	standard_convolution_int_rapper(argv[1], in_H, in_W, in_ch, argv[5], k_H, k_W, k_ch, argv[9], o_H, o_W,stride,padding);
	
	return 0;
}

UINT32 Relu(int argc, char** argv)
{
	if (argc != 6){
		printf("ex)relu in-filename in-H in-W in-ch out-filename \n");
		//                   1        2    3    4         5       
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	relu_rapper(argv[1], in_H, in_W, in_ch, argv[5]);

	return 0;
}

UINT32 MaxPooling(int argc, char** argv)
{
	if (argc != 9){
		printf("ex)maxp in-filename in-H in-W in-ch out-filename pooling-H pooling-W stride \n");
		//                   1        2    3    4         5        6           7        8
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	int maxp_H = atoi(argv[6]);
	int maxp_W = atoi(argv[7]);
	int stride = atoi(argv[8]);

	max_pooling_rapper(argv[1], in_H, in_W, in_ch, argv[5],maxp_H, maxp_W,stride);

	return 0;
}