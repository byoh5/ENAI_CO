#ifndef LAYER_H
#define LAYER_H



extern void new_conv_int(int *input_data, int input_height, int input_width, int *kernel_data, int kernel_height, int kernel_weight,
	int *output_data, int output_height, int output_width, int stride, int padding);
extern void standard_convolution_int(int *input_data, int input_height, int input_width, int input_ch, int *kernel_data, int kernel_height, int kernel_width, int kernel_ch,
	int *output_data, int output_height, int output_width, int stride, int padding);
extern void ReLU_int(int *input_data, int input_height, int input_width, int input_ch);
extern void new_max_pool_int(int *input_data, int input_height, int input_weight, int input_ch, int *result, int kernel_height, int kernel_weight, int stride);
extern void shift_int(int *input_data, int in_length, int shift_num);
extern void softmax_int(int *input_data, int in_length);
extern void global_average_pooling_int(int *input_data, int input_height, int input_width, int input_ch, int *out_data);
extern void standard_convolution_int_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_file, int kernel_height, int kernel_width, int kernel_ch,
	char *output_file, int output_height, int output_width, int stride, int padding);

extern void relu_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file);
extern void max_pooling_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file, int pooling_height, int pooling_width, int stride);

extern void file2data(char *filename, int *input_data);
extern void file2data_char(char *filename, char *input_data);
extern void concatenate_rapper(char* input_file1, char* input_file2, char* output_file);
extern void global_average_pooling_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file);


#endif  //LAYER