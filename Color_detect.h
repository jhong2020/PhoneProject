#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include "Form1.h"
using namespace cv;
using namespace std;

namespace Project1 {
	string getColorName(const Scalar& mean_color);
	int hsv_Color();
}

