#include "Form1.h"
#include "Size.h"

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
        // �ΰ� ���͸� ����
        // ���ǿ� �´� ���͸� �ڵ� ����
        return false; // ���� �ΰ� �ƴ� ���
    }

    // size_detect �Լ� ����
    bool size_detect(Form1^ form, VideoCapture& cap , double& width, double& height)
    {
        Mat frame, gray, edges, binary, morph;
        cap >> frame; // ī�޶󿡼� ������ ��������

        if (frame.empty()) {
            form->AppendTextToOutputBox("������ �����ϴ�.");
            return false; // ���� ����
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

        if (!largestContour.empty()) {
            // �޴��� ���̽��� �ּ� ��� �簢�� ����
            RotatedRect rect = detectRectangle(largestContour);

            // �簢���� �׸��� ���� ��ǥ ���
            Point2f vertices[4];
            rect.points(vertices);

            for (int i = 0; i < 4; i++) {
                line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0), 2);
            }

            // ũ��� �߽��� ���
            form->AppendTextToOutputBox("Width: " + rect.size.width.ToString() + " Height: " + rect.size.height.ToString());

            width = rect.size.width;
            height = rect.size.height;

            // ���⿡ �����ϸ� true ��ȯ
            return true;
        }

        // ���� ���� �� false ��ȯ
        return false;
    }

}
