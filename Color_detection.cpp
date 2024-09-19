#include "color_detect.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

namespace Project1 {
    // ��� ���� ���� ���� ������ �����ϴ� �Լ� ����
    string getColorName(const Scalar& mean_color) {
        int B = static_cast<int>(mean_color[0]);
        int G = static_cast<int>(mean_color[1]);
        int R = static_cast<int>(mean_color[2]);

        if (B >= 120 && B <= 180 && G < 100 && R < 100) {
            return "Blue";  // �Ķ���
        }
        else if (G >= 120 && G <= 180 && B < 100 && R < 100) {
            return "Green";  // �ʷϻ�
        }
        else if (R >= 150 && R <= 200 && B < 100 && G < 100) {
            return "Red";  // ������
        }
        else if (B >= 220 && G >= 220 && R >= 220) {
            return "White";  // ���
        }
        else if (B < 50 && G < 50 && R < 50) {
            return "Black";  // ������
        }
        else if (B >= 150 && B <= 180 && G >= 150 && G <= 200 && R >= 150 && R <= 200) {
            return "Yellow";  // �����
        }
        else if (B >= 180 && B <= 220 && G < 100 && R >= 150 && R <= 220) {
            return "Magenta";  // ����Ÿ��
        }
        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 100) {
            return "Cyan";  // û�ϻ�
        }
        else if (B >= 100 && B <= 160 && G < 100 && R >= 100 && R <= 180) {
            return "Purple";  // �����
        }
        else if (B >= 180 && B <= 220 && G >= 150 && G <= 200 && R >= 200 && R <= 230) {
            return "Pink";  // ��ũ��
        }
        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 200) {
            return "Silver";  // ����
        }
        else if (B >= 150 && B <= 200 && G >= 140 && G <= 180 && R >= 180 && R <= 220) {
            return "Gold";  // ����
        }
        else if (B >= 90 && B <= 130 && G >= 90 && G <= 130 && R >= 90 && R <= 130) {
            return "Gray";  // ȸ��
        }

        return "Unknown";  // �νĵ��� �ʴ� ����
    }

    int hsv_Color() {
        VideoCapture cap(0);
        if (!cap.isOpened()) {
            cerr << "Error: ī�޶� �� �� �����ϴ�." << endl;
            return -1;
        }

        while (true) {
            Mat frame, img_gray, img_blur, img_edges, img_contours;
            bool ret = cap.read(frame);
            if (!ret) {
                cerr << "Error: �̹����� ĸó�� �� �����ϴ�." << endl;
                break;
            }

            // �÷� �̹����� �׷��̽����Ϸ� ��ȯ
            cvtColor(frame, img_gray, COLOR_BGR2GRAY);
            // �׷��̽����� �̹����� ����þ� �� ����
            GaussianBlur(img_gray, img_blur, cv::Size(5, 5), 0);
            // �� ó���� �̹������� ���� ����
            Canny(img_blur, img_edges, 50, 150);

            // ����� �������� ������ ã��
            vector<vector<cv::Point>> contours;
            vector<Vec4i> hierarchy;
            findContours(img_edges, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

            // ���� �÷� �̹��� ���纻 ����
            img_contours = frame.clone();

            if (!contours.empty()) {
                // ���� ū �������� ã�� ���� ���� �ʱ�ȭ
                size_t largest_contour_index = 0;
                double largest_area = 0;

                for (size_t i = 0; i < contours.size(); i++) {
                    double area = contourArea(contours[i]);
                    if (area > largest_area) {
                        largest_area = area;
                        largest_contour_index = i;
                    }
                }

                // ���� ū �������� ����ũ�� ����
                Mat mask = Mat::zeros(frame.size(), CV_8UC1);
                drawContours(mask, contours, (int)largest_contour_index, Scalar(255), FILLED);

                // ����ũ�� ����Ͽ� ���� ���� ����
                Mat img_roi;
                frame.copyTo(img_roi, mask);

                // ���� ������ ��� ���� ���
                Scalar mean_color = mean(img_roi, mask);
                // ��� ���� ���� ���� �̸� ���
                string detected_color = getColorName(mean_color);

                // ��� �ؽ�Ʈ ��� (���� �̸��� BGR ��)
                string result_text = "Color: " + detected_color + " (B: " + to_string(static_cast<int>(mean_color[0])) +
                    ", G: " + to_string(static_cast<int>(mean_color[1])) +
                    ", R: " + to_string(static_cast<int>(mean_color[2])) + ")";
                putText(img_contours, result_text, cv::Point(10, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

                // ���� ū �������� �̹����� �׸���
                drawContours(img_contours, contours, (int)largest_contour_index, Scalar(0, 255, 0), 2);
            }

            // ��� �̹��� ���
            imshow("Detected Phone Area", img_contours);

            int key = waitKey(1);
            if (key == 27)  // ESC Ű�� ������ ���� ����
                break;
        }

        cap.release();  // ī�޶� �ڿ� ����
        return 0;
    }

}
