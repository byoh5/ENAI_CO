#include "stdafx.h"
#include <windows.h>
#include "shell.h"
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)

void weightAnalyzerOfFile(char *FILE_name){


	FILE *fp = fopen(FILE_name, "r");

	if (fp != NULL){
		char buffer[20] = { 0, };
		char *pStr;
		int num1;
		int max = 0;
		int min = 0;
		int zeroCnt = 0;

		long long sum_up = 0;
		long long sum_down = 0;
		int sum_up_cnt = 0;
		int sum_down_cnt = 0;

		int avg_up	=0;
		int avg_down = 0;

		int totalCnt = 0;

		while (!feof(fp)){
			pStr = fgets(buffer, sizeof(buffer), fp);
			if (pStr != NULL){
				num1 = atoi(pStr);

				totalCnt++;

				if (max < num1) max = num1;
				if (min > num1) min = num1;
				if (num1 > 0){
					sum_up += num1;
					sum_up_cnt++;
				}
				else if (num1 < 0){
					sum_down += num1;
					sum_down_cnt++;
				}
				else{
					zeroCnt++;
				}
			
			}
			//else break;
		}
		if (sum_up_cnt !=0)		avg_up = sum_up / sum_up_cnt;
		if (sum_down_cnt != 0)	avg_down = sum_down / sum_down_cnt;

		int abs_max = (abs(max) > abs(min)) ? abs(max) : abs(min);

		printf("-------- %s --------\n", FILE_name);
		printf("ABS MAX Value	: %d\n", abs_max);
		printf("Positive MAX	: %d\n", max);
		printf("Negative MIN	: %d\n", min);
		printf("Positive AVG	: %d\n", avg_up);
		printf("Negative AVG	: %d\n", avg_down);
		printf("Zero			: %d\n", zeroCnt);
		printf("Total			: %d\n", totalCnt);
		printf("Zero percent	: %f\n", ((float)zeroCnt / (float)totalCnt) * 100);
		printf("----------------------------\n");
		
		fclose(fp);
	}
	else{
		printf("The file does not exist.\n");
	}


}

