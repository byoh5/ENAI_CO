#ifndef IMAGE_CV_H
#define IMAGE_CV_H

#include "stdafx.h"

extern UINT32 ImageViewer(int argc, char** argv);
extern UINT32 dataResize(int argc, char** argv);
extern UINT32 ImageViewerFromFile(char *input_file, int input_height, int input_width, int input_ch, int disp_ch);
#endif // IMAGE_CV_H