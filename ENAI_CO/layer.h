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
extern void divider_rapper(char* input_file1, char* output_file2, char* output_file);
extern void global_average_pooling_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file);

extern void max_val(char* input_file, int input);

extern void layer_dump(int *data, int size, char* outfile);
extern void depthwise_convolution_int_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_file, int kernel_height, int kernel_width, int kernel_ch,
	char *output_file, int output_height, int output_width, int stride, int padding);
extern void batch_normalize_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *kernel_2_file, char *kernel_3_file, char *output_file);
extern void scale_bias_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *kernel_2_file, char *output_file);
extern void bias_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *output_file);
extern void fully_connected_rapper(char *input_file, int input_ch, char *kernel_file, int kernel_ch, char *output_file);
extern void scale_only_rapper(char *input_file, int input_height, int input_width, int input_ch, int scale, char *output_file);

extern void data_view_rapper(char* inputfile, int size_h, int size_w, int offset, char* outfile);

#endif  //LAYER