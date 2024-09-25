#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "Form1.h"

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

namespace Project1 {
	// 원의 중심부 색상을 기준으로 렌즈인지 판단하는 함수 선언
	bool isLens(const Mat& img, const Vec3f& detectedCircle);

	// Hough Circle Transform을 사용하여 원을 검출하고 렌즈 개수를 출력하는 함수 선언
	bool hough_circle(Form1^ form, VideoCapture& cap, int& lensNum);
}
