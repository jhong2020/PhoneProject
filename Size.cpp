#include "Form1.h"
#include "Size.h"

namespace Project1 {

    // 두 점 사이의 거리 계산 함수
    double getDistance(Point2f p1, Point2f p2) {
        return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    }

    // 윤곽선에서 사각형 검출
    RotatedRect detectRectangle(vector<cv::Point> contour) {
        return minAreaRect(contour);  // 필렛을 포함한 최소 영역의 사각형을 찾습니다.
    }

    // 윤곽선 필터링 (로고 검출 방지)
    bool isLogoContour(vector<cv::Point> contour, cv::Size imageSize) {
        // 로고 필터링 로직
        // 조건에 맞는 필터링 코드 유지
        return false; // 실제 로고가 아닌 경우
    }

    // size_detect 함수 수정
    bool size_detect(Form1^ form, VideoCapture& cap , double& width, double& height)
    {
        Mat frame, gray, edges, binary, morph;
        cap >> frame; // 카메라에서 프레임 가져오기

        if (frame.empty()) {
            form->AppendTextToOutputBox("영상이 없습니다.");
            return false; // 검출 실패
        }

        // 그레이스케일 변환
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // 이진화
        threshold(gray, binary, 128, 255, THRESH_BINARY_INV);

        // 모폴로지 연산 (노이즈 제거)
        Mat element = getStructuringElement(MORPH_RECT, cv::Size(5, 5));
        morphologyEx(binary, morph, MORPH_CLOSE, element);
        morphologyEx(morph, morph, MORPH_OPEN, element);

        // Canny 엣지 검출
        Canny(morph, edges, 50, 230);

        // 윤곽선 검출
        vector<vector<cv::Point>> contours;
        findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // 가장 큰 윤곽선 검출 (휴대폰 케이스로 가정)
        double maxArea = 0;
        vector<cv::Point> largestContour;
        for (size_t i = 0; i < contours.size(); i++) {
            if (isLogoContour(contours[i], frame.size())) {
                continue; // 로고로 간주된 윤곽선은 무시
            }

            double area = contourArea(contours[i]);
            if (area > maxArea) {
                maxArea = area;
                largestContour = contours[i];
            }
        }

        if (!largestContour.empty()) {
            // 휴대폰 케이스의 최소 경계 사각형 검출
            RotatedRect rect = detectRectangle(largestContour);

            // 사각형을 그리기 위한 좌표 계산
            Point2f vertices[4];
            rect.points(vertices);

            for (int i = 0; i < 4; i++) {
                line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0), 2);
            }

            // 크기와 중심점 출력
            form->AppendTextToOutputBox("Width: " + rect.size.width.ToString() + " Height: " + rect.size.height.ToString());

            width = rect.size.width;
            height = rect.size.height;

            // 검출에 성공하면 true 반환
            return true;
        }

        // 검출 실패 시 false 반환
        return false;
    }

}
