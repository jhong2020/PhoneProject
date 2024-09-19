#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "Form1.h"
#include "DetectCircles.h"

using namespace cv;
using namespace std;

namespace Project1 {
	ref class Form1;

	double getDistance(Point2f p1, Point2f p2);
	RotatedRect detectRectangle(vector<cv::Point> contour);

	bool isLogoContour(vector<cv::Point> contour, cv::Size imageSize);

	bool size_detect(Form1^ form, VideoCapture& cap, double& width, double& height);
}