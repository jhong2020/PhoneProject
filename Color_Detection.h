#pragma once

#ifndef COLOR_DETECTION_H
#define COLOR_DETECTION_H

#include <opencv2/opencv.hpp>
#include <string>

// ���� �̸��� �����ϴ� �Լ� ����
std::string getColorName(const cv::Scalar& mean_color);

#endif // COLOR_DETECTION_H