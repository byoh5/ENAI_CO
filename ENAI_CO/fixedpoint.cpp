#include "fixedpoint.h"
#include "ieee754.h"
#include "stdafx.h"
#include <math.h>"

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)

int float_to_fixed(float input,int fractionbit)
{
	return floor(input * (1 << fractionbit));
}

float fixed_to_float(int input, int fractionbit)
{
	return ((float)input / (float)(1 << fractionbit));
}


void print_binary(int num, int len) {

	unsigned int mask = 1 << len - 1;

	while (len--) {
		if (num & mask) printf("1");
		else printf("0");

		mask = mask >> 1;
	}
	
}

void printb_fix(int num, int wl, int iwl) {
	int fwl = wl - 1 - iwl;
	int integer = num >> fwl;

	print_binary(integer, iwl + 1);
	printf(".");
	print_binary(num, fwl);
	printf("\n");
}

void printd_fix(int num, int wl, int iwl) {
	int fwl = wl - 1 - iwl;
	int sign_filter = 1 << wl - 1;
	int int_filter = (1 << iwl) - 1;

	if (num & sign_filter) {
		printf("-");
		num = ~(num - 1);
	}

	int integer = (num >> fwl) & int_filter;
	float fraction = 0.0;
	float div = 0.5;
	unsigned int mask = 1 << fwl - 1;
	for (int i = 0; i < fwl; i++) {
		if (mask & num) {
			fraction += div;
		}
		mask = mask >> 1;
		div /= 2;
	}

	printf("%f\n", integer + fraction);

}

int mul_fix(int a, int b, int wl, int iwl) {
	int c = a * b;
	return c >> wl - 1 - iwl;
}

int div_fix(int a, int b, int wl, int iwl) {
	int c = a << wl - 1 - iwl;
	return c / b;
}

int optiwl(float floats[], int len) {
	int max = 0;
	int iwl = 0;

	for (int i = 0; i < len; i++) {
		int n = floats[i];

		if (n < 0) n *= (-1);
		if (max < n) max = n;
	}

	if (max) iwl = log2((double)max) + 1;

	return iwl;
}




void Convert_Float2Fixed(char *input_name, char *output_name, int fb){

	FILE *fp;
	FILE *fp1;

	if ((fp = fopen(input_name, "r")) == NULL){
		printf("fopen fail! \n");
		return;
	}

	if ((fp1 = fopen(output_name, "w")) == NULL) {
		printf("fopen fail! \n");
		return;
	}

	if (fp != NULL)
	{
		char buffer[100] = { 0, };
		char s1[100] = { 0, };
		char *pStr;
		float float_num;
		int num2;
	
		while (!feof(fp)){
			pStr = fgets(buffer, sizeof(buffer), fp);
			if (pStr != NULL){
				float_num = atof(pStr);
				num2 = float_to_fixed(float_num, fb);
				sprintf(s1, "%d", num2);
				fprintf(fp1, "%s\n", s1);
			}
			else break;
		}
	}

	fclose(fp);
	fclose(fp1);
}

void quantization_float2fixed_rapper(char* input_name, char* output_name, int fb)
{
	Convert_Float2Fixed(input_name, output_name,fb);
}

void Convert_Fixed2Float(char *input_name, char *output_name, int fb){

	FILE *fp;
	FILE *fp1;

	if ((fp = fopen(input_name, "r")) == NULL){
		printf("fopen fail! \n");
		return;
	}

	if ((fp1 = fopen(output_name, "w")) == NULL) {
		printf("fopen fail! \n");
		return;
	}

	if (fp != NULL)
	{
		char buffer[100] = { 0, };
		char s1[100] = { 0, };
		char *pStr;
		int int_num;
		float num2;

		while (!feof(fp)){
			pStr = fgets(buffer, sizeof(buffer), fp);
			if (pStr != NULL){
				int_num = atoi(pStr);
				num2 = fixed_to_float(int_num, fb);
				sprintf(s1, "%f", num2);
				fprintf(fp1, "%s\n", s1);
			}
			else break;
		}
	}

	fclose(fp);
	fclose(fp1);
}

void quantization_fixed2float_rapper(char* input_name, char* output_name, int fb)
{
	Convert_Fixed2Float(input_name, output_name, fb);
}