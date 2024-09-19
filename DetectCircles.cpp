#include "DetectCircles.h"


 // �ð� ���� �Լ� ���

namespace Project1 {

    int Project1::detectCircles(Project1::Form1^ form, cv::VideoCapture& cap)
    {
        if (!cap.isOpened()) {
            form->AppendTextToOutputBox("ī�޶� �� �� �����ϴ�.");
            return -1;
        }

        Mat img_color;
        Mat img_gray;

        // ������ ��� �ð��� ������ ����. 3�ʸ��� ����ϴ� Ÿ�̸� �뵵
        auto last_output_time = steady_clock::now(); // ���� �ð��� steady_clock���� ���

        while (true) {

            bool ret = cap.read(img_color);
            if (!ret) {
                form->AppendTextToOutputBox("ĸó ����");
                break;
            }
            if (img_color.empty()) {
                form->AppendTextToOutputBox("�̹��� ������ ���� �� ����");
                return -1;
            }
            cvtColor(img_color, img_gray, COLOR_BGR2GRAY);

            medianBlur(img_gray, img_gray, 3);

            vector<Vec3f> circles;
            HoughCircles(img_gray, circles, HOUGH_GRADIENT, 1, img_gray.rows / 8, 100, 50, 0, 0);

            sort(circles.begin(), circles.end(), [](const Vec3f& a, const Vec3f& b) {
                return a[0] < b[0];
                });

            // ���� �ð� ���
            auto current_time = steady_clock::now(); // ���� �ð��� ���
            auto elapsed_seconds = duration_cast<seconds>(current_time - last_output_time).count();
            // ���� �ð��� ������ ��� �ð� ������ ��� �ð��� �� ������ ���

            // 5�ʸ��� ��� (elapsed_seconds�� 5�� �̻��� �� ����)
            if (elapsed_seconds >= 5) {
                form->AppendTextToOutputBox("����� ���� ���� : " + circles.size().ToString());
                for (size_t i = 0; i < circles.size(); i++) {
                    Vec3f c = circles[i];

                    cv::Point center(cvRound(c[0]), cvRound(c[1]));
                    int radius = cvRound(c[2]);

                    circle(img_color, center, radius, Scalar(0, 255, 0), 2);
                    circle(img_color, center, 2, Scalar(0, 0, 255), 3);

                    // ������ ��ġ ��ǥ (x, y)�� �������� ���
                    form->AppendTextToOutputBox("���� " + (i + 1).ToString() + " - �߽� ��ǥ: ("
                        + center.x.ToString() + ", " + center.y.ToString() + "), ������: " + radius.ToString());
                }
                // ������ ��� �ð��� ���� �ð����� ������Ʈ (5�� �ֱ�� ��µǵ��� �ϱ� ����)
                last_output_time = current_time;
            }
            imshow("detected circles", img_color);
            if (waitKey(1) == 27)
                break;
        }

        destroyAllWindows();

        return 0;
    }
}
