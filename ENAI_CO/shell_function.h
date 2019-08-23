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
extern const char* sRelu[];
extern const char* sMaxpooling[];

extern tMonCmd gCmdList[];

extern UINT32 HelpDisp(int argc, char** argv);
extern UINT32 Squeeze(int argc, char** argv);
extern UINT32 WeightAnalyzer(int argc, char** argv);
extern UINT32 Float2int_converter(int argc, char** argv);
extern UINT32 RunScript(int argc, char** argv);
extern UINT32 Image_view(int argc, char** argv);
extern UINT32 Image_resize(int argc, char** argv);
extern UINT32 Convolution(int argc, char** argv);
extern UINT32 Relu(int argc, char** argv);
extern UINT32 MaxPooling(int argc, char** argv);

#endif //SHELL_FUNCTION_H