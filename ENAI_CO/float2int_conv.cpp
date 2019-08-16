#include "stdafx.h"
#include <iostream>
#include "float2int_conv.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)

float absolute_max_value(char *FILE_name){

	FILE *fp = fopen(FILE_name, "r");

	if (fp != NULL){
		char buffer[20] = { 0, };
		char *pStr;
		float num1;
		float max = 0;
		float min = 0;
	
		while (!feof(fp)){
			pStr = fgets(buffer, sizeof(buffer), fp);
			if (pStr != NULL){
				num1 = atof(pStr);
				if (max < num1) max = num1;
				if (min > num1) min = num1;	
			}		
		}
		float abs_max = (fabs(max) > fabs(min)) ? fabs(max) : fabs(min);
		fclose(fp);
		return abs_max;
	}
	else{
		printf("The file does not exist.\n");
		return NULL;
	}
	
}



void Convert_INT(char *input_name, char *output_name, float ALL_max, int bits, int TYPE2){

	FILE *fp=NULL;
	FILE *fp1=NULL;

	fp = fopen(input_name, "r");
	if (TYPE2 == STRING) fp1 = fopen(output_name, "w");
	else if (TYPE2 == BINARY) fp1 = fopen(output_name, "wb");

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
				//	sprintf(s1, "%d", num2);
					fprintf(fp1, "%d\n", num2);
				}
				else if (TYPE2 == BINARY) fwrite(&num1, sizeof(char), 1, fp1);
			}
			else break;
		}
	}

	fclose(fp);
	fclose(fp1);
}