#ifndef SHELL_FUNCTION_H
#define SHELL_FUNCTION_H

#include "stdafx.h"

typedef struct {
	char* name;
	UINT32(*func)(int, char**);
	const char** helphelp;
} tMonCmd;




extern const char* sHelpDisp[];
extern const char* sSqueeze[];
extern const char* sWeightAnalyer[];
extern const char* sfloat2int_conv[];
extern const char* sScript[];
extern const char* sImgView[];
extern const char* sResize[];
extern const char* sConvolution[];
extern const char* sConcatenate[];
extern const char* sRelu[];
extern const char* sMaxpooling[];
extern const char* simgVfd[];
extern const char* sFire[];
extern const char* sGlobalAveragePooling[];
extern const char* sMaxvalue[];
extern const char* sQuantization[];
extern const char* sfloat2int_double[];
extern const char* sDepthwiseconv[];
extern const char* sFloat2Fixedpoint[];
extern const char* sFixedMul[];
extern const char* sFloat[];


extern tMonCmd gCmdList[];


extern UINT32 HelpDisp(int argc, char** argv);
extern UINT32 Squeeze(int argc, char** argv);
extern UINT32 WeightAnalyzer(int argc, char** argv);
extern UINT32 Float2int_converter(int argc, char** argv);
extern UINT32 RunScript(int argc, char** argv);
extern UINT32 Image_view(int argc, char** argv);
extern UINT32 Image_resize(int argc, char** argv);
extern UINT32 Convolution(int argc, char** argv);
extern UINT32 Concatenate(int argc, char** argv);
extern UINT32 Relu(int argc, char** argv);
extern UINT32 MaxPooling(int argc, char** argv);
extern UINT32 Image_viewFromData(int argc, char** argv);
extern UINT32 Fire(int argc, char** argv);
extern UINT32 Global_Average_pooling(int argc, char** argv);
extern UINT32 Max_value(int argc, char** argv);
extern UINT32 Max_value(int argc, char** argv);
extern UINT32 Quantization(int argc, char** argv);
extern UINT32 Float2int_converter_double(int argc, char** argv);
extern UINT32 DepthwiseConvolution(int argc, char** argv);
extern UINT32 Float2Fixedpoint(int argc, char** argv);
extern UINT32 Fixed_mul(int argc, char** argv);
extern UINT32 Float2Fixed_converter(int argc, char** argv);
extern UINT32 Shift(int argc, char** argv);

//Float Function
extern UINT32 Convolution_float(int argc, char** argv);
extern UINT32 DepthwiseConvolution_float(int argc, char** argv);
extern UINT32 Concatenate_float(int argc, char** argv);
extern UINT32 Relu_float(int argc, char** argv);
extern UINT32 MaxPooling_float(int argc, char** argv);
extern UINT32 Global_Average_pooling_float(int argc, char** argv);
extern UINT32 Max_value_float(int argc, char** argv);
extern UINT32 Batch_normalize_float(int argc, char** argv);
extern UINT32 Scale_bias_float(int argc, char** argv);
extern UINT32 Bias_float(int argc, char** argv);
#endif //SHELL_FUNCTION_H