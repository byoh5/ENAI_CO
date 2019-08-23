#include "stdafx.h"
#include <windows.h>
#include "shell.h"
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)


void new_conv_int(int *input_data, int input_height, int input_width, int *kernel_data, int kernel_height, int kernel_weight,
	int *output_data, int output_height, int output_width, int stride, int padding){

	//int output_height = (input_height + 2 * padding - kernel_height) / stride + 1;
	//int output_width = (input_width + 2 * padding - kernel_width) / stride + 1;
	//output의 W, H를 구하는 공식. 이미 해당 W, H를 알고있다고 가정하여 연산하지 않음. 필요시 사용.

	int i, j, w, h, s = 0;
	int sum = 0, p_sum = 0;

	int total_width = input_width + 2 * padding;
	int total_height = input_height + 2 * padding;
	int padding1_w = total_width - padding;
	int padding1_h = total_height - padding;

	for (h = 0; h < total_width - kernel_height + 1; h += stride){
		for (w = 0; w < total_height - kernel_weight + 1; w += stride){
			for (i = 0; i < kernel_height; i++){
				for (j = 0; j < kernel_weight; j++){
					int num1 = h + i;
					int num2 = w + j;
					if ((num1 >= padding) && (num1 < padding1_h) && (num2 >= padding) && (num2 < padding1_w)){
						int num3 = num1 - padding;
						int num4 = num2 - padding;
						p_sum = kernel_data[kernel_weight*i + j] * input_data[input_width * num3 + num4];
						sum = sum + p_sum;
					}
				}
			}
			output_data[s] = output_data[s] + sum;
			//printf("%f\n", output_data[s]);
			s++;
			sum = 0;
		}
	}

}

void standard_convolution_int(int *input_data, int input_height, int input_width, int input_ch, int *kernel_data, int kernel_height, int kernel_width, int kernel_ch,
	int *output_data, int output_height, int output_width, int stride, int padding){

	int *input_buf, *kernel_buf, *output_buf;
	int i, j;
	for (i = 0; i < input_ch; i++){
		for (j = 0; j < kernel_ch; j++){
			input_buf = &((input_data[i*input_height*input_width]));
			kernel_buf = &((kernel_data[i*kernel_height*kernel_width + j*input_ch*kernel_height*kernel_width]));
			output_buf = &(output_data[j*output_height*output_width]);
			new_conv_int(input_buf, input_height, input_width, kernel_buf, kernel_height, kernel_width, output_buf, output_height, output_width, stride, padding);
		}
	}
}

void ReLU_int(int *input_data, int input_height, int input_width, int input_ch){
	int i;
	int all_mul = input_height * input_width * input_ch;
	for (i = 0; i < all_mul; i++){
		if (input_data[i] < 0) input_data[i] = 0;
	}
}

void new_max_pool_int(int *input_data, int input_height, int input_weight, int input_ch, int *result, int kernel_height, int kernel_weight, int stride){
	int output_height = (input_height - kernel_height) / stride + 1;
	int output_weight = (input_weight - kernel_weight) / stride + 1;
	int q, i, j, w, h, s = 0;
	int Max_value = input_data[0];

	for (q = 0; q < input_ch; q++){
		for (i = 0; i < input_height - 2; i += stride){
			for (j = 0; j < input_weight - 2; j += stride){
				Max_value = input_data[input_height*input_weight*q + input_weight*i + j];
				for (h = 0; h < kernel_height; h++){
					for (w = 0; w < kernel_weight; w++){
						int buf1 = input_data[input_weight*(i + h + input_height*q) + j + w];
						Max_value = (Max_value > buf1) ? Max_value : buf1;
					}
				}
				result[s] = Max_value;
				s++;
			}
		}
	}
}

void shift_int(int *input_data, int in_length, int shift_num){
	int i;
	for (i = 0; i < in_length; i++){
		input_data[i] = input_data[i] >> shift_num;
	}
}

void global_average_pooling_int(int *input_data, int input_height, int input_width, int input_ch, int *out_data){ // output size : 1X1Xinput_ch
	int i, j;
	int sum = 0;
	int s = 0;
	int mul_in = input_height * input_width;
	int *input_data_buf;

	for (j = 0; j < input_ch; j++){
		input_data_buf = (int*)(input_data + mul_in * j);

		for (i = 0; i < mul_in; i++){
			sum = sum + input_data_buf[i];
		}
		out_data[s] = sum;
		//printf("%f\n", s, sum / (13 * 13));
		sum = 0;
		s++;
	}
}

void softmax_int(int *input_data, int in_length){

	int i;
	int sum = 0;
	int acc = 100;
	int label[1] = { 0 };
	int max[1] = { 0 };
	//max[0] = input_data[0];

	for (i = 0; i<in_length; i++){
		if (input_data[i] > max[0]){
			max[0] = input_data[i];
			label[0] = i;
		}
	}
	/*
	//처음 For문 돌릴때 가장 큰 MAX value 5개를 뽑아
	//그 차이를 확인하여 acc를 확인하는게 좋을 듯 함.
	for (i = 0; i < in_length; i++){
	input_data[i] = input_data[i] - max[0];
	if (input_data[i]=!0){
	if (input_data[i] >= -1) { acc = 0;}
	else if (input_data[i] >= -1.5){ acc = acc - 22; }
	else if (input_data[i] >= -2){ acc = acc - 13; }
	else if (input_data[i] >= -2.5){ acc = acc - 8; }
	else if (input_data[i] >= -3){ acc = acc - 5; }
	}
	}
	*/
	//for (i = 0; i<in_length; i++)	if (max == input_data_buf[i]) label = i;
	//for (i = 0; i < in_length; i++){ printf("%d. %f\n", input_data[i]); }
	printf("Label1 = %d\n", label[0]);
	printf("Max = %d\n", max[0]);
	//printf("acc = %d\n", sum);

}

void file2data(char *filename,int *input_data ){

	FILE *fp;

	fp = fopen(filename, "r");

	//******************************************************************************
	//input_data
	//******************************************************************************
	if (fp != NULL)
	{
		char buffer[100] = { 0, };
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


void standard_convolution_int_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_file, int kernel_height, int kernel_width, int kernel_ch,
	char *output_file, int output_height, int output_width, int stride, int padding){
	
	int* input_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	file2data(input_file, input_data);
	
	int* kernel_data = (int*)malloc(sizeof(int)*(kernel_height * kernel_width * kernel_ch));
	file2data(kernel_file, kernel_data);

	int* output_data = (int*)malloc(sizeof(int)*(output_height * output_width * kernel_ch));
	memset(output_data, 0, sizeof(int)*(output_height * output_width * kernel_ch));

#define OUTDATA_CHECKER_CONV
#ifdef OUTDATA_CHECKER_CONV
	for (int i = 0; i < output_height*output_width*kernel_ch; i++){
		if (output_data[i] != 0){
			printf("Output data init value is not zero!\n");
		}
	}
#endif
	standard_convolution_int(input_data, input_height, input_width, input_ch, kernel_data, kernel_height, kernel_width, kernel_ch,
		output_data, output_height, output_width, stride, padding);

	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < output_height * output_width*kernel_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%d\n", output_data[i]);
		}
		fclose(fp);
	}
	else{
		printf("fopen fail!\n");
	}
	free(input_data);
	free(kernel_data);
	free(output_data);

}


void relu_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file)
{
	
	int* input_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	file2data(input_file, input_data);

	int* output_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));

	for (int i = 0; i < (input_height * input_width * input_ch); i++){
		output_data[i] = input_data[i];											// Data copy
	}
	ReLU_int(output_data, input_height, input_width, input_ch);					// Relu

#define OUTDATA_CHECKER_RELU
#ifdef OUTDATA_CHECKER_RELU
	for (int i = 0; i < (input_height * input_width * input_ch); i++){
		if (output_data[i] < 0){
			printf("Relu : Error! This is minus(-) value!\n");
		}
	}
#endif
	FILE *fp = NULL;
	fp = fopen(output_file, "w");

	for (int i = 0; i < (input_height * input_width * input_ch); i++){
		//	printf("%d\n", output_data[i]); 
		fprintf(fp, "%d\n", output_data[i]);
	}
	fclose(fp);

	free(input_data);
	free(output_data);
}

void max_pooling_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file, int pooling_height, int pooling_width, int stride)
{
	int output_height = (input_height - pooling_height) / stride + 1;
	int output_weight = (input_width - pooling_width) / stride + 1;

	int* input_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	file2data(input_file, input_data);

	int* output_data = (int*)malloc(sizeof(int)*(output_height * output_weight * input_ch));
	memset(output_data, 0, sizeof(int)*(output_height * output_weight * input_ch));

	new_max_pool_int(input_data, input_height, input_width, input_ch, output_data, pooling_height, pooling_width, stride);

	FILE *fp = NULL;
	fp = fopen(output_file, "w");

	for (int i = 0; i < output_height * output_weight * input_ch; i++){
		//	printf("%d\n", output_data[i]);
		fprintf(fp, "%d\n", output_data[i]);
	}
	fclose(fp);

	free(input_data);
	free(output_data);


}

