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
	// ���� �߽ɺ� ������ �������� �������� �Ǵ��ϴ� �Լ� ����
	bool isLens(const Mat& img, const Vec3f& detectedCircle);

	// Hough Circle Transform�� ����Ͽ� ���� �����ϰ� ���� ������ ����ϴ� �Լ� ����
	bool hough_circle(Form1^ form, VideoCapture& cap, int& lensNum);
}
