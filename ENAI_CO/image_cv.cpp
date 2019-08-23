#include "stdafx.h"


#include "opencv/cv.hpp"
#include "opencv/highgui.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <string>
using namespace cv;
using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)

UINT32 ImageViewer(int argc, char** argv){

	string imageName(argv[1]); // by default
	if (argc > 1)
	{
		imageName = argv[1];
	}
	Mat image;
	image = imread(imageName.c_str(), IMREAD_COLOR); // Read the file
	if (image.empty())                      // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window", image);                // Show our image inside it.
	waitKey(0); // Wait for a keystroke in the window
	destroyAllWindows();
	return 0;
}

UINT32 dataResize(int argc, char** argv){

	string imageName(argv[1]); // by default
	if (argc > 1)
	{
		imageName = argv[1];
	}
	Mat image;

	int size_h = atoi(argv[2]);
	int size_w = atoi(argv[3]);


	image = imread(imageName.c_str(), IMREAD_COLOR); // Read the file
	if (image.empty())                      // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	Mat res_image;
	resize(image, res_image, Size(size_h, size_w));
	Mat ch_bgr[3];
	split(res_image, ch_bgr);

	FILE *fp = NULL;
	fp = fopen(argv[4], "w");

	for (int i = 0; i < size_h * size_w; i++){
	//	printf("R %d\n", ch_bgr[2].data[i]);
		fprintf(fp, "%d\n", ch_bgr[2].data[i]);
	}

	for (int i = 0; i < size_h * size_w; i++){
	//	printf("G %d\n", ch_bgr[1].data[i]);
		fprintf(fp, "%d\n", ch_bgr[1].data[i]);
	}

	for (int i = 0; i < size_h * size_w; i++){
	//	printf("B %d\n", ch_bgr[0].data[i]);
		fprintf(fp, "%d\n", ch_bgr[0].data[i]);
	}
	fclose(fp);
	namedWindow("Resize  window", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Resize  window", res_image);

	waitKey(0); // Wait for a keystroke in the window
	destroyAllWindows();
	return 0;
}
