#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <algorithm>
#include "Form1.h"


using namespace cv;
using namespace std;
using namespace chrono;

namespace Project1 {
    ref class Form1;
    int detectCircles(Project1::Form1^ form, cv::VideoCapture& cap);  // �Լ� ������Ÿ���� ��Ȯ�� ���
}