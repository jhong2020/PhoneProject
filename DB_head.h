#pragma once

#include <opencv2/opencv.hpp>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <vcclr.h>
#include <thread>
#include <memory>
// #include "Form1.h"

using namespace cv;
using namespace std;
using namespace sql;
using namespace chrono;

//using namespace System;
//using namespace System::ComponentModel;
//using namespace System::Collections;
//using namespace System::Windows::Forms;
//using namespace System::Data;
//using namespace System::Drawing;

namespace Project1 {

    string getColorName(const Scalar& mean_color);
    int hsv_Color();
    ref class Form1;
    int detectCircles(Form1^ form, VideoCapture& cap);

    double getDistance(Point2f p1, Point2f p2);
    RotatedRect detectRectangle(vector<::Point> contour);
    bool isLogoContour(vector<::Point> contour, ::Size imageSize);
    bool size_detect(Form1^ form, VideoCapture& cap, double& width, double& height);
}

// Database functions
int get_INFO(double width, double height, int lensNum, const string& color , const std::vector<uchar>& imageBuffer);
bool Matching_INFO(double width, double height, int lensNum, const string& color, const std::vector<uchar>& imageBuffer);
void Input_And_Match_Info(void);
