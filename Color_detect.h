#include <opencv2/opencv.hpp>
#include <string>
#include "Form1.h"


namespace Project1 {
    // 평균 색상 값에 따라 색상 이름을 반환하는 함수
    std::string getColorName(const cv::Scalar& mean_color);

    // 비디오 스트림에서 색상을 감지하는 함수
    string color_detect(Form1^ form, cv::VideoCapture& cap);
}

