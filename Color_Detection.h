#pragma once

#ifndef COLOR_DETECTION_H
#define COLOR_DETECTION_H

#include <opencv2/opencv.hpp>
#include <string>

// 색상 이름을 결정하는 함수 선언
std::string getColorName(const cv::Scalar& mean_color);

#endif // COLOR_DETECTION_H