#include "stdafx.h"
#include "shell.h"
#include "shell_function.h"
#include "analyzer.h"
#include "analyzer_f.h"
#include "float2int_conv.h"
#include "image_cv.h"
#include "layer.h"
#include "layer_f.h"
#include "fixedpoint.h"
#include "shell_function.h"
#include "mnist_int.h"

//global
int* conv1_0;
int* conv1_1;
int* conv2_0;
int* conv2_1;
int* ip1_0;
int* ip1_1;
int* ip2_0;
int* ip2_1;

void Mnist_mem_init(void){
	// Kernel load
	conv1_0 = (int*)malloc(sizeof(int)*(5 * 5 * 20 * 1));
	file2data("fixed/conv1_0.txt", conv1_0);
	conv1_1 = (int*)malloc(sizeof(int)*(20));
	file2data("fixed/conv1_1.txt", conv1_1);
	conv2_0 = (int*)malloc(sizeof(int)*(5 * 5 * 50 * 20));
	file2data("fixed/conv2_0.txt", conv2_0);
	conv2_1 = (int*)malloc(sizeof(int)*(50));
	file2data("fixed/conv2_1.txt", conv2_1);
	ip1_0 = (int*)malloc(sizeof(int)*(500*800));
	file2data("fixed/ip1_0.txt", ip1_0);
	ip1_1 = (int*)malloc(sizeof(int)*(500));
	file2data("fixed/ip1_1.txt", ip1_1);
	ip2_0 = (int*)malloc(sizeof(int)*(500 * 10));
	file2data("fixed/ip2_0.txt", ip2_0);
	ip2_1 = (int*)malloc(sizeof(int)*(10));
	file2data("fixed/ip2_1.txt", ip2_1);
}

float* input_float;
int* input_int;
int* conv1_0_out;
int* conv1_1_out;
int* maxp1_out;
int* conv2_0_out;
int* conv2_1_out;
int* maxp2_out;
int* ip1_0_out;
int* ip1_1_out;
int* relu1_out;
int* ip2_0_out;
int* ip2_1_out;

void Mnist_inout_mem_init(void){
	input_float = (float*)malloc(sizeof(float)*(28 * 28));
	input_int = (int*)malloc(sizeof(int)*(28 * 28));
	conv1_0_out = (int*)malloc(sizeof(int)*(24 * 24 * 20));
	conv1_1_out = (int*)malloc(sizeof(int)*(24 * 24 * 20));
	maxp1_out	= (int*)malloc(sizeof(int)*(12 * 12 * 20));
	conv2_0_out = (int*)malloc(sizeof(int)*(8 * 8 * 50));
	conv2_1_out = (int*)malloc(sizeof(int)*(8 * 8 * 50));
	maxp2_out = (int*)malloc(sizeof(int)*(4 * 4 * 50));
	ip1_0_out = (int*)malloc(sizeof(int)*(500));
	ip1_1_out = (int*)malloc(sizeof(int)*(500));
	relu1_out = (int*)malloc(sizeof(int)*(500));
	ip2_0_out = (int*)malloc(sizeof(int)*(10));
	ip2_1_out = (int*)malloc(sizeof(int)*(10));
}

void Mnist_inout_mem_clear(void){
	if (input_float != NULL)	memset(input_float, 0, sizeof(float)*(28 * 28));
	if (input_int != NULL)		memset(input_int, 0, sizeof(int)*(28 * 28));
	if (conv1_0_out != NULL)	memset(conv1_0_out, 0, sizeof(int)*(24 * 24 * 20));
	if (conv1_1_out != NULL)	memset(conv1_1_out, 0, sizeof(int)*(24 * 24 * 20));
	if (maxp1_out != NULL)		memset(maxp1_out, 0,   sizeof(int)*(12 * 12 * 20));
	if (conv2_0_out != NULL)	memset(conv2_0_out, 0, sizeof(int)*(8 * 8 * 50));
	if (conv2_1_out != NULL)	memset(conv2_1_out, 0, sizeof(int)*(8 * 8 * 50));
	if (maxp2_out != NULL)	memset(maxp2_out, 0, sizeof(int)*(4 * 4 * 50));
	if (ip1_0_out != NULL)	memset(ip1_0_out, 0, sizeof(int)*(500));
	if (ip1_1_out != NULL)	memset(ip1_1_out, 0, sizeof(int)*(500));
	if (relu1_out != NULL)	memset(relu1_out, 0, sizeof(int)*(500));
	if (ip2_0_out != NULL)	memset(ip2_0_out, 0, sizeof(int)*(10));
	if (ip2_1_out != NULL)	memset(ip2_1_out, 0, sizeof(int)*(10));

}

void shift_layer(int* inputfile,int size ,int shift){
	
	for (int i = 0; i < size; i++){
		*(inputfile + i) = *(inputfile + i) >> shift;
	}

}

void int_2_float(int* inputfile, float* output, int size){

	for (int i = 0; i < size; i++){
		*(output + i) = *(inputfile + i);
	}

}

void float_2_fixed(float* inputfile, int* output,int size ,int fb){

	for (int i = 0; i < size; i++){
		*(output + i) = float_to_fixed(*(inputfile + i), fb);
	}

}



int Mnist_int(int *input_file){

	Mnist_inout_mem_clear();
	int number = 0;
	int_2_float(input_file, input_float, 28 * 28);
	scale_only_float(input_float, 28, 28, 1, 0.00390625, input_float);
	float_2_fixed(input_float, input_int, 28 * 28, 13);
	standard_convolution_int(input_int, 28, 28, 1, conv1_0, 5, 5, 20, conv1_0_out, 24, 24, 1, 0);
	shift_layer(conv1_0_out, (24 * 24 * 20), 13);
	bias(conv1_0_out, 24, 24, 20, conv1_1, conv1_1_out);
	new_max_pool_int(conv1_1_out, 24, 24, 20, maxp1_out, 2, 2, 2);
	standard_convolution_int(maxp1_out, 12, 12, 20, conv2_0, 5, 5, 50, conv2_0_out, 8, 8, 1, 0);
	shift_layer(conv2_0_out, (8 * 8 * 50), 13);
	bias(conv2_0_out,  8,  8, 50, conv2_1, conv2_1_out);
	new_max_pool_int(conv2_1_out,  8,  8, 50, maxp2_out, 2, 2, 2);
	fully_connected(maxp2_out, 800, ip1_0, 500, ip1_0_out);
	shift_layer(ip1_0_out, 500, 13);
	bias(ip1_0_out, 1, 1, 500, ip1_1, ip1_1_out);
	ReLU_int(ip1_1_out, 1, 1, 500);
	fully_connected(ip1_1_out, 500, ip2_0, 10, ip2_0_out);
	shift_layer(ip2_0_out, 10, 13);
	bias(ip2_0_out, 1, 1, 10, ip2_1, ip2_1_out);
	number = max_val_int(ip2_1_out, 10);


	return number;
}

void bit_checker(int *input_file, int size,int bit){
	 
	int min = -(1 << (bit - 1));
	int max = (1 << (bit - 1)) - 1;

	for (int i = 0; i < size; i++){
		if (*(input_file + i) < min){
			printf("under min : %d\n", *(input_file + i));
		}
		if (*(input_file + i) > max){
			printf("over max : %d\n", *(input_file + i));
		}
	}

}

#define CHECK_BIT 8
#define SHIFT_BIT 7

int bit_optimizer(int *input_file, int size, int bit){

	int c_min = -(1 << (bit - 1));
	int c_max = (1 << (bit - 1)) - 1;

	int shift = 0;
	int num = 0;
	int max = 0;
	int min = 0;

	int max_shift = 0;
	int min_shift = 0;

	for (int i = 0; i < size; i++){
		num = *(input_file + i);
		if (max < num) max = num;
		if (min > num) min = num;
	}

	for (; max > c_max; max = max >> 1){
		max_shift++;
	}
	for (; min < c_min; min = min >> 1){
		min_shift++;
	}

	shift = (max_shift >= min_shift) ? max_shift : min_shift;
	printf("Shift %d", shift);
	return shift;
}





int Mnist_int_opt(int *input_file){
	int bt1, bt2, bt3, bt4 = 0;
	int wt1, wt2, wt3, wt4 = 0;

	Mnist_inout_mem_clear();
	int number = 0;
	int_2_float(input_file, input_float, 28 * 28);
	scale_only_float(input_float, 28, 28, 1, 0.00390625, input_float);
	float_2_fixed(input_float, input_int, 28 * 28, SHIFT_BIT);
	printf("LV1\n");
	bt1 = bit_optimizer(input_int, 28 * 28, CHECK_BIT);
	shift_layer(input_int, (28 * 28), bt1);
	bit_checker(input_int, 28 * 28, CHECK_BIT);
	wt1 = bit_optimizer(conv1_0, (5 * 5 * 20), CHECK_BIT);
	shift_layer(conv1_0, (5 * 5 * 20), wt1);
	standard_convolution_int(input_int, 28, 28, 1, conv1_0, 5, 5, 20, conv1_0_out, 24, 24, 1, 0);
	shift_layer(conv1_0_out, (24 * 24 * 20), SHIFT_BIT - (bt1+wt1));
	printf("LV2\n");
	bias(conv1_0_out, 24, 24, 20, conv1_1, conv1_1_out);
	bt2 = bit_optimizer(conv1_1_out, (24 * 24 * 20), CHECK_BIT);
	shift_layer(conv1_1_out, (24 * 24 * 20), bt2);
	bit_checker(conv1_1_out, (24 * 24 * 20), CHECK_BIT);
	new_max_pool_int(conv1_1_out, 24, 24, 20, maxp1_out, 2, 2, 2);
	wt2 = bit_optimizer(conv2_0, (5 * 5 * 50 * 20), CHECK_BIT);
	shift_layer(conv2_0, (5 * 5 * 50 * 20), wt2);
	standard_convolution_int(maxp1_out, 12, 12, 20, conv2_0, 5, 5, 50, conv2_0_out, 8, 8, 1, 0);
	shift_layer(conv2_0_out, (8 * 8 * 50), SHIFT_BIT - (bt2+wt2));
	printf("LV3\n");
	//bit_checker(conv2_0_out, (8 * 8 * 50), CHECK_BIT);
	bias(conv2_0_out, 8, 8, 50, conv2_1, conv2_1_out);
	bt3 = bit_optimizer(conv2_1_out, (8 * 8 * 50), CHECK_BIT);
	shift_layer(conv2_1_out, (8 * 8 * 50), bt3);
	bit_checker(conv2_1_out, (8 * 8 * 50), CHECK_BIT);
	new_max_pool_int(conv2_1_out, 8, 8, 50, maxp2_out, 2, 2, 2);
	wt3 = bit_optimizer(ip1_0, (500*800), CHECK_BIT);
	shift_layer(ip1_0, (500 * 800), wt3);
	fully_connected(maxp2_out, 800, ip1_0, 500, ip1_0_out);
	shift_layer(ip1_0_out, 500, SHIFT_BIT - (bt3+wt3));
	printf("LV4\n");
	
	bias(ip1_0_out, 1, 1, 500, ip1_1, ip1_1_out);
	ReLU_int(ip1_1_out, 1, 1, 500);
	bt4 = bit_optimizer(ip1_1_out, 500, CHECK_BIT);
	shift_layer(ip1_1_out, 500, bt4);
	bit_checker(ip1_1_out, 500, CHECK_BIT);
	wt4 = bit_optimizer(ip2_0, (500 * 10), CHECK_BIT);
	shift_layer(ip2_0, (500 * 10), wt4);
	fully_connected(ip1_1_out, 500, ip2_0, 10, ip2_0_out);
	shift_layer(ip2_0_out, 10, SHIFT_BIT - (bt4+wt4));
	printf("LV5\n");
	bias(ip2_0_out, 1, 1, 10, ip2_1, ip2_1_out);
//	shift_layer(ip2_1_out, 10, 2);
//	bit_checker(ip2_1_out, 10, CHECK_BIT);
	number = max_val_int(ip2_1_out, 10);


	return number;
}

int Mnist_int_opt_(int *input_file){
	int bt1, bt2, bt3, bt4 = 0;


	Mnist_inout_mem_clear();
	int number = 0;
	int_2_float(input_file, input_float, 28 * 28);
	scale_only_float(input_float, 28, 28, 1, 0.00390625, input_float);
	float_2_fixed(input_float, input_int, 28 * 28, SHIFT_BIT);
	printf("LV1\n");
	bt1 = bit_optimizer(input_int, 28 * 28, CHECK_BIT);
	shift_layer(input_int, (28 * 28), bt1);
	bit_checker(input_int, 28 * 28, CHECK_BIT);
	standard_convolution_int(input_int, 28, 28, 1, conv1_0, 5, 5, 20, conv1_0_out, 24, 24, 1, 0);
	shift_layer(conv1_0_out, (24 * 24 * 20), SHIFT_BIT - bt1);
	printf("LV2\n");
	bias(conv1_0_out, 24, 24, 20, conv1_1, conv1_1_out);
	bt2 = bit_optimizer(conv1_1_out, (24 * 24 * 20), CHECK_BIT);
	shift_layer(conv1_1_out, (24 * 24 * 20), bt2);
	bit_checker(conv1_1_out, (24 * 24 * 20), CHECK_BIT);
	new_max_pool_int(conv1_1_out, 24, 24, 20, maxp1_out, 2, 2, 2);
	standard_convolution_int(maxp1_out, 12, 12, 20, conv2_0, 5, 5, 50, conv2_0_out, 8, 8, 1, 0);
	shift_layer(conv2_0_out, (8 * 8 * 50), SHIFT_BIT - bt2);
	printf("LV3\n");
	//bit_checker(conv2_0_out, (8 * 8 * 50), CHECK_BIT);
	bias(conv2_0_out, 8, 8, 50, conv2_1, conv2_1_out);
	bt3 = bit_optimizer(conv2_1_out, (8 * 8 * 50), CHECK_BIT);
	shift_layer(conv2_1_out, (8 * 8 * 50), bt3);
	bit_checker(conv2_1_out, (8 * 8 * 50), CHECK_BIT);
	new_max_pool_int(conv2_1_out, 8, 8, 50, maxp2_out, 2, 2, 2);
	fully_connected(maxp2_out, 800, ip1_0, 500, ip1_0_out);
	shift_layer(ip1_0_out, 500, SHIFT_BIT - bt3);
	printf("LV4\n");

	bias(ip1_0_out, 1, 1, 500, ip1_1, ip1_1_out);
	ReLU_int(ip1_1_out, 1, 1, 500);
	bt4 = bit_optimizer(ip1_1_out, 500, CHECK_BIT);
	shift_layer(ip1_1_out, 500, bt4);
	bit_checker(ip1_1_out, 500, CHECK_BIT);
	fully_connected(ip1_1_out, 500, ip2_0, 10, ip2_0_out);
	shift_layer(ip2_0_out, 10, SHIFT_BIT - bt4);
	printf("LV5\n");
	bias(ip2_0_out, 1, 1, 10, ip2_1, ip2_1_out);
	//	shift_layer(ip2_1_out, 10, 2);
	//	bit_checker(ip2_1_out, 10, CHECK_BIT);
	number = max_val_int(ip2_1_out, 10);


	return number;
}

int Mnist_do(void){
	
	Mnist_mem_init();
	Mnist_inout_mem_init();



	return 0;
}