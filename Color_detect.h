#include <opencv2/opencv.hpp>
#include <string>
#include "Form1.h"


namespace Project1 {
    // ��� ���� ���� ���� ���� �̸��� ��ȯ�ϴ� �Լ�
    std::string getColorName(const cv::Scalar& mean_color);

    // ���� ��Ʈ������ ������ �����ϴ� �Լ�
    string color_detect(Form1^ form, cv::VideoCapture& cap);
}

