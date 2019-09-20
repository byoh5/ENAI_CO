#ifndef LAYER_F_H
#define LAYER_F_H

extern void new_conv_float(float *input_data, int input_height, int input_width, float *kernel_data, int kernel_height, int kernel_weight,
	float *output_data, int output_height, int output_width, int stride, int padding);
extern void standard_convolution_float(float *input_data, int input_height, int input_width, int input_ch, float *kernel_data, int kernel_height, int kernel_width, int kernel_ch,
	float *output_data, int output_height, int output_width, int stride, int padding);
extern void depthwise_convolution_float(float *input_data, int input_height, int input_width, int input_ch, float *kernel_data, int kernel_height, int kernel_width, int kernel_ch,
	float *output_data, int output_height, int output_width, int stride, int padding);
extern void ReLU_float(float *input_data, int input_height, int input_width, int input_ch);
extern void new_max_pool_float(float *input_data, int input_height, int input_weight, int input_ch, float *result, int kernel_height, int kernel_weight, int stride);
extern void global_average_pooling_float(float *input_data, int input_height, int input_width, int input_ch, float *out_data);
extern void softmax_float(float *input_data, int in_length);
extern void file2data_float(char *filename, float *input_data);
extern void standard_convolution_float_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_file, int kernel_height, int kernel_width, int kernel_ch,
	char *output_file, int output_height, int output_width, int stride, int padding);
extern void depthwise_convolution_float_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_file, int kernel_height, int kernel_width, int kernel_ch,
	char *output_file, int output_height, int output_width, int stride, int padding);
extern void relu_float_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file);
extern void max_pooling_float_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file, int pooling_height, int pooling_width, int stride);
extern void concatenate_float_rapper(char* input_file1, char* input_file2, char* output_file);
extern void global_average_pooling_float_rapper(char* input_file, int input_height, int input_width, int input_ch, char* output_file);
extern void max_val_float(char* input_file, int input);
extern void layer_dump_float(float *data, int size, char* outfile);
extern void batch_normalize_float_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *kernel_2_file, char *kernel_3_file, char *output_file);
extern void scale_bias_float_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *kernel_2_file, char *output_file);
extern void bias_float_rapper(char *input_file, int input_height, int input_width, int input_ch, char *kernel_1_file, char *output_file);
extern void fully_connected_float_rapper(char *input_file, int input_ch, char *kernel_file, int kernel_ch, char *output_file);
extern void scale_only_float_rapper(char *input_file, int input_height, int input_width, int input_ch, float scale, char *output_file);
extern void batch_normalize_plus_scale_bias_float_rapper(int input_ch, char *kernel_1_file, char *kernel_2_file, char *kernel_3_file, char *kernel_4_file, char *kernel_5_file, char *output_1_file, char *output_2_file);
extern void diff_float_rapper(char *input_file1, char *input_file2, char *output_file);

#endif //LAYER_H
