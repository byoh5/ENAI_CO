#include "stdafx.h"
#include <windows.h>
#include "shell.h"
#include <iostream>
#include "fixedpoint.h"
#include "image_cv.h"
#include "mnist_int.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)


#define FRICTIONBIT 0
#define SHIFTBIT	13

int mul_fix(int a, int b, int fb) {
	int c = a * b;
	return c >> fb;
}

int div_fix(int a, int b, int fb) {
	int c = a << fb;
	return c / b;
}


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
					//	p_sum = kernel_data[kernel_weight*i + j] * input_data[input_width * num3 + num4];
						p_sum = mul_fix(kernel_data[kernel_weight*i + j], input_data[input_width * num3 + num4], FRICTIONBIT);
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
			input_buf  = input_data+(i*input_height*input_width);
			kernel_buf = kernel_data+((i*kernel_height*kernel_width) + (j*input_ch*kernel_height*kernel_width));
			//printf(" kernel_buf %x  i_ch:%d k_ch:%d> offset:%d \n", kernel_buf, i,j, (i * kernel_height * kernel_width) + (j * input_ch * kernel_height * kernel_width));
			output_buf = output_data+(j*output_height*output_width);
			if (input_buf > input_data + (input_height * input_width * input_ch)) {
				printf("Error input_buf %x > %x\n", input_buf, input_data + (input_height * input_width * input_ch));
			}
			if (kernel_buf > kernel_data + (kernel_height * kernel_width * kernel_ch* input_ch)) {
				printf("Error kernel_buf %x > %x\n", kernel_buf, kernel_data + (kernel_height * kernel_width * kernel_ch* input_ch));
			}
			if (output_buf > output_data + (output_height * output_width * kernel_ch)) {
				printf("Error output_buf %x > %x\n", output_buf, output_data + (output_height * output_width * kernel_ch));
			}
			new_conv_int(input_buf, input_height, input_width, kernel_buf, kernel_height, kernel_width, output_buf, output_height, output_width, stride, padding);
		}
	}
}

void depthwise_convolution_int(int *input_data, int input_height, int input_width, int input_ch, int *kernel_data, int kernel_height, int kernel_width, int kernel_ch,
	int *output_data, int output_height, int output_width, int stride, int padding){

	int *input_buf, *kernel_buf, *output_buf;
	int i, j;

	if (input_ch != kernel_ch){
		printf("Error depthwise input_ch != kernel_ch\n");
	}

	for (i = 0; i < input_ch; i++){
	//	for (j = 0; j < kernel_ch; j++){
			input_buf = input_data + (i*input_height*input_width);
			kernel_buf = kernel_data + (i*kernel_height*kernel_width);
			output_buf = output_data + (i*output_height*output_width);

			if (input_buf > input_data + (input_height * input_width * input_ch)) {
				printf("Error input_buf %x > %x\n", input_buf, input_data + (input_height * input_width * input_ch));
			}
			if (kernel_buf > kernel_data + (kernel_height * kernel_width * input_ch)) {
				printf("Error kernel_buf %x > %x\n", kernel_buf, kernel_data + (kernel_height * kernel_width * input_ch));
			}
			if (output_buf > output_data + (output_height * output_width * input_ch)) {
				printf("Error output_buf %x > %x\n", output_buf, output_data + (output_height * output_width * input_ch));
			}
			new_conv_int(input_buf, input_height, input_width, kernel_buf, kernel_height, kernel_width, output_buf, output_height, output_width, stride, padding);
	//	}
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
	
	int q, i, j, w, h, s = 0;
	int Max_value = input_data[0];

	for (q = 0; q < input_ch; q++){
		for (i = 0; i < input_height - (kernel_height - 1); i += stride){
			for (j = 0; j < input_weight - (kernel_weight - 1); j += stride){
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
		out_data[s] = sum / mul_in;
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

void file2data_char(char *filename, char *input_data){

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
		char *in_data_buf = input_data;
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
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);
	
	int* kernel_data = (int*)malloc(sizeof(int)*(kernel_height * kernel_width * kernel_ch * input_ch));
	if (kernel_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(kernel_file, kernel_data);

	int* output_data = (int*)malloc(sizeof(int)*(output_height * output_width * kernel_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if(output_data != NULL)memset(output_data, 0, sizeof(int)*(output_height * output_width * kernel_ch));

#define OUTDATA_CHECKER_CONV
#ifdef OUTDATA_CHECKER_CONV
	for (int i = 0; i < output_height*output_width*kernel_ch; i++){
		if (output_data[i] != 0){
			printf("Output data init value is not zero!\n");
		}
	}
#endif
	standard_convolution_int(input_data, input_height, input_width, input_ch, kernel_data, kernel_height, kernel_width, kernel_ch,output_data, output_height, output_width, stride, padding);


	int* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < output_height * output_width*kernel_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%d\n", (*pOutput_data) >> SHIFTBIT);
			pOutput_data++;
		}
		fclose(fp);
	}
	else{
		printf("fopen fail!\n");
	}
	if (input_data)		free(input_data);
	if (kernel_data)	free(kernel_data);
	if (output_data)	free(output_data);

}


void depthwise_convolution_int_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_file, int kernel_height, int kernel_width, int kernel_ch,
	char *output_file, int output_height, int output_width, int stride, int padding){

	if (input_ch != kernel_ch){
		printf("Error depthwise input_ch != kernel_ch\n");
		return;
	}

	int* input_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);

	int* kernel_data = (int*)malloc(sizeof(int)*(kernel_height * kernel_width * input_ch));
	if (kernel_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(kernel_file, kernel_data);

	int* output_data = (int*)malloc(sizeof(int)*(output_height * output_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(int)*(output_height * output_width * input_ch));

#define OUTDATA_CHECKER_CONV
#ifdef OUTDATA_CHECKER_CONV
	for (int i = 0; i < output_height*output_width*input_ch; i++){
		if (output_data[i] != 0){
			printf("Output data init value is not zero!\n");
		}
	}
#endif
	depthwise_convolution_int(input_data, input_height, input_width, input_ch, kernel_data, kernel_height, kernel_width, kernel_ch, output_data, output_height, output_width, stride, padding);


	int* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < output_height * output_width*kernel_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%d\n", (*pOutput_data) >> SHIFTBIT);
			pOutput_data++;
		}
		fclose(fp);
	}
	else{
		printf("fopen fail!\n");
	}
	if (input_data)		free(input_data);
	if (kernel_data)	free(kernel_data);
	if (output_data)	free(output_data);

}

void relu_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file)
{
	
	int* input_data = NULL;
	input_data=(int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);

	int* output_data = NULL;
	output_data =(int*)malloc(sizeof(int) * (input_height * input_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	int* pdata = output_data;
	memcpy(output_data, input_data, sizeof(int) * (input_height * input_width * input_ch));
	ReLU_int(output_data, input_height, input_width, input_ch);					// Relu
	
#define OUTDATA_CHECKER_RELU
#ifdef OUTDATA_CHECKER_RELU
	
	for (int i = 0; i < (input_height * input_width * input_ch); i++){
		if (*pdata < 0){
			printf("Relu : Error! This is minus(-) value!\n");
		}
		pdata++;
	}
#endif
	FILE *fp = NULL;
	if ((fp = fopen(output_file, "w")) == NULL) {
		printf("fopen fail\n");
		return ;
	}
	pdata = output_data;

	for (int i = 0; i < (input_height * input_width * input_ch); i++){
		//	printf("%d\n", output_data[i]); 
		fprintf(fp, "%d\n", *pdata);
		pdata++;
	}
	fclose(fp);

	if(input_data)	free(input_data);
	if(output_data)	free(output_data);
}

void max_pooling_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file, int pooling_height, int pooling_width, int stride)
{
	int output_height = (input_height - pooling_height) / stride + 1;
	int output_weight = (input_width - pooling_width) / stride + 1;

	int* input_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);

	int* output_data = (int*)malloc(sizeof(int)*(output_height * output_weight * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if(output_data)memset(output_data, 0, sizeof(int)*(output_height * output_weight * input_ch));

	new_max_pool_int(input_data, input_height, input_width, input_ch, output_data, pooling_height, pooling_width, stride);

	FILE *fp = NULL;
	if ((fp = fopen(output_file, "w")) == NULL) {
		printf("fopen fail\n");
		return;
	}

	int* pdata = output_data;

	for (int i = 0; i < output_height * output_weight * input_ch; i++){
		//	printf("%d\n", output_data[i]);
		fprintf(fp, "%d\n", *pdata);
		pdata++;
	}
	fclose(fp);

	if(input_data)	free(input_data);
	if(output_data)	free(output_data);


}

void concatenate_rapper(char* input_file1, char* input_file2, char* output_file)
{
	FILE *fd1, *fd2, *fd3;

	if ((fd1 = fopen(input_file1, "r")) == NULL){
		printf("fopen fail\n");
		return;
	}
	if ((fd2 = fopen(input_file2, "r")) == NULL){
		printf("fopen fail\n");
		return;
	}
	if ((fd3 = fopen(output_file, "w")) == NULL){
		printf("fopen fail\n");
		return;
	}
	int c;
	while ((c = getc(fd1)) != EOF) putc(c, fd3);
	while ((c = getc(fd2)) != EOF) putc(c, fd3);

	fclose(fd1);
	fclose(fd2);
	fclose(fd3);

}
/*
void divider_rapper(char* input_file1, char* output_file2, char* output_file)
{
	FILE *fd1, *fd2, *fd3;

	if ((fd1 = fopen(input_file1, "r")) == NULL){
		printf("fopen fail\n");
		return;
	}
	if ((fd2 = fopen(output_file2, "w")) == NULL){
		printf("fopen fail\n");
		return;
	}
	if ((fd3 = fopen(output_file, "w")) == NULL){
		printf("fopen fail\n");
		return;
	}
	char *pStr;

	char buffer[100] = { 0, };
	int i = 0;
	while (!feof(fd1)){
		pStr = fgets(buffer, sizeof(buffer), fd1);
		if (pStr != NULL){			
			i++;
		}
	}
	



	fclose(fd1);
	fclose(fd2);
	fclose(fd3);

}
*/

void global_average_pooling_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file)
{
	int* input_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);

	int* output_data = (int*)malloc(sizeof(int)*(1 * 1 * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data)memset(output_data, 0, sizeof(int)*(1 * 1 * input_ch));


	global_average_pooling_int(input_data, input_height, input_width, input_ch, output_data);


	FILE *fp = NULL;
	if ((fp = fopen(output_file, "w")) == NULL) {
		printf("fopen fail\n");
		return;
	}

	int* pdata = output_data;

	for (int i = 0; i < input_ch; i++){
		//	printf("%d\n", output_data[i]);
		fprintf(fp, "%d\n", *pdata);
		pdata++;
	}
	fclose(fp);

	if (input_data)	free(input_data);
	if (output_data)	free(output_data);

}

int max_val_int(int* input_data ,int input)
{
	int max = 0;
	int maxidx = 0;

	for (int i = 0; i<input; i++){
		if (*(input_data + i) > max){
			max = *(input_data + i);
			maxidx = i;
		}
	}

	return maxidx;
}


void max_val(char* input_file, int input)
{
	int* input_data = (int*)malloc(sizeof(int)*(input));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);

	int max = 0;
	int maxidx = 0;

	for (int i = 0; i<input; i++){
		if (*(input_data+i) > max){
			max = *(input_data+i);
			maxidx = i;
		}
	}

	printf("Max is %d\n", maxidx);

	if (input_data)	free(input_data);
}


void layer_dump(int *data,int size,char* outfile)
{
	FILE *fp = NULL;
	if ((fp = fopen(outfile, "w")) == NULL) {
		printf("fopen fail\n");
		return;
	}

	int* pdata = data;
	for (int i = 0; i < size; i++){
		fprintf(fp, "%d\n", *pdata);
		pdata++;
	}
	fclose(fp);

}

void batch_normalize(int *input_data, int input_height, int input_width, int input_ch, int *kernel_data_1, int *kernel_data_2, int *kernel_data_3, int *output_data){

	int *input_buf, *kernel_1_buf, *kernel_2_buf, *kernel_3_buf, *output_buf;
	int i, j;

	int mean = 0;
	int var = 0;
	int scalef = 0;

	for (i = 0; i < input_ch; i++){
		input_buf = input_data + (i*input_height*input_width);
		kernel_1_buf = kernel_data_1 + i;
		kernel_2_buf = kernel_data_2 + i;
		kernel_3_buf = kernel_data_3;
		output_buf = output_data + (i*input_height*input_width);


		mean = *kernel_1_buf;
		var = *kernel_2_buf;
		scalef = *kernel_3_buf;
	//	mean = mean / scalef;
		mean =div_fix(mean, scalef, FRICTIONBIT);
		//	var = var / scalef;
		var = div_fix(var, scalef, FRICTIONBIT);

		float var_f = fixed_to_float(var, FRICTIONBIT);
		float div_f = sqrt(var_f + .000001f);
		int   div_int = float_to_fixed(div_f, FRICTIONBIT);
		if (div_int == 0) div_int = 1;  // div 0 free


		for (j = 0; j < input_height*input_width; j++){
		//	*(output_buf + j) = (*(input_buf + j) - mean) / div_int;

			*(output_buf + j) = div_fix((*(input_buf + j) - mean), div_int, FRICTIONBIT);

		}
	}
}

void batch_normalize_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *kernel_2_file, char *kernel_3_file, char *output_file){


	int* input_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);

	int* kernel_data_1 = (int*)malloc(sizeof(int)*(input_ch));
	if (kernel_data_1 == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(kernel_1_file, kernel_data_1);

	int* kernel_data_2 = (int*)malloc(sizeof(int)*(input_ch));
	if (kernel_data_2 == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(kernel_2_file, kernel_data_2);

	int* kernel_data_3 = (int*)malloc(sizeof(int));
	if (kernel_data_3 == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(kernel_3_file, kernel_data_3);


	int* output_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(int)*(input_height * input_width * input_ch));


	batch_normalize(input_data, input_height, input_width, input_ch, kernel_data_1, kernel_data_2, kernel_data_3, output_data);

	int* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < input_height * input_width * input_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%d\n", *pOutput_data);
			pOutput_data++;
		}
		fclose(fp);
	}
	else{
		printf("fopen fail!\n");
	}
	if (input_data)		free(input_data);
	if (kernel_data_1)	free(kernel_data_1);
	if (kernel_data_2)	free(kernel_data_2);
	if (kernel_data_3)	free(kernel_data_3);
	if (output_data)	free(output_data);

}


void scale_bias(int *input_data, int input_height, int input_width, int input_ch, int *kernel_data_1, int *kernel_data_2, int *output_data){

	int *input_buf, *kernel_1_buf, *kernel_2_buf, *output_buf;
	int i, j;

	int scale = 0, shift = 0;

	for (i = 0; i < input_ch; i++){
		input_buf = input_data + (i*input_height*input_width);
		kernel_1_buf = kernel_data_1 + i;
		kernel_2_buf = kernel_data_2 + i;
		//kernel_3_buf = kernel_data_3;
		output_buf = output_data + (i*input_height*input_width);

		scale = *kernel_1_buf;
		shift = *kernel_2_buf;

		for (j = 0; j < input_height*input_width; j++){
		//	*(output_buf + j) = (*(input_buf + j)*scale) + shift;
			*(output_buf + j) = mul_fix(*(input_buf + j), scale, SHIFTBIT) + shift;
		}
	}
}

void scale_bias_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *kernel_2_file, char *output_file){


	int* input_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);

	int* kernel_data_1 = (int*)malloc(sizeof(int)*(input_ch));
	if (kernel_data_1 == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(kernel_1_file, kernel_data_1);

	int* kernel_data_2 = (int*)malloc(sizeof(int)*(input_ch));
	if (kernel_data_2 == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(kernel_2_file, kernel_data_2);


	int* output_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(int)*(input_height * input_width * input_ch));


	scale_bias(input_data, input_height, input_width, input_ch, kernel_data_1, kernel_data_2, output_data);

	int* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < input_height * input_width * input_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%d\n", *pOutput_data);
			pOutput_data++;
		}
		fclose(fp);
	}
	else{
		printf("fopen fail!\n");
	}
	if (input_data)		free(input_data);
	if (kernel_data_1)	free(kernel_data_1);
	if (kernel_data_2)	free(kernel_data_2);
	if (output_data)	free(output_data);

}

void bias(int *input_data, int input_height, int input_width, int input_ch, int *kernel_data_1, int *output_data){

	int *input_buf, *kernel_1_buf, *output_buf;
	int i, j;

	int bias = 0;

	for (i = 0; i < input_ch; i++){
		input_buf = input_data + (i*input_height*input_width);
		kernel_1_buf = kernel_data_1 + i;
		output_buf = output_data + (i*input_height*input_width);

		bias = *kernel_1_buf;

		for (j = 0; j < input_height*input_width; j++){
			*(output_buf + j) = *(input_buf + j) + bias;
		}
	}
}


void bias_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *output_file){


	int* input_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);

	int* kernel_data_1 = (int*)malloc(sizeof(int)*(input_ch));
	if (kernel_data_1 == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(kernel_1_file, kernel_data_1);

	int* output_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(int)*(input_height * input_width * input_ch));


	bias(input_data, input_height, input_width, input_ch, kernel_data_1, output_data);

	int* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < input_height * input_width * input_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%d\n", *pOutput_data);
			pOutput_data++;
		}
		fclose(fp);
	}
	else{
		printf("fopen fail!\n");
	}
	if (input_data)		free(input_data);
	if (kernel_data_1)	free(kernel_data_1);
	if (output_data)	free(output_data);

}

void scale_only(int *input_data, int input_height, int input_width, int input_ch, int scale, int *output_data){

	int i;

	for (i = 0; i < input_ch*input_height*input_width; i++){
		//*(output_data + i) = *(input_data + i) * scale;
		*(output_data + i) = mul_fix(*(input_data + i), scale, FRICTIONBIT);
	}
}

void scale_only_rapper(char *input_file, int input_height, int input_width, int input_ch, int scale, char *output_file){


	int* input_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);

	int* output_data = (int*)malloc(sizeof(int)*(input_height * input_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(int)*(input_height * input_width * input_ch));


	scale_only(input_data, input_height, input_width, input_ch, scale, output_data);

	int* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < input_height * input_width * input_ch; i++){
			//	printf("%f\n", *pOutput_data);
			fprintf(fp, "%d\n", (*pOutput_data) >> SHIFTBIT);
			pOutput_data++;
		}
		fclose(fp);
	}
	else{
		printf("fopen fail!\n");
	}
	if (input_data)		free(input_data);
	if (output_data)	free(output_data);

}

void fully_connected(int *input_data, int input_ch, int *kernel_data, int kernel_ch, int *output_data){
	int i = 0, k = 0;
	for (i = 0; i < kernel_ch; i++){
		for (k = 0; k < input_ch; k++){

				*(output_data + i) += *(input_data + k) * *(kernel_data + (i*input_ch) + k);
			//*(output_data + i) += mul_fix(*(input_data + k), *(kernel_data + (i*input_ch) + k), FRICTIONBIT);
		}
	}
}

void fully_connected_rapper(char *input_file, int input_ch, char *kernel_file, int kernel_ch, char *output_file){



	int* input_data = (int*)malloc(sizeof(int)*(input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(input_file, input_data);

	int* kernel_data = (int*)malloc(sizeof(int)*(input_ch *kernel_ch));
	if (kernel_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data(kernel_file, kernel_data);

	int* output_data = (int*)malloc(sizeof(int)*(kernel_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(int)*(kernel_ch));


	
	int i = 0, k = 0;
	for (i = 0; i < kernel_ch; i++){
		for (k = 0; k < input_ch; k++){

		//	*(output_data + i) += *(input_data + k) * *(kernel_data + (i*input_ch) + k);
			*(output_data + i) += mul_fix(*(input_data + k), *(kernel_data + (i*input_ch) + k), FRICTIONBIT);
		}
	}

	int* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < kernel_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%d\n", (*pOutput_data)>>SHIFTBIT);
			pOutput_data++;
		}
		fclose(fp);
	}
	else{
		printf("fopen fail!\n");
	}
	if (input_data)		free(input_data);
	if (kernel_data)	free(kernel_data);
	if (output_data)	free(output_data);

}


void data_view_rapper(char* inputfile,int size_h, int size_w,int offset, char* outfile){

	BYTE* input_data = (BYTE*)malloc(sizeof(BYTE)*(size_h*size_w));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	
	FILE *frp = NULL;
	frp = fopen(inputfile, "rb");
	int num = fread(input_data, sizeof(BYTE), 16, frp); // for header
	num = fread(input_data, sizeof(BYTE), (size_h*size_h), frp);
	for (int i = 0; i < offset; i++){
		num = fread(input_data, sizeof(BYTE), (size_h*size_h), frp);
	}

	BYTE* pOutput_data = input_data;
	FILE *fp = NULL;
	fp = fopen(outfile, "w");
	if (fp != NULL){
		for (int i = 0; i < (size_h*size_h); i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%d\n", *pOutput_data) ;
			pOutput_data++;
		}
		fclose(fp);
	}
	else{
		printf("fopen fail!\n");
	}

	dataView((BYTE*)input_data, size_h, size_w);

	if (input_data)		free(input_data);
	
}

void mnist_rapper(char* inputfile,char* idxfile){
	int size_h = 28;
	int size_w = 28;

	Mnist_mem_init();
	Mnist_inout_mem_init();
	Mnist_inout_mem_clear();


	BYTE* input_data = (BYTE*)malloc(sizeof(BYTE)*(size_h*size_w));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}

	int* input_data_int = (int*)malloc(sizeof(int)*(size_h*size_w));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}


	FILE *frp = NULL;
	frp = fopen(inputfile, "rb");
	 fread(input_data, sizeof(BYTE), 16, frp); // for header
	 
	FILE *fidx = NULL;
	fidx = fopen(idxfile, "rb");
	BYTE hdr[16], idx;
	 fread(hdr, sizeof(BYTE), 8, fidx); // for header
	

	int correct = 0;
	int miss = 0;
	int ai_ret = 0;
	while (!feof(frp)){
		fread(&idx, sizeof(BYTE), 1, fidx);
		fread(input_data, sizeof(BYTE), (size_h*size_h), frp);

		for (int i = 0; i < size_h*size_w; i++){
			*(input_data_int + i) = (int)*(input_data + i);
		}

		ai_ret = Mnist_int_opt(input_data_int);

		
		
		if (ai_ret == idx){
			correct++;
			printf("O");
		}
		else{
			miss++;
			printf("X: AI:%d - real:%d", ai_ret, idx);
		//	dataView((BYTE*)input_data, size_h, size_w);
		}

		
		printf("[%d]Accuracy : %f\n", correct + miss, ((float)correct / (float)(correct + miss)));
//		printf("Num :%d\n", ai_ret);

		
	
	}

	printf("Correct %d miss %d\n", correct, miss);
	printf("Accuracy : %f\n", ((float)correct / (float)(correct + miss)));

	if (input_data)		free(input_data);

}
