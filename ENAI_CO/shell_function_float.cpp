#include "stdafx.h"
#include "shell.h"
#include "shell_function.h"
#include "squeezeNet.h"
#include "analyzer.h"
#include "analyzer_f.h"
#include "float2int_conv.h"
#include "image_cv.h"
#include "layer.h"
#include "layer_f.h"
#include "fixedpoint.h"

#include <fstream>
#include <iostream>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)






UINT32 Convolution_float(int argc, char** argv)
{
	if (argc != 14){
		printf("ex)conv_f in-filename in-H in-W in-ch kernel-filename k-H k-W k-ch out-filename o-H o-W stride padding \n");
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


	standard_convolution_float_rapper(argv[1], in_H, in_W, in_ch, argv[5], k_H, k_W, k_ch, argv[9], o_H, o_W, stride, padding);

	return 0;
}

UINT32 DepthwiseConvolution_float(int argc, char** argv)
{
	if (argc != 14){
		printf("ex)dwcv_f in-filename in-H in-W in-ch kernel-filename k-H k-W k-ch out-filename o-H o-W stride padding \n");
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


	depthwise_convolution_float_rapper(argv[1], in_H, in_W, in_ch, argv[5], k_H, k_W, k_ch, argv[9], o_H, o_W, stride, padding);

	return 0;
}


UINT32 Fully_connected_float(int argc, char** argv)
{
	if (argc != 6){
		printf("ex)fulc_f in-filename in-ch kernel-filename k-ch out-filename  \n");
		//                   1         2          3           4         5          
		return -1;
	}
	
	int in_ch = atoi(argv[2]);

	
	int k_ch = atoi(argv[4]);

	fully_connected_float_rapper(argv[1], in_ch, argv[3], k_ch, argv[5]);

	return 0;
}

UINT32 Concatenate_float(int argc, char** argv)
{
	if (argc != 4){
		printf("ex)conc_f in-filename1 in-filename2 out-filename \n");
		//                   1            2              3   
		return -1;
	}
	concatenate_float_rapper(argv[1], argv[2], argv[3]);

	return 0;
}



UINT32 Relu_float(int argc, char** argv)
{
	if (argc != 6){
		printf("ex)relu_f in-filename in-H in-W in-ch out-filename \n");
		//                   1        2    3    4         5       
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	relu_float_rapper(argv[1], in_H, in_W, in_ch, argv[5]);

	return 0;
}

UINT32 MaxPooling_float(int argc, char** argv)
{
	if (argc != 9){
		printf("ex)maxp_f in-filename in-H in-W in-ch out-filename pooling-H pooling-W stride \n");
		//                   1        2    3    4         5        6           7        8
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	int maxp_H = atoi(argv[6]);
	int maxp_W = atoi(argv[7]);
	int stride = atoi(argv[8]);

	max_pooling_float_rapper(argv[1], in_H, in_W, in_ch, argv[5], maxp_H, maxp_W, stride);

	return 0;
}





UINT32 Global_Average_pooling_float(int argc, char** argv)
{
	if (argc != 6){
		printf("ex)gavp_f in-filename in-H in-W in-ch out-file\n");
		//                   1        2    3    4     5      
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	global_average_pooling_float_rapper(argv[1], in_H, in_W, in_ch, argv[5]);

	return 0;
}

UINT32 Max_value_float(int argc, char** argv)
{
	if (argc != 3){
		printf("ex)maxv_f in-filename insize \n");
		//                   1        2          
		return -1;
	}
	int size = atoi(argv[2]);


	max_val_float(argv[1], size);

	return 0;
}


UINT32 Batch_normalize_float(int argc, char** argv)
{
	if (argc != 9){
		printf("ex)bcnm_f in-filename in-H in-W in-ch k-file1 k-file1 k-file1 out-filename \n");
		//                     1        2    3    4      5       6      7          8        
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	batch_normalize_float_rapper(argv[1], in_H, in_W, in_ch, argv[5], argv[6], argv[7], argv[8]);

	return 0;
}

UINT32 Scale_bias_float(int argc, char** argv)
{
	if (argc != 8){
		printf("ex)scbi_f in-filename in-H in-W in-ch k-file1 k-file1 out-filename \n");
		//                     1        2    3    4      5       6          7                  
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	scale_bias_float_rapper(argv[1], in_H, in_W, in_ch, argv[5], argv[6], argv[7]);

	return 0;
}

UINT32 Bias_float(int argc, char** argv)
{
	if (argc != 7){
		printf("ex)bias_f in-filename in-H in-W in-ch k-file1 out-filename \n");
		//                     1        2    3    4      5       6                            
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);

	bias_float_rapper(argv[1], in_H, in_W, in_ch, argv[5], argv[6]);

	return 0;
}

UINT32 Scale_only_float(int argc, char** argv)
{
	if (argc != 7){
		printf("ex)scon_f in-filename in-H in-W in-ch scale out-filename \n");
		//                     1        2    3    4      5         6                             
		return -1;
	}
	int in_H = atoi(argv[2]);
	int in_W = atoi(argv[3]);
	int in_ch = atoi(argv[4]);
	float scale = atof(argv[5]);

	scale_only_float_rapper(argv[1], in_H, in_W, in_ch, scale ,argv[6]);

	return 0;
}