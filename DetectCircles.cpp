#include "DB_head.h"
#include "hough_circle.h"
#include "Form1.h"
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace Project1 {
    bool detected = false;   // ũ�⸦ �̹� �����ߴ��� Ȯ���ϴ� �÷���
    bool outputShown = false;  // ������� �� �� ��µǾ����� Ȯ���ϴ� �÷���

    bool isLens(const Mat& img, const Vec3f& detectedCircle) {
        cv::Point center(cvRound(detectedCircle[0]), cvRound(detectedCircle[1])); // ���� �߽� ��ǥ
        int radius = cvRound(detectedCircle[2]); // ���� ������

        // �߽ɺ��� �������� �� �������� 50%�� ����
        int inner_radius = radius * 0.5;

        // ���� �̹����� ������ ũ���� ���� ä�� ����ũ ���� (CV_8UC1)
        Mat mask = Mat::zeros(img.rows, img.cols, CV_8UC1);

        // �߽ɺ� ����ũ ���� (����ũ�� ������� ���� �׸���)
        circle(mask, center, inner_radius, Scalar(255), FILLED);

        // �߽ɺ��� ���� ��� ���
        Scalar meanColor = mean(img, mask); // �߽ɺ��� ��� ���� ���

        // �߽ɺΰ� ��ο��� Ȯ�� (��ο� ���̸� ����� ����)
        return meanColor[0] < 100 && meanColor[1] < 100 && meanColor[2] < 100;
    }

    bool hough_circle(Form1^ form, VideoCapture& cap, int& lensNum) {

        if (!cap.isOpened()) {
            cout << "ī�޶� �� �� �����ϴ�." << endl;
            return false;
        }

        Mat img_color;  // ���� �÷� �̹���
        Mat img_gray;   // �׷��̽����� �̹���
        auto start_time = steady_clock::now();

        // 5�� ���� ���� ��� �����ϰ� ȭ�鿡 �׸���
        while (true) {
            // ��� �ð� ���
            auto current_time = steady_clock::now();
            auto elapsed_seconds = duration_cast<seconds>(current_time - start_time).count();

            // 5�ʰ� ������ ����
            if (elapsed_seconds >= 5) {
                break;
            }

            bool ret = cap.read(img_color); // ī�޶�κ��� �̹����� ����
            if (!ret || img_color.empty()) {
                cout << "ĸ�� ����" << endl;
                return false;
            }

            // �÷� �̹����� �׷��̽����� �̹����� ��ȯ
            cvtColor(img_color, img_gray, COLOR_BGR2GRAY);

            // �̹����� Gaussian ���� �����Ͽ� ������ ����
            GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);

            vector<Vec3f> circles;

            // Hough Circle Transform�� ����Ͽ� ���� ����
            HoughCircles(img_gray, circles, HOUGH_GRADIENT, 1, img_gray.rows / 16, 110, 30, 5, 100);

            // ����� ���� ���͸��Ͽ� ��� ����
            vector<Vec3f> filtered_circles;
            for (const auto& c : circles) {
                int radius = cvRound(c[2]); // ���� ������
                if (radius > 10 && radius < 50 && isLens(img_color, c)) { // �������� ������ �������� ���͸�
                    filtered_circles.push_back(c); // ���͸��� ���� ���Ϳ� �߰�
                }
            }

            // ���� ���� ����
            lensNum = static_cast<int>(filtered_circles.size());

            // �̹����� ���� ��� �׸���
            for (const auto& c : filtered_circles) {
                cv::Point center(cvRound(c[0]), cvRound(c[1])); // ���� �߽� ��ǥ
                int radius = cvRound(c[2]); // ���� ������

                // ���� ��踦 �ʷϻ����� �׸���
                circle(img_color, center, radius, Scalar(0, 255, 0), 2);
                // ���� �߽��� ���������� �׸���
                circle(img_color, center, 2, Scalar(0, 0, 255), 3);
            }

            // ������� �� ���� ��µǵ��� ����
            if (!outputShown) {
                form->AppendTextToOutputBox("lensNum " + System::Convert::ToString(lensNum));
                outputShown = true;  // ��� ��� �� �ٽ� ��µ��� �ʵ��� ����
            }

            // ��� ȭ�� ���
            imshow("Detection", img_color);

            // ESC Ű�� ����
            if (waitKey(1) == 27) {
                break;
            }
        }

        return true;
    }
}
