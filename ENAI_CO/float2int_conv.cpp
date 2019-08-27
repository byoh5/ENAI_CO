#include "stdafx.h"
#include <iostream>
#include "float2int_conv.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)

float absolute_max_value(char* FILE_name) {

	FILE* fp=NULL;
	if ((fp = fopen(FILE_name, "r")) == NULL){
		printf("fopen fail! \n");
		return NULL;
	}

	if (fp != NULL) {
		char buffer[20] = { 0, };
		char* pStr;
		float num1;
		float max = 0;
		float min = 0;

		while (!feof(fp)) {
			pStr = fgets(buffer, sizeof(buffer), fp);
			if (pStr != NULL) {
				num1 = atof(pStr);
				if (max < num1) max = num1;
				if (min > num1) min = num1;
			}
		}
		float abs_max = (fabs(max) > fabs(min)) ? fabs(max) : fabs(min);
		fclose(fp);
		return abs_max;
	}
	else {
		printf("The file does not exist.\n");
		return NULL;
	}

}

void Convert_Float2INT(char *input_name, char *output_name, float ALL_max, int bits, int TYPE2){

	FILE *fp;
	FILE *fp1;

	if ((fp = fopen(input_name, "r")) == NULL){
		printf("fopen fail! \n");
		return;
	}
	if (TYPE2 == STRING) {
		if ((fp1 = fopen(output_name, "w")) == NULL) {
			printf("fopen fail! \n");
			return;
		}
	}
	else if (TYPE2 == BINARY) {
		if ((fp1 = fopen(output_name, "wb")) == NULL) {
			printf("fopen fail! \n");
			return;
		}
	}


	if (fp != NULL)
	{
		char buffer[20] = { 0, };
		char s1[20] = { 0, };
		char *pStr;
		float num1;
		int num2;
		float bit_max = (pow(2.0, bits - 1) - 1);
		float buf = (bit_max / ALL_max);

		while (!feof(fp)){
			pStr = fgets(buffer, sizeof(buffer), fp);
			if (pStr != NULL){
				num1 = atof(pStr);
				num2 = num1 * buf;

				if (num2 > bit_max) num2 = bit_max;
				if (num2 < -(bit_max + 1)) num2 = -(bit_max + 1);

				if (TYPE2 == STRING) {
					sprintf(s1, "%d", num2);
					fprintf(fp1, "%s\n", s1);
				}
				else if (TYPE2 == BINARY) fwrite(&num1, sizeof(char), 1, fp1);
			}
			else break;
		}
	}

	fclose(fp);
	fclose(fp1);
}

void Convert_INT2BYTE(char* input_name, char* output_name, float ALL_max, int bits, int TYPE2) {

	FILE* fp = NULL;
	FILE* fp1 = NULL;

	

	if ((fp = fopen(input_name, "r")) == NULL){
		printf("fopen fail! \n");
		return;
	}
	if (TYPE2 == STRING) {
		if ((fp1 = fopen(output_name, "w")) == NULL) {
			printf("fopen fail! \n");
			return;
		}
	}
	else if (TYPE2 == BINARY) {
		if ((fp1 = fopen(output_name, "wb")) == NULL) {
			printf("fopen fail! \n");
			return;
		}
	}

	if (fp != NULL)
	{
		char buffer[20] = { 0, };
		char s1[20] = { 0, };
		char *pStr;
		int num1;
		int num2;
		int bit_max = (pow(2.0, bits - 1) - 1);
		int buf = (int)ALL_max ;

		

		UINT32 shift=0;
		
		for (int i = 1; i < 32; i++){
			buf = buf >> 1;
			if (buf == 0){
				shift = i;
				break;
			}
		}

		printf("div %f %d %d \n", ALL_max, buf, shift);

		if (shift > (bits - 1))	shift = shift - (bits - 1);
		else			shift = 0;

		printf("#%d \n", shift);

		while (!feof(fp)){
			pStr = fgets(buffer, sizeof(buffer), fp);
			if (pStr != NULL){
				num1 = atof(pStr);
				num2 = num1 >> shift;

				if (num2 > bit_max){
					printf("Value over flow %d\n", num2);
				//	num2 = bit_max;
				}
				if (num2 < -(bit_max + 1)){
					printf("Value under flow %d\n", num2);
				//	num2 = -(bit_max + 1);
				}
				if (TYPE2 == STRING) {
				//	sprintf(s1, "%d", num2);
					fprintf(fp1, "%d\n", num2);
				}
				else if (TYPE2 == BINARY) {
					if(fp1)	fwrite(&num1, sizeof(char), 1, fp1);
				}
			}
			else break;
		}
	}

	if(fp)  fclose(fp);
	if(fp1) fclose(fp1);
}

void Convert_shift(char* input_name, char* output_name, int shiftnum) {

	FILE* fp = NULL;
	FILE* fp1 = NULL;

	if ((fp = fopen(input_name, "r")) == NULL){
		printf("fopen fail! \n");
		return;
	}
	if ((fp1 = fopen(output_name, "w")) == NULL){
		printf("fopen fail! \n");
		return;
	}

	if (fp != NULL)
	{
		char buffer[50] = { 0, };
		char *pStr;
		int num2;
	
		while (!feof(fp)){
			pStr = fgets(buffer, sizeof(buffer), fp);
			if (pStr != NULL){
				num2 = atoi(pStr);
				fprintf(fp1, "%d\n", num2 >> shiftnum);
			}
		
		}
	}

	if (fp)  fclose(fp);
	if (fp1)  fclose(fp1);
	
}


void quantization_INT8_rapper(char* input_name, char* output_name,int shift)
{
#define QUANT_METHOD
#ifdef QUANT_METHOD
	float max = 0;
	max = absolute_max_value(input_name);
	Convert_INT2BYTE(input_name, output_name, max, 8, STRING);
#else
	Convert_shift(input_name, output_name, shift);
#endif
}


void quantization_float2INT8_rapper(char* input_name, char* output_name)
{
	float max = 0;
	max = absolute_max_value(input_name);
	Convert_Float2INT(input_name, output_name, max, 8, STRING);

}

void quantization_float2INT8_double_rapper(char* input1_name, char* output1_name, char* input2_name, char* output2_name)
{
	float max1 = 0;
	float max2 = 0;
	max1 = absolute_max_value(input1_name);
	max2 = absolute_max_value(input2_name);

	float new_max = (max1 > max2) ? max1 : max2;

	Convert_Float2INT(input1_name, output1_name, new_max, 8, STRING);
	Convert_Float2INT(input2_name, output2_name, new_max, 8, STRING);

}
