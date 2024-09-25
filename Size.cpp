// Size.cpp

#include "Form1.h"
#include "Size.h"
#include <opencv2/opencv.hpp>

namespace Project1 {

    // �� �� ������ �Ÿ� ��� �Լ�
    double getDistance(Point2f p1, Point2f p2) {
        return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    }

    // ���������� �簢�� ����
    RotatedRect detectRectangle(vector<cv::Point> contour) {
        return minAreaRect(contour);  // �ʷ��� ������ �ּ� ������ �簢���� ã���ϴ�.
    }

    // ������ ���͸� (�ΰ� ���� ����)
    bool isLogoContour(vector<cv::Point> contour, cv::Size imageSize) {
        double area = contourArea(contour);
        if (area < 4000)  // ���� ������ �ΰ�� ����
            return true;

        RotatedRect rect = minAreaRect(contour);
        double aspectRatio = max(rect.size.width, rect.size.height) / min(rect.size.width, rect.size.height);
        if (aspectRatio > 4.0)  // ���� ���� ������ ����ġ�� �ٸ� ��� ����
            return true;

        Point2f center = rect.center;
        double centerX = imageSize.width / 2.0;
        double centerY = imageSize.height / 2.0;
        double distanceFromCenter = getDistance(center, Point2f(centerX, centerY));

        if (distanceFromCenter < imageSize.width * 0.2 && area < imageSize.width * imageSize.height * 0.1) {
            return true;   // �߾ӿ��� �ʹ� ����� ���� ��ü�� �ΰ�� ����
        }

        return false;  // �ΰ� �ƴ� ���
    }


    // size_detect �Լ� ����
    bool size_detect(Form1^ form, VideoCapture& cap, double& width, double& height) {

        int quit = 0;
        static auto start_time = steady_clock::now();

        double ratio = 0;  // ����/���� ������ ������ ����
        bool detected = false;   // ũ�⸦ �̹� �����ߴ��� Ȯ���ϴ� �÷���
        bool size_output_done = false;   // ũ�� ������ ����ߴ��� Ȯ���ϴ� �÷���
        Point2f vertices[4];  // �簢���� ��ǥ�� ������ �迭

        while (true)
        {
            // ���� �ð� ����
            auto now = steady_clock::now();
            auto elapsed_seconds = duration_cast<seconds>(now - start_time).count();

            Mat frame, gray, edges, binary, morph;
            cap >> frame;   // ī�޶󿡼� ������ ��������

            if (frame.empty()) {
                form->AppendTextToOutputBox("������ �����ϴ�.");
                return false;   // ���� ����
            }

            // �׷��̽����� ��ȯ
            cvtColor(frame, gray, COLOR_BGR2GRAY);

            // ����ȭ
            threshold(gray, binary, 128, 255, THRESH_BINARY_INV);

            // �������� ���� (������ ����)
            Mat element = getStructuringElement(MORPH_RECT, cv::Size(5, 5));
            morphologyEx(binary, morph, MORPH_CLOSE, element);
            morphologyEx(morph, morph, MORPH_OPEN, element);

            // Canny ���� ����
            Canny(morph, edges, 50, 230);

            // ������ ����
            vector<vector<cv::Point>> contours;
            findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

            // 5�� �ĺ��� width�� height�� �޾Ƶ��̰� �ʷϻ� �׸� ���
            if (elapsed_seconds >= 5 && !detected) { // ���� ũ�⸦ �������� �ʾҴٸ�

                // ������ ����
                double maxArea = 0;
                vector<cv::Point> largestContour;
                for (size_t i = 0; i < contours.size(); i++) {
                    if (isLogoContour(contours[i], frame.size())) {
                        continue;  // �ΰ�� ���ֵ� �������� ����
                    }

                    double area = contourArea(contours[i]);
                    if (area > maxArea) {
                        maxArea = area;
                        largestContour = contours[i];
                    }
                }

                if (!largestContour.empty()) {

                    // �޴��� ���̽��� �ּ� ��� �簢�� ����
                    RotatedRect rect = detectRectangle(largestContour);

                    // ����, ���� ���� ���
                    width = round(rect.size.width * 100) / 100;  //�ڡ١�
                    height = round(rect.size.height * 100) / 100;  //�ڡ١�

                    // ����, ���� �ٲ�� ��� ���
                    if (width < height) {
                        swap(width, height);
                    }

                    // �簢�� ��ǥ ���
                    rect.points(vertices);

                    // ũ�⸦ �� �� ������ �� �ٽ� �������� �ʵ��� ����
                    detected = true;

                }
            }

            // ���� ū ������ ���� (�޴��� ���̽��� ����)
            double maxArea = 0;
            vector<cv::Point> largestContour;
            for (size_t i = 0; i < contours.size(); i++) {
                if (isLogoContour(contours[i], frame.size())) {
                    continue; // �ΰ�� ���ֵ� �������� ����
                }

                double area = contourArea(contours[i]);
                if (area > maxArea) {
                    maxArea = area;
                    largestContour = contours[i];
                }
            }

            if (width > 250 && height > 150) {

                // �ʷϻ� �׸� �׸���
                for (int i = 0; i < 4; i++) {
                    line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0), 2);
                }

                // ����, ���� �ܼ� ��� (�� ���� ���)
                if (!size_output_done) {
                    ratio = round((width / height) * 100) / 100;
                    cout << "Width: " << width << ", Height: " << height << endl << "Ratio(Width/Height): " << ratio << endl;
                    size_output_done = true;  // �� �κп��� �÷��׸� true�� ����
                    cout << size_output_done << endl;  // �÷��� �� ��� (����׿�)
                }

                // ����� ũ�⸦ ȭ�鿡 �ؽ�Ʈ�� ǥ��
                Point2f text_loc(15, 30);
                putText(frame, format("W: %.2f px, H: %.2f px", width, height), text_loc, FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 2);
            }

            // ��� ȭ�� ���
            imshow("Detection", frame);

            // 10�� �� ����
            if (elapsed_seconds >= 10) {
                static bool size_unsuitable = false; // �������� ���� ��ü �Ǻ��ϴ� �÷���
                form->AppendTextToOutputBox("width " + System::Convert::ToString(width) + "height " + System::Convert::ToString(height));

                if (width < 250 || height < 150) {
                    cout << "���� ����� �ƴմϴ�.\n" << endl;
                    size_unsuitable = true;
                    if (size_unsuitable == true)
                        return false;
                }
                break;
            }


            // 'Esc' Ű�� ������ ����
            if (waitKey(1) == 27) {
                cout << "���α׷��� �����մϴ�.\n" << endl;
                quit = -1;
                break;
            }
        }

        if (width < 250 || height < 150 || quit == -1)
            return false;


        return true;
    }

}
