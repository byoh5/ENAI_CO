#include "stdafx.h"
#include "shell.h"
#include "shell_function.h"
#include "squeezeNet.h"
#include "analyzer.h"
#include "analyzer_f.h"
#include "float2int_conv.h"
#include "image_cv.h"
#include "layer.h"
#include "fixedpoint.h"
#include "shell_function.h"

#include <fstream>
#include <iostream>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)


const char* sHelpDisp[] = { "Shell command list (! : Repeat command)", (char*)0 };
const char* sSqueeze[] = { "Squeeze", (char*)0 };
const char* sWeightAnalyer[] = { "WeightAnalyer", (char*)0 };
const char* sQuantization[] = { "Quantization", (char*)0 };
const char* sfloat2int_conv[] = { "float 2 int ", (char*)0 };
const char* sfloat2int_double[] = { "float 2 int double", (char*)0 };
const char* sScript[] = { "Script", (char*)0 };
const char* sImgView[] = { "img ", (char*)0 };
const char* sResize[] = { "resize ", (char*)0 };
const char* sConvolution[] = { "convolution ", (char*)0 };
const char* sConcatenate[] = { "concatenate ", (char*)0 };
const char* sRelu[] = { "relu ", (char*)0 };
const char* sMaxpooling[] = { "Maxpooling ", (char*)0 };
const char* simgVfd[] = { "Image view from data ", (char*)0 };
const char* sFire[] = { "Fire ", (char*)0 };
const char* sGlobalAveragePooling[] = { "GrobalAveragePooling ", (char*)0 };
const char* sMaxvalue[] = { "Maxvalue ", (char*)0 };
const char* sDepthwiseconv[] = { "depthwise convolution ", (char*)0 };
const char* sFloat2Fixedpoint[] = { "Float to Fixedpoint convertion", (char*)0 };
const char* sFixedMul[] = { "Fixed multipiler", (char*)0 };
const char* sShift[] = { "Shift", (char*)0 };

const char* sFloat[] = { "This is float function", (char*)0 };
const char* sInt[] = { "This is int function", (char*)0 };

tMonCmd gCmdList[] =
{
	{ (char*) "?", HelpDisp, sHelpDisp },
	{ (char*) "sqz", Squeeze, sSqueeze },
	{ (char*) "wa", WeightAnalyzer, sWeightAnalyer },
	{ (char*) "qunt", Quantization, sQuantization },
	{ (char*) "f2ic", Float2int_converter, sfloat2int_conv },
	{ (char*) "f2id", Float2int_converter_double, sfloat2int_double },
	{ (char*) "run", RunScript, sScript },
	{ (char*) "img", Image_view, sImgView },
	{ (char*) "resize", Image_resize, sResize },
	
	{ (char*) "imfd", Image_viewFromData, simgVfd },
	{ (char*) "fire", Fire, sFire },

	{ (char*) "2fix", Float2Fixedpoint, sFloat2Fixedpoint },
	{ (char*) "fixm", Fixed_mul, sFixedMul },
	{ (char*) "f2fc", Float2Fixed_converter, sFloat2Fixedpoint },
	{ (char*) "fx2f", Fixed2Float_converter, sFloat2Fixedpoint },
	{ (char*) "shif", Shift, sShift },

	{ (char*) "conv", Convolution, sConvolution },
	{ (char*) "dwcv", DepthwiseConvolution, sDepthwiseconv },
	{ (char*) "conc", Concatenate, sConcatenate },
	{ (char*) "relu", Relu, sRelu },
	{ (char*) "maxp", MaxPooling, sMaxpooling },
	{ (char*) "gavp", Global_Average_pooling, sGlobalAveragePooling },
	{ (char*) "maxv", Max_value, sMaxvalue },
	{ (char*) "bcnm", Batch_normalize, sInt },
	{ (char*) "scbi", Scale_bias, sInt },
	{ (char*) "bias", Bias, sInt },
	{ (char*) "fulc", Fully_connected, sInt },
	{ (char*) "scon", Scale_only, sInt },

	{ (char*) "dviu", Data_view, sInt },


	{ (char*) "conv_f", Convolution_float, sFloat },
	{ (char*) "dwcv_f", DepthwiseConvolution_float, sFloat },
	{ (char*) "conc_f", Concatenate_float, sFloat },
	{ (char*) "relu_f", Relu_float, sFloat },
	{ (char*) "maxp_f", MaxPooling_float, sFloat },
	{ (char*) "gavp_f", Global_Average_pooling_float, sFloat },
	{ (char*) "maxv_f", Max_value_float, sFloat },
	{ (char*) "bcnm_f", Batch_normalize_float, sFloat },
	{ (char*) "scbi_f", Scale_bias_float, sFloat },
	{ (char*) "bias_f", Bias_float, sFloat },
	{ (char*) "fulc_f", Fully_connected_float, sFloat },
	{ (char*) "scon_f", Scale_only_float, sFloat },
	{ (char*) "bmsb_f", Batch_normal_plus_scale_bias_float, sFloat },
	{ (char*) "diff_f", Diff_float, sFloat },
	{ 0, 0, 0 }
};

//extern tMonCmd gCmdList[];

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
	if (argc != 3 ){
		printf("ex) wa filename (I/F)\n");
		return -1;
	}

	if (strcmp(argv[2], "I") == NULL){

		weightAnalyzerOfFile(argv[1]);
	}
	else if (strcmp(argv[2], "F") == NULL){
		weightAnalyzerOfFileFloat(argv[1]);
	}
	else{
		printf("Error!\n");
	}

	return 0;
}

UINT32 Quantization(int argc, char** argv)
{
	if (argc != 4){
		printf("ex) qunt in-filename out-filename shift\n");
		//                    1          2          3
		return -1;
	}
	int shift = atoi(argv[3]);
	quantization_INT8_rapper(argv[1], argv[2],shift);
	return 0;
}


UINT32 Shift(int argc, char** argv)
{
	if (argc != 4){
		printf("ex) shif in-filename out-filename shift \n");
		//                    1          2          3
		return -1;
	}
	int shift = atoi(argv[3]);
	Shift_rapper(argv[1], argv[2], shift);
	return 0;
}

UINT32 Float2int_converter(int argc, char** argv)
{
	if (argc != 3){
		printf("ex) f2ic in-filename out-filename \n");
		//                    1          2          
		return -1;
	}
	quantization_float2INT8_rapper(argv[1], argv[2]);
	return 0;
}

UINT32 Float2Fixed_converter(int argc, char** argv)
{
	if (argc != 4){
		printf("ex) f2fc in-filename out-filename fb \n");
		//                    1          2         3 
		return -1;
	}
	int fb = atoi(argv[3]);

	quantization_float2fixed_rapper(argv[1], argv[2], fb);
	return 0;
}

UINT32 Fixed2Float_converter(int argc, char** argv)
{
	if (argc != 4){
		printf("ex) fx2f in-filename out-filename fb \n");
		//                    1          2         3 
		return -1;
	}
	int fb = atoi(argv[3]);

	quantization_fixed2float_rapper(argv[1], argv[2], fb);
	return 0;
}


UINT32 Float2int_converter_double(int argc, char** argv)
{
	if (argc != 5){
		printf("ex) f2id in1-filename  out1-filename  in2-filename  out2-filename \n");
		//                    1          2             3               4 
		return -1;
	}
	quantization_float2INT8_double_rapper(argv[1], argv[2], argv[3], argv[4]);
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

UINT32 Image_viewFromData(int argc, char** argv)
{
	if (argc != 6){
		printf("ex)imfd in-filename in-H in-W in-ch disp-ch");
		//                    1       2    3    4      5
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);
	int disp_ch = atoi(argv[5]);

	ImageViewerFromFile(argv[1], in_H, in_W, in_ch,disp_ch);
	
	return 0;
}


UINT32 Image_resize(int argc, char** argv)
{
	if (argc != 6){
		printf("ex)resize filename H-size W-size out-filename ch\n");
		//                    1       2    3            4     5
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

UINT32 DepthwiseConvolution(int argc, char** argv)
{
	if (argc != 14){
		printf("ex)dwcv in-filename in-H in-W in-ch kernel-filename k-H k-W k-ch out-filename o-H o-W stride padding \n");
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


	depthwise_convolution_int_rapper(argv[1], in_H, in_W, in_ch, argv[5], k_H, k_W, k_ch, argv[9], o_H, o_W, stride, padding);

	return 0;
}


UINT32 Concatenate(int argc, char** argv)
{
	if (argc != 4){
		printf("ex)conv in-filename1 in-filename2 out-filename \n");
		//                   1            2              3   
		return -1;
	}
	concatenate_rapper(argv[1], argv[2], argv[3]);

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


UINT32 Fire(int argc, char** argv)
{
	if (argc != 15){
		printf("ex)fire in-filename in-H in-W in-ch k-d1 k-d2 k-d3 k-s1 k-s2 k-s3   firename o-H o-W o-ch \n");
		//                   1        2    3    4     5    6    7   8     9    10      11     12  13  14   
   		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	int k_s1 = atoi(argv[8]);
	int k_s2 = atoi(argv[9]);
	int k_s3 = atoi(argv[10]);

	int o_H = atoi(argv[12]);
	int o_W = atoi(argv[13]);
	int o_ch = atoi(argv[14]);
	
	char conv_1[128];
	char conv_2[128];
	char conv_3[128];
	char relu_1[128];
	char relu_2[128];
	char relu_3[128];
	char qunt_1[128];
	char qunt_2[128];
	char qunt_3[128];
	char conc[128];
	char conc_t[128];

	sprintf(conv_1, "%s_conv_1_out.txt", argv[11]);
	sprintf(conv_2, "%s_conv_2_out.txt", argv[11]);
	sprintf(conv_3, "%s_conv_3_out.txt", argv[11]);
	sprintf(relu_1, "%s_relu_1_out.txt", argv[11]);
	sprintf(relu_2, "%s_relu_2_out.txt", argv[11]);
	sprintf(relu_3, "%s_relu_3_out.txt", argv[11]);
	sprintf(qunt_1, "%s_qunt_1_out.txt", argv[11]);
	sprintf(qunt_2, "%s_qunt_2_out.txt", argv[11]);
	sprintf(qunt_3, "%s_qunt_3_out.txt", argv[11]);
	sprintf(conc, "%s_conc_out.txt", argv[11]);
	sprintf(conc_t, "%s_conc_t_out.txt", argv[11]);

	standard_convolution_int_rapper(argv[1], in_H, in_W, in_ch, argv[5], 1, 1, k_s1, conv_1, in_H, in_W, 1, 0);

	relu_rapper(conv_1, in_H, in_W, k_s1, relu_1);

	quantization_INT8_rapper(relu_1, qunt_1,7);

	standard_convolution_int_rapper(qunt_1, in_H, in_W, k_s1, argv[6], 1, 1, k_s2, conv_2, in_H, in_W, 1, 0);

	relu_rapper(conv_2, in_H, in_W, k_s2, relu_2);

//	quantization_INT8_rapper(relu_2, qunt_2,7);

	standard_convolution_int_rapper(qunt_1, in_H, in_W, k_s1, argv[7], 3, 3, k_s3, conv_3, in_H, in_W, 1, 1);

	relu_rapper(conv_3, in_H, in_W, k_s3, relu_3);

//	quantization_INT8_rapper(relu_3, qunt_3,7);

//	concatenate_rapper(qunt_2, qunt_3, conc);

	concatenate_rapper(relu_2, relu_3, conc_t);

	quantization_INT8_rapper(conc_t, conc, 7);

	return 0;
}


UINT32 Global_Average_pooling(int argc, char** argv)
{
	if (argc != 6){
		printf("ex)gavp in-filename in-H in-W in-ch out-file\n");
		//                   1        2    3    4     5      
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	global_average_pooling_rapper(argv[1],in_H,in_W,in_ch,argv[5]);

	return 0;
}

UINT32 Max_value(int argc, char** argv)
{
	if (argc != 3){
		printf("ex)maxv in-filename insize \n");
		//                   1        2          
		return -1;
	}
	int size = atoi(argv[2]);
	

	max_val(argv[1], size);

	return 0;
}



UINT32 Float2Fixedpoint(int argc, char** argv)
{
	if (argc != 3){
		printf("ex)2fix float fb \n");
		//                1   2             
		return -1;
	}
	float f = atof(argv[1]);
	int fb = atoi(argv[2]);

	int num = float_to_fixed(f, fb);

	printf("%08x\n", num);
	printf("%f\n", fixed_to_float(num, fb));
	printf("%f\n", fixed_to_float(num, fb) - f);

	return 0;
}

int iwlbit(float floats) {
	
	int iwl = 0;
	int n = floats;
	if (n < 0) n *= (-1);
	if (n) iwl = log2((double)n) + 1;
	return iwl;
}


UINT32 Fixed_mul(int argc, char** argv)
{
	if (argc != 4){
		printf("ex)fixm float float fb \n");
		//                1    2     3           
		return -1;
	}

	float f1 = atof(argv[1]);
	float f2 = atof(argv[2]);
	int fb = atoi(argv[3]);
	
	int	i1 = float_to_fixed(f1, fb);
	int i2 = float_to_fixed(f2, fb);

	int ret = (i1 * i2)>>fb;

	float ret_f = fixed_to_float(ret, fb);

	printf("fixed :%f origin:%f\n", ret_f, (float)(f1 * f2));

	return 0;
}

UINT32 Batch_normalize(int argc, char** argv)
{
	if (argc != 9){
		printf("ex)bcnm in-filename in-H in-W in-ch k-file1 k-file1 k-file1 out-filename \n");
		//                     1        2    3    4      5       6      7          8        
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	batch_normalize_rapper(argv[1], in_H, in_W, in_ch, argv[5], argv[6], argv[7], argv[8]);

	return 0;
}

UINT32 Scale_bias(int argc, char** argv)
{
	if (argc != 8){
		printf("ex)scbi in-filename in-H in-W in-ch k-file1 k-file1 out-filename \n");
		//                     1        2    3    4      5       6          7                  
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	scale_bias_rapper(argv[1], in_H, in_W, in_ch, argv[5], argv[6], argv[7]);

	return 0;
}

UINT32 Bias(int argc, char** argv)
{
	if (argc != 7){
		printf("ex)bias in-filename in-H in-W in-ch k-file1 out-filename \n");
		//                     1        2    3    4      5       6                            
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	bias_rapper(argv[1], in_H, in_W, in_ch, argv[5], argv[6]);

	return 0;
}

UINT32 Scale_only(int argc, char** argv)
{
	if (argc != 7){
		printf("ex)scon in-filename in-H in-W in-ch scale out-filename \n");
		//                     1        2    3    4      5         6                             
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);
	int scale = atof(argv[5]);

	scale_only_rapper(argv[1], in_H, in_W, in_ch, scale, argv[6]);

	return 0;
}

UINT32 Fully_connected(int argc, char** argv)
{
	if (argc != 6){
		printf("ex)fulc in-filename in-ch kernel-filename k-ch out-filename  \n");
		//                   1         2          3           4         5          
		return -1;
	}

	int in_ch = atoi(argv[2]);


	int k_ch = atoi(argv[4]);

	fully_connected_rapper(argv[1], in_ch, argv[3], k_ch, argv[5]);

	return 0;
}

UINT32 Data_view(int argc, char** argv)
{
	if (argc != 6){
		printf("ex)dviu in-filename  size_h  size_w offset out-file  \n");
		//                   1         2        3     4       5        
		return -1;
	}

	int h = atoi(argv[2]);
	int w = atoi(argv[3]);
	int offset = atoi(argv[4]);

	data_view_rapper(argv[1], h, w,offset,argv[5]);

	return 0;
}



