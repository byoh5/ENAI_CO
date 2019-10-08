#ifndef MNIST_INT_H
#define MNIST_INT_H


extern void Mnist_mem_init(void);
extern void Mnist_inout_mem_init(void);
extern void Mnist_inout_mem_clear(void);
extern int Mnist_int(int *input_file);
extern int Mnist_int_opt(int *input_file);
#endif //MNIST_INT_H