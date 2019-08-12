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














#endif  //LAYER