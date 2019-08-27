#ifndef FLOAT2INT_CONV_H
#define FLOAT2INT_CONV_H

extern float absolute_max_value(char *FILE_name);
extern void Convert_INT(char *input_name, char *output_name, float ALL_max, int bits, int TYPE2);

extern void quantization_INT8_rapper(char* input_name, char* output_name, int shift);

extern void quantization_float2INT8_rapper(char* input_name, char* output_name);
extern void quantization_float2INT8_double_rapper(char* input1_name, char* output1_name, char* input2_name, char* output2_name);
#define STRING 0
#define BINARY 1

#endif //FLOAT2INT_CONV_H