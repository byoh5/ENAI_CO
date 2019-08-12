#include "stdafx.h"
#include <windows.h>
#include "shell.h"
#include <iostream>

#include "layer.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)


void fire(int *input_data, int input_height, int input_width, int input_ch, int *kernel_data1, int *kernel_data2, int *kernel_data3, int kernel_s1, int kernel_s2, int kernel_s3,
	int *output_data, int output_height, int output_width, int output_ch, int shiftnum){

	int *fire_buf1 = (int*)calloc(input_height*input_width*kernel_s1, sizeof(int));// (sizeof(float)*input_height*input_width*kernel_s1);

	standard_convolution_int(input_data, input_height, input_width, input_ch, kernel_data1, 1, 1, kernel_s1, fire_buf1, input_height, input_width, 1, 0);
	ReLU_int(fire_buf1, input_height, input_width, kernel_s1);
	shift_int(fire_buf1, input_height*input_width*kernel_s1, shiftnum);

	standard_convolution_int(fire_buf1, input_height, input_width, kernel_s1, kernel_data2, 1, 1, kernel_s2, output_data, input_height, input_width, 1, 0);
	ReLU_int(output_data, input_height, input_width, kernel_s2);
	shift_int(output_data, input_height*input_width*kernel_s2, shiftnum);

	int *fire_buf2 = (&output_data[input_height * input_width * kernel_s2]);
	standard_convolution_int(fire_buf1, input_height, input_width, kernel_s1, kernel_data3, 3, 3, kernel_s3, fire_buf2, input_height, input_width, 1, 1);
	ReLU_int(fire_buf2, input_height, input_width, kernel_s3);
	shift_int(fire_buf2, input_height*input_width*kernel_s3, shiftnum);

	free(fire_buf1);
}


void init_2(int *input_data, char *name){

	FILE *fp;

	fp = fopen(name, "r");

	//******************************************************************************
	//input_data
	//******************************************************************************
	if (fp != NULL)
	{
		char buffer[20] = { 0, };
		char *pStr;
		int num1;
		int *in_data_buf = input_data;
		int i = 0;

		while (!feof(fp)){
			pStr = fgets(buffer, sizeof(buffer), fp);
			//printf("%s", pStr);
			if (pStr != NULL){
				num1 = atoi(pStr);
				//printf("%d. %f\n", i + 1, num1);
				input_data[i] = num1;
				i++;
			}
			else break;
		}
		fclose(fp);
	}
	else{
		printf("fopen fail\n");
	}

}


void init_squeezeNet_buf(void){
	int *input_data = (int*)malloc(sizeof(int)*(227 * 227 * 3));

	int *conv1 = (int*)malloc(sizeof(int)*(7 * 7 * 64) * 3);
	int *conv10 = (int*)malloc(sizeof(int)*(1 * 1 * 100) * 512);

	int *fire2_1 = (int*)malloc(sizeof(int)*(1 * 1 * 16) * 64);
	int *fire2_2 = (int*)malloc(sizeof(int)*(1 * 1 * 64) * 16);
	int *fire2_3 = (int*)malloc(sizeof(int)*(3 * 3 * 64) * 16);

	int *fire3_1 = (int*)malloc(sizeof(int)*(1 * 1 * 16) * 128);
	int *fire3_2 = (int*)malloc(sizeof(int)*(1 * 1 * 64) * 16);
	int *fire3_3 = (int*)malloc(sizeof(int)*(3 * 3 * 64) * 16);

	int *fire4_1 = (int*)malloc(sizeof(int)*(1 * 1 * 32) * 128);
	int *fire4_2 = (int*)malloc(sizeof(int)*(1 * 1 * 128) * 32);
	int *fire4_3 = (int*)malloc(sizeof(int)*(3 * 3 * 128) * 32);

	int *fire5_1 = (int*)malloc(sizeof(int)*(1 * 1 * 32) * 256);
	int *fire5_2 = (int*)malloc(sizeof(int)*(1 * 1 * 128) * 32);
	int *fire5_3 = (int*)malloc(sizeof(int)*(3 * 3 * 128) * 32);

	int *fire6_1 = (int*)malloc(sizeof(int)*(1 * 1 * 48) * 256);
	int *fire6_2 = (int*)malloc(sizeof(int)*(1 * 1 * 192) * 48);
	int *fire6_3 = (int*)malloc(sizeof(int)*(3 * 3 * 192) * 48);

	int *fire7_1 = (int*)malloc(sizeof(int)*(1 * 1 * 48) * 384);
	int *fire7_2 = (int*)malloc(sizeof(int)*(1 * 1 * 192) * 48);
	int *fire7_3 = (int*)malloc(sizeof(int)*(3 * 3 * 192) * 48);

	int *fire8_1 = (int*)malloc(sizeof(int)*(1 * 1 * 64) * 384);
	int *fire8_2 = (int*)malloc(sizeof(int)*(1 * 1 * 256) * 64);
	int *fire8_3 = (int*)malloc(sizeof(int)*(3 * 3 * 256) * 64);

	int *fire9_1 = (int*)malloc(sizeof(int)*(1 * 1 * 64) * 512);
	int *fire9_2 = (int*)malloc(sizeof(int)*(1 * 1 * 256) * 64);
	int *fire9_3 = (int*)malloc(sizeof(int)*(3 * 3 * 256) * 64);

	int *SQZ_buf1 = (int*)calloc(111 * 111 * 64, sizeof(int));
	int *SQZ_buf2 = (int*)calloc(55 * 55 * 64, sizeof(int));
	int *SQZ_buf3 = (int*)calloc(55 * 55 * 128, sizeof(int));
	int *SQZ_buf4 = (int*)calloc(55 * 55 * 128, sizeof(int));
	int *SQZ_buf5 = (int*)calloc(27 * 27 * 128, sizeof(int));
	int *SQZ_buf6 = (int*)calloc(27 * 27 * 256, sizeof(int));
	int *SQZ_buf7 = (int*)calloc(27 * 27 * 256, sizeof(int));
	int *SQZ_buf8 = (int*)calloc(13 * 13 * 256, sizeof(int));
	int *SQZ_buf9 = (int*)calloc(13 * 13 * 384, sizeof(int));
	int *SQZ_buf10 = (int*)calloc(13 * 13 * 384, sizeof(int));
	int *SQZ_buf11 = (int*)calloc(13 * 13 * 512, sizeof(int));
	int *SQZ_buf12 = (int*)calloc(13 * 13 * 512, sizeof(int));
	int *SQZ_buf13 = (int*)calloc(13 * 13 * 100, sizeof(int));

	int *result = (int*)malloc(sizeof(int)*(1 * 1 * 100));
	char * input7 = "weights/7img_text.txt";
	char * iconv1 = "weights/int_conv1_0.txt";
	char * iconv10 = "weights/int_conv10_0.txt";
	char * ifire2_1 = "weights/int_fire2_squeeze1x1_0.txt";
	char * ifire2_2 = "weights/int_fire2_expand1x1_0.txt";
	char * ifire2_3 = "weights/int_fire2_expand3x3_0.txt";
	char * ifire3_1 = "weights/int_fire3_squeeze1x1_0.txt";
	char * ifire3_2 = "weights/int_fire3_expand1x1_0.txt";
	char * ifire3_3 = "weights/int_fire3_expand3x3_0.txt";
	char * ifire4_1 = "weights/int_fire4_squeeze1x1_0.txt";
	char * ifire4_2 = "weights/int_fire4_expand1x1_0.txt";
	char * ifire4_3 = "weights/int_fire4_expand3x3_0.txt";
	char * ifire5_1 = "weights/int_fire5_squeeze1x1_0.txt";
	char * ifire5_2 = "weights/int_fire5_expand1x1_0.txt";
	char * ifire5_3 = "weights/int_fire5_expand3x3_0.txt";
	char * ifire6_1 = "weights/int_fire6_squeeze1x1_0.txt";
	char * ifire6_2 = "weights/int_fire6_expand1x1_0.txt";
	char * ifire6_3 = "weights/int_fire6_expand3x3_0.txt";
	char * ifire7_1 = "weights/int_fire7_squeeze1x1_0.txt";
	char * ifire7_2 = "weights/int_fire7_expand1x1_0.txt";
	char * ifire7_3 = "weights/int_fire7_expand3x3_0.txt";
	char * ifire8_1 = "weights/int_fire8_squeeze1x1_0.txt";
	char * ifire8_2 = "weights/int_fire8_expand1x1_0.txt";
	char * ifire8_3 = "weights/int_fire8_expand3x3_0.txt";
	char * ifire9_1 = "weights/int_fire9_squeeze1x1_0.txt";
	char * ifire9_2 = "weights/int_fire9_expand1x1_0.txt";
	char * ifire9_3 = "weights/int_fire9_expand3x3_0.txt";

	init_2(input_data, input7);
	init_2(conv1, iconv1);
	init_2(conv10, iconv10);
	init_2(fire2_1, ifire2_1);
	init_2(fire2_2, ifire2_2);
	init_2(fire2_3, ifire2_3);
	init_2(fire3_1, ifire3_1);
	init_2(fire3_2, ifire3_2);
	init_2(fire3_3, ifire3_3);
	init_2(fire4_1, ifire4_1);
	init_2(fire4_2, ifire4_2);
	init_2(fire4_3, ifire4_3);
	init_2(fire5_1, ifire5_1);
	init_2(fire5_2, ifire5_2);
	init_2(fire5_3, ifire5_3);
	init_2(fire6_1, ifire6_1);
	init_2(fire6_2, ifire6_2);
	init_2(fire6_3, ifire6_3);
	init_2(fire7_1, ifire7_1);
	init_2(fire7_2, ifire7_2);
	init_2(fire7_3, ifire7_3);
	init_2(fire8_1, ifire8_1);
	init_2(fire8_2, ifire8_2);
	init_2(fire8_3, ifire8_3);
	init_2(fire9_1, ifire9_1);
	init_2(fire9_2, ifire9_2);
	init_2(fire9_3, ifire9_3);

	//******************************************************************************
	//convolution layer 1 + ReLU
	//******************************************************************************
	standard_convolution_int(input_data, 227, 227, 1, conv1, 7, 7, 64, SQZ_buf1, 111, 111, 2, 0);
	ReLU_int(SQZ_buf1, 111, 111, 64);

	//******************************************************************************
	//Max pooling layer 1
	//******************************************************************************
	new_max_pool_int(SQZ_buf1, 111, 111, 64, SQZ_buf2, 3, 3, 2);
	shift_int(SQZ_buf2, 55 * 55 * 64, 1);

	//******************************************************************************
	//Fire layer 2-3
	//******************************************************************************
	fire(SQZ_buf2, 55, 55, 64, fire2_1, fire2_2, fire2_3, 16, 64, 64, SQZ_buf3, 55, 55, 128, 7);	//fire2
	fire(SQZ_buf3, 55, 55, 128, fire3_1, fire3_2, fire3_3, 16, 64, 64, SQZ_buf4, 55, 55, 128, 7);	//fire3

	//******************************************************************************
	//Max pooling layer 3
	//******************************************************************************
	new_max_pool_int(SQZ_buf4, 55, 55, 128, SQZ_buf5, 3, 3, 2);
	shift_int(SQZ_buf5, 27 * 27 * 128, 1);

	//******************************************************************************
	//Fire layer 4-5
	//******************************************************************************
	fire(SQZ_buf5, 27, 27, 128, fire4_1, fire4_2, fire4_3, 32, 128, 128, SQZ_buf6, 27, 27, 256, 7);	//fire4

	fire(SQZ_buf6, 27, 27, 256, fire5_1, fire5_2, fire5_3, 32, 128, 128, SQZ_buf7, 27, 27, 256, 7);	//fire5


	//******************************************************************************
	//Max pooling layer5
	//******************************************************************************
	new_max_pool_int(SQZ_buf7, 27, 27, 256, SQZ_buf8, 3, 3, 2);
	shift_int(SQZ_buf8, 13 * 13 * 256, 1);

	//******************************************************************************
	//Fire layer 6-9
	//******************************************************************************
	fire(SQZ_buf8, 13, 13, 256, fire6_1, fire6_2, fire6_3, 48, 192, 192, SQZ_buf9, 13, 13, 384, 7);		//fire6
	fire(SQZ_buf9, 13, 13, 384, fire7_1, fire7_2, fire7_3, 48, 192, 192, SQZ_buf10, 13, 13, 384, 7);	//fire7
	fire(SQZ_buf10, 13, 13, 384, fire8_1, fire8_2, fire8_3, 64, 256, 256, SQZ_buf11, 13, 13, 512, 7);	//fire8
	fire(SQZ_buf11, 13, 13, 512, fire9_1, fire9_2, fire9_3, 64, 256, 256, SQZ_buf12, 13, 13, 512, 7);	//fire9

	//******************************************************************************
	//convolution layer 10
	//******************************************************************************
	standard_convolution_int(SQZ_buf12, 13, 13, 512, conv10, 1, 1, 100, SQZ_buf13, 13, 13, 1, 0);
	ReLU_int(SQZ_buf13, 13, 13, 100);
	shift_int(SQZ_buf13, 13 * 13 * 100, 7);

	//******************************************************************************
	//Global average pooling layer
	//******************************************************************************
	global_average_pooling_int(SQZ_buf13, 13, 13, 100, result);

	int i;
	for (i = 0; i < 100; i++){
		printf("%d. %d\n", i, result[i]);
	}

	//******************************************************************************
	//Softmax
	//******************************************************************************
	softmax_int(result, 100);

	free(input_data);
	free(conv1);
	free(conv10);

	free(fire2_1);
	free(fire2_2);
	free(fire2_3);
	free(fire3_1);
	free(fire3_2);
	free(fire3_3);
	free(fire4_1);
	free(fire4_2);
	free(fire4_3);
	free(fire5_1);
	free(fire5_2);
	free(fire5_3);
	free(fire6_1);
	free(fire6_2);
	free(fire6_3);
	free(fire7_1);
	free(fire7_2);
	free(fire7_3);
	free(fire8_1);
	free(fire8_2);
	free(fire8_3);
	free(fire9_1);
	free(fire9_2);
	free(fire9_3);

	free(SQZ_buf1);
	free(SQZ_buf2);
	free(SQZ_buf3);
	free(SQZ_buf4);
	free(SQZ_buf5);
	free(SQZ_buf6);
	free(SQZ_buf7);
	free(SQZ_buf8);
	free(SQZ_buf9);
	free(SQZ_buf10);
	free(SQZ_buf11);
	free(SQZ_buf12);
	free(SQZ_buf13);

	free(result);

}




void squeezeNet(void){

	init_squeezeNet_buf();



}