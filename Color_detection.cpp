#include <opencv2/opencv.hpp>
#include <iostream>
#include <mysql.h>
#include "Color_detect.h"
#include "Form1.h"

using namespace cv;
using namespace std;

namespace Project1 {

    // �Լ�: ��� ���� ���� ���� ������ �����ϴ� �Լ�
    // �־��� ����(Scalar ����)�� ��� ���� ���� �м��Ͽ� �ش� ������ �̸��� ��ȯ�մϴ�.
    // ��� ���� ���� B, G, R ä�η� ������ ������ �����մϴ�.
    string getColorName(const Scalar& mean_color) {
        int B = static_cast<int>(mean_color[0]); // ��� ������ B ä�� ��
        int G = static_cast<int>(mean_color[1]); // ��� ������ G ä�� ��
        int R = static_cast<int>(mean_color[2]); // ��� ������ R ä�� ��

        // ���� ���� ������ �����Ͽ� ���� �̸��� ��ȯ
        if (B > G + 20 && B > R + 20) {
            return "Blue";  // �Ķ���
        }
        else if (G > B + 20 && G > R + 20) {
            return "Green";  // �ʷϻ�
        }
        else if (R > B + 20 && R > G + 20) {
            return "Red";  // ������
        }
        else if (B > 200 && G > 200 && R > 200) {
            return "White";  // ���
        }
        else if (B < 80 && G < 80 && R < 80) {
            return "Black";  // ������
        }
        else if (B > 150 && G > 150 && R < 150) {
            return "Cyan";  // û�ϻ�
        }
        else if (R > 150 && G > 150 && B < 100) {
            return "Yellow";  // �����
        }
        else if (R > 100 && B > 100 && G < 100) {
            return "Purple";  // �����
        }
        else if (B > 150 && G > 150 && R > 150 && (abs(B - G) < 30) && (abs(G - R) < 30)) {
            return "Gray";  // ȸ��
        }
        else if (R > 130 && G > 110 && B < 110) {
            return "Orange";  // ��Ȳ��
        }
        else if (B > 150 && R > 150 && G > 150) {
            return "Silver";  // ����
        }
        else if (R > 170 && G > 150 && B > 100) {
            return "Pink";  // ��ũ��
        }
        else {
            return "Gray";  // �⺻������ ȸ������ ó��
        }
    }

    // �Լ�: ���� ���� �� ���� �м�
    // ��ķ���� �������� ĸó�ϰ�, �ش� �����ӿ��� Ư�� ������ �����Ͽ� ��� ������ ����մϴ�.
    // ��� ���� ���� �̿��� ������ �Ǻ��ϰ�, �� ����� �ֿܼ� ����մϴ�.
    string color_detect(Form1^ form, VideoCapture& cap) {
        if (!cap.isOpened()) {
            cerr << "Error: Could not open camera" << endl;
            return ""; // ���� ���� �� �� ���ڿ� ��ȯ
        }

        Mat frame, img_gray, img_blur, img_edges;
        bool ret = cap.read(frame); // ��ķ���� ������ ĸó
        if (!ret) {
            cerr << "Error: Failed to capture image" << endl;
            return ""; // ������ ĸó ���� �� �� ���ڿ� ��ȯ
        }

        // �÷� �̹����� �׷��̽����Ϸ� ��ȯ �� ��/���� ó��
        cvtColor(frame, img_gray, COLOR_BGR2GRAY);
        GaussianBlur(img_gray, img_blur, cv::Size(5, 5), 0);
        Canny(img_blur, img_edges, 65, 200);

        vector<cv::Point> edge_points; // ���� �ȼ� ��ǥ ����
        for (int y = 0; y < img_edges.rows; y++) {
            for (int x = 0; x < img_edges.cols; x++) {
                if (img_edges.at<uchar>(y, x) > 0) {
                    edge_points.push_back(cv::Point(x, y));
                }
            }
        }

        if (!edge_points.empty()) {
            Rect largest_rect = boundingRect(edge_points);
            if (largest_rect.area() > 1000) { // ������ 1000 �̻��� ���� ó��
                Mat img_roi = frame(largest_rect); // �簢�� ���� �̹��� ����
                Scalar mean_color = mean(img_roi); // ��� ���� ���

                // ��� ���� ���� ���� �̸� �Ǻ�
                string detected_color = getColorName(mean_color);

                // ���� ���� �� �޽��� ���
                cout << "�÷� ���� ����: " << detected_color << endl;
                form->AppendTextToOutputBox("�÷� ���� ����: " + gcnew System::String(detected_color.c_str()));

                return detected_color; // ���� �̸� ��ȯ
            }
        }

        return ""; // ���� ���� �� �� ���ڿ� ��ȯ
    }

}