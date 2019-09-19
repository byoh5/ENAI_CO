
#include "stdafx.h"
#include <windows.h>
#include "shell.h"
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)


void new_conv_float(float *input_data, int input_height, int input_width, float *kernel_data, int kernel_height, int kernel_weight,
	float *output_data, int output_height, int output_width, int stride, int padding){

	//int output_height = (input_height + 2 * padding - kernel_height) / stride + 1;
	//int output_width = (input_width + 2 * padding - kernel_width) / stride + 1;
	//output의 W, H를 구하는 공식. 이미 해당 W, H를 알고있다고 가정하여 연산하지 않음. 필요시 사용.

	int i, j, w, h, s = 0;
	float sum = 0, p_sum = 0;

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

void standard_convolution_float(float *input_data, int input_height, int input_width, int input_ch, float *kernel_data, int kernel_height, int kernel_width, int kernel_ch,
	float *output_data, int output_height, int output_width, int stride, int padding){

	float *input_buf, *kernel_buf, *output_buf;
	int i, j;

	for (i = 0; i < input_ch; i++){
		for (j = 0; j < kernel_ch; j++){
			input_buf = input_data + (i*input_height*input_width);
			kernel_buf = kernel_data + ((i*kernel_height*kernel_width) + (j*input_ch*kernel_height*kernel_width));
			//printf(" kernel_buf %x  i_ch:%d k_ch:%d> offset:%d \n", kernel_buf, i,j, (i * kernel_height * kernel_width) + (j * input_ch * kernel_height * kernel_width));
			output_buf = output_data + (j*output_height*output_width);
			if (input_buf > input_data + (input_height * input_width * input_ch)) {
				printf("Error input_buf %x > %x\n", input_buf, input_data + (input_height * input_width * input_ch));
			}
			if (kernel_buf > kernel_data + (kernel_height * kernel_width * kernel_ch* input_ch)) {
				printf("Error kernel_buf %x > %x\n", kernel_buf, kernel_data + (kernel_height * kernel_width * kernel_ch* input_ch));
			}
			if (output_buf > output_data + (output_height * output_width * kernel_ch)) {
				printf("Error output_buf %x > %x\n", output_buf, output_data + (output_height * output_width * kernel_ch));
			}
			new_conv_float(input_buf, input_height, input_width, kernel_buf, kernel_height, kernel_width, output_buf, output_height, output_width, stride, padding);
		}
	}
}

void depthwise_convolution_float(float *input_data, int input_height, int input_width, int input_ch, float *kernel_data, int kernel_height, int kernel_width, int kernel_ch,
	float *output_data, int output_height, int output_width, int stride, int padding){

	float *input_buf, *kernel_buf, *output_buf;
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
		new_conv_float(input_buf, input_height, input_width, kernel_buf, kernel_height, kernel_width, output_buf, output_height, output_width, stride, padding);
		//	}
	}
}

void ReLU_float(float *input_data, int input_height, int input_width, int input_ch){
	int i;
	int all_mul = input_height * input_width * input_ch;
	for (i = 0; i < all_mul; i++){
		if (input_data[i] < 0) input_data[i] = 0;
	}
}

void new_max_pool_float(float *input_data, int input_height, int input_weight, int input_ch, float *result, int kernel_height, int kernel_weight, int stride){
	int output_height = (input_height - kernel_height) / stride + 1;
	int output_weight = (input_weight - kernel_weight) / stride + 1;
	int q, i, j, w, h, s = 0;
	float Max_value = input_data[0];

	for (q = 0; q < input_ch; q++){
		for (i = 0; i < input_height - 2; i += stride){
			for (j = 0; j < input_weight - 2; j += stride){
				Max_value = input_data[input_height*input_weight*q + input_weight*i + j];
				for (h = 0; h < kernel_height; h++){
					for (w = 0; w < kernel_weight; w++){
						float buf1 = input_data[input_weight*(i + h + input_height*q) + j + w];
						Max_value = (Max_value > buf1) ? Max_value : buf1;
					}
				}
				result[s] = Max_value;
				s++;
			}
		}
	}
}
/*
void shift_float(float *input_data, int in_length, int shift_num){
	int i;
	for (i = 0; i < in_length; i++){
		input_data[i] = input_data[i] >> shift_num;
	}
}
*/
void global_average_pooling_float(float *input_data, int input_height, int input_width, int input_ch, float *out_data){ // output size : 1X1Xinput_ch
	int i, j;
	float sum = 0;
	int s = 0;
	int mul_in = input_height * input_width;
	float *input_data_buf;

	for (j = 0; j < input_ch; j++){
		input_data_buf = (float*)(input_data + mul_in * j);

		for (i = 0; i < mul_in; i++){
			sum = sum + input_data_buf[i];
		}
		out_data[s] = sum / mul_in;
		//printf("%f\n", s, sum / (13 * 13));
		sum = 0;
		s++;
	}
}

void softmax_float(float *input_data, int in_length){

	int i;
	int label[1] = { 0 };
	float max[1] = { 0 };
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

void file2data_float(char *filename, float *input_data){

	FILE *fp;

	fp = fopen(filename, "r");

	//******************************************************************************
	//input_data
	//******************************************************************************
	if (fp != NULL)
	{
		char buffer[100] = { 0, };
		char *pStr;
		float num1;
		float *in_data_buf = input_data;
		int i = 0;

		while (!feof(fp)){
			pStr = fgets(buffer, sizeof(buffer), fp);
			//printf("%s", pStr);
			if (pStr != NULL){
				num1 =(float)atof(pStr);
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


void batch_normalize_float(float *input_data, int input_height, int input_width, int input_ch, float *kernel_data_1, float *kernel_data_2, float *kernel_data_3, float *output_data){

	float *input_buf, *kernel_1_buf, *kernel_2_buf, *kernel_3_buf, *output_buf;
	int i, j;

	float mean = 0;
	float var = 0;
	float scalef = 0;

	for (i = 0; i < input_ch; i++){
			input_buf = input_data + (i*input_height*input_width);
			kernel_1_buf = kernel_data_1 + i;
			kernel_2_buf = kernel_data_2 + i;
			kernel_3_buf = kernel_data_3;
			output_buf = output_data + (i*input_height*input_width);
			
			
			mean	= *kernel_1_buf;
			var		= *kernel_2_buf;
			scalef	= *kernel_3_buf;
			mean = mean / scalef;
			var = var / scalef;
			for (j = 0; j < input_height*input_width; j++){
				*(output_buf+j) = (*(input_buf+j) - mean) / sqrt(var + .000001f);
			}
	}
}

void scale_bias_float(float *input_data, int input_height, int input_width, int input_ch, float *kernel_data_1, float *kernel_data_2, float *output_data){

	float *input_buf, *kernel_1_buf, *kernel_2_buf, *output_buf;
	int i, j;

	float scale=0, shift=0;

	for (i = 0; i < input_ch; i++){
		input_buf = input_data + (i*input_height*input_width);
		kernel_1_buf = kernel_data_1 + i;
		kernel_2_buf = kernel_data_2 + i;
		//kernel_3_buf = kernel_data_3;
		output_buf = output_data + (i*input_height*input_width);

		scale = *kernel_1_buf;
		shift = *kernel_2_buf;

		for (j = 0; j < input_height*input_width; j++){
			*(output_buf + j) = (*(input_buf + j)*scale) + shift;
		}
	}
}


void bias_float(float *input_data, int input_height, int input_width, int input_ch, float *kernel_data_1, float *output_data){

	float *input_buf, *kernel_1_buf, *output_buf;
	int i, j;

	float bias = 0;

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

void scale_only_float(float *input_data, int input_height, int input_width, int input_ch, float scale, float *output_data){

	int i;

	for (i = 0; i < input_ch*input_height*input_width ; i++){
		*(output_data + i) = *(input_data + i) * (float)scale;
		
	}
}



void standard_convolution_float_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_file, int kernel_height, int kernel_width, int kernel_ch,
	char *output_file, int output_height, int output_width, int stride, int padding){

	float* input_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(input_file, input_data);

	float* kernel_data = (float*)malloc(sizeof(float)*(kernel_height * kernel_width * kernel_ch * input_ch));
	if (kernel_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(kernel_file, kernel_data);

	float* output_data = (float*)malloc(sizeof(float)*(output_height * output_width * kernel_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(float)*(output_height * output_width * kernel_ch));

#define OUTDATA_CHECKER_CONV
#ifdef OUTDATA_CHECKER_CONV
	for (int i = 0; i < output_height*output_width*kernel_ch; i++){
		if (output_data[i] != 0){
			printf("Output data init value is not zero!\n");
		}
	}
#endif
	standard_convolution_float(input_data, input_height, input_width, input_ch, kernel_data, kernel_height, kernel_width, kernel_ch, output_data, output_height, output_width, stride, padding);


	float* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < output_height * output_width*kernel_ch; i++){
//			printf("%e\n", *pOutput_data);
			fprintf(fp, "%.15f\n", *pOutput_data);
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


void depthwise_convolution_float_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_file, int kernel_height, int kernel_width, int kernel_ch,
	char *output_file, int output_height, int output_width, int stride, int padding){

	if (input_ch != kernel_ch){
		printf("Error depthwise input_ch != kernel_ch\n");
		return;
	}

	float* input_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(input_file, input_data);

	float* kernel_data = (float*)malloc(sizeof(float)*(kernel_height * kernel_width * input_ch));
	if (kernel_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(kernel_file, kernel_data);

	float* output_data = (float*)malloc(sizeof(float)*(output_height * output_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(float)*(output_height * output_width * input_ch));

#define OUTDATA_CHECKER_CONV
#ifdef OUTDATA_CHECKER_CONV
	for (int i = 0; i < output_height*output_width*input_ch; i++){
		if (output_data[i] != 0){
			printf("Output data init value is not zero!\n");
		}
	}
#endif
	depthwise_convolution_float(input_data, input_height, input_width, input_ch, kernel_data, kernel_height, kernel_width, kernel_ch, output_data, output_height, output_width, stride, padding);


	float* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < output_height * output_width*kernel_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%.15f\n", *pOutput_data);
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


void fully_connected_float_rapper(char *input_file, int input_ch, char *kernel_file, int kernel_ch,char *output_file){



	float* input_data = (float*)malloc(sizeof(float)*(input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(input_file, input_data);

	float* kernel_data = (float*)malloc(sizeof(float)*( input_ch *kernel_ch));
	if (kernel_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(kernel_file, kernel_data);

	float* output_data = (float*)malloc(sizeof(float)*(kernel_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(float)*(kernel_ch));


	//depthwise_convolution_float(input_data, input_height, input_width, input_ch, kernel_data, kernel_height, kernel_width, kernel_ch, output_data, output_height, output_width, stride, padding);
	int i = 0, k = 0;
	for (i = 0; i < kernel_ch; i++){
		for (k = 0; k < input_ch; k++){

			*(output_data + i) += *(input_data + k) * *(kernel_data + (i*input_ch) + k);

		}
	}
		



	float* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < kernel_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%.15f\n", *pOutput_data);
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


void relu_float_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file)
{

	float* input_data = NULL;
	input_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(input_file, input_data);

	float* output_data = NULL;
	output_data = (float*)malloc(sizeof(float)* (input_height * input_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	float* pdata = output_data;
	memcpy(output_data, input_data, sizeof(float)* (input_height * input_width * input_ch));
	ReLU_float(output_data, input_height, input_width, input_ch);					// Relu

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
		return;
	}
	pdata = output_data;

	for (int i = 0; i < (input_height * input_width * input_ch); i++){
		//	printf("%d\n", output_data[i]); 
		fprintf(fp, "%.15f\n", *pdata);
		pdata++;
	}
	fclose(fp);

	if (input_data)	free(input_data);
	if (output_data)	free(output_data);
}

void max_pooling_float_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file, int pooling_height, int pooling_width, int stride)
{
	int output_height = (input_height - pooling_height) / stride + 1;
	int output_weight = (input_width - pooling_width) / stride + 1;

	float* input_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(input_file, input_data);

	float* output_data = (float*)malloc(sizeof(float)*(output_height * output_weight * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data)memset(output_data, 0, sizeof(float)*(output_height * output_weight * input_ch));

	new_max_pool_float(input_data, input_height, input_width, input_ch, output_data, pooling_height, pooling_width, stride);

	FILE *fp = NULL;
	if ((fp = fopen(output_file, "w")) == NULL) {
		printf("fopen fail\n");
		return;
	}

	float* pdata = output_data;

	for (int i = 0; i < output_height * output_weight * input_ch; i++){
		//	printf("%d\n", output_data[i]);
		fprintf(fp, "%.15f\n", *pdata);
		pdata++;
	}
	fclose(fp);

	if (input_data)	free(input_data);
	if (output_data)	free(output_data);


}

void concatenate_float_rapper(char* input_file1, char* input_file2, char* output_file)
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

void global_average_pooling_float_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file)
{
	float* input_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(input_file, input_data);

	float* output_data = (float*)malloc(sizeof(float)*(1 * 1 * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data)memset(output_data, 0, sizeof(float)*(1 * 1 * input_ch));


	global_average_pooling_float(input_data, input_height, input_width, input_ch, output_data);


	FILE *fp = NULL;
	if ((fp = fopen(output_file, "w")) == NULL) {
		printf("fopen fail\n");
		return;
	}

	float* pdata = output_data;

	for (int i = 0; i < input_ch; i++){
		//	printf("%d\n", output_data[i]);
		fprintf(fp, "%.15f\n", *pdata);
		pdata++;
	}
	fclose(fp);

	if (input_data)	free(input_data);
	if (output_data)	free(output_data);

}

void max_val_float(char* input_file, int input)
{
	float* input_data = (float*)malloc(sizeof(float)*(input));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(input_file, input_data);

	float max = 0;
	int maxidx = 0;

	for (int i = 0; i<input; i++){
		if (*(input_data + i) > max){
			max = *(input_data + i);
			maxidx = i;
		}
	}

	printf("Max is %d\n", maxidx);

	if (input_data)	free(input_data);
}


void layer_dump_float(float *data, int size, char* outfile)
{
	FILE *fp = NULL;
	if ((fp = fopen(outfile, "w")) == NULL) {
		printf("fopen fail\n");
		return;
	}

	float* pdata = data;
	for (int i = 0; i < size; i++){
		fprintf(fp, "%.15f\n", *pdata);
		pdata++;
	}
	fclose(fp);

}

void batch_normalize_float_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *kernel_2_file, char *kernel_3_file, char *output_file){

	
		float* input_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
		if (input_data == NULL) {
			printf("malloc fail\n");
			return;
		}
		file2data_float(input_file, input_data);

		float* kernel_data_1 = (float*)malloc(sizeof(float)*(input_ch));
		if (kernel_data_1 == NULL) {
			printf("malloc fail\n");
			return;
		}
		file2data_float(kernel_1_file, kernel_data_1);

		float* kernel_data_2 = (float*)malloc(sizeof(float)*(input_ch));
		if (kernel_data_2 == NULL) {
			printf("malloc fail\n");
			return;
		}
		file2data_float(kernel_2_file, kernel_data_2);

		float* kernel_data_3 = (float*)malloc(sizeof(float));
		if (kernel_data_3 == NULL) {
			printf("malloc fail\n");
			return;
		}
		file2data_float(kernel_3_file, kernel_data_3);


		float* output_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
		if (output_data == NULL) {
			printf("malloc fail\n");
			return;
		}
		if (output_data != NULL)memset(output_data, 0, sizeof(float)*(input_height * input_width * input_ch));


		batch_normalize_float(input_data, input_height, input_width, input_ch, kernel_data_1, kernel_data_2, kernel_data_3, output_data);

		float* pOutput_data = output_data;
		FILE *fp = NULL;
		fp = fopen(output_file, "w");
		if (fp != NULL){
			for (int i = 0; i < input_height * input_width * input_ch; i++){
				//	printf("%d\n", output_data[i]);
				fprintf(fp, "%.15f\n", *pOutput_data);
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

void scale_bias_float_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *kernel_2_file, char *output_file){


	float* input_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(input_file, input_data);

	float* kernel_data_1 = (float*)malloc(sizeof(float)*(input_ch));
	if (kernel_data_1 == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(kernel_1_file, kernel_data_1);

	float* kernel_data_2 = (float*)malloc(sizeof(float)*(input_ch));
	if (kernel_data_2 == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(kernel_2_file, kernel_data_2);


	float* output_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(float)*(input_height * input_width * input_ch));


	scale_bias_float(input_data, input_height, input_width, input_ch, kernel_data_1, kernel_data_2, output_data);

	float* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < input_height * input_width * input_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%.15f\n", *pOutput_data);
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


void bias_float_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *output_file){


	float* input_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(input_file, input_data);

	float* kernel_data_1 = (float*)malloc(sizeof(float)*(input_ch));
	if (kernel_data_1 == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(kernel_1_file, kernel_data_1);

	float* output_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(float)*(input_height * input_width * input_ch));


	bias_float(input_data, input_height, input_width, input_ch, kernel_data_1, output_data);

	float* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < input_height * input_width * input_ch; i++){
			//	printf("%d\n", output_data[i]);
			fprintf(fp, "%.15f\n", *pOutput_data);
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


void scale_only_float_rapper(char *input_file, int input_height, int input_width, int input_ch, float scale, char *output_file){


	float* input_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (input_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	file2data_float(input_file, input_data);

	float* output_data = (float*)malloc(sizeof(float)*(input_height * input_width * input_ch));
	if (output_data == NULL) {
		printf("malloc fail\n");
		return;
	}
	if (output_data != NULL)memset(output_data, 0, sizeof(float)*(input_height * input_width * input_ch));


	scale_only_float(input_data, input_height, input_width, input_ch, scale, output_data);

	float* pOutput_data = output_data;
	FILE *fp = NULL;
	fp = fopen(output_file, "w");
	if (fp != NULL){
		for (int i = 0; i < input_height * input_width * input_ch; i++){
		//	printf("%f\n", *pOutput_data);
			fprintf(fp, "%.15f\n", *pOutput_data);
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
