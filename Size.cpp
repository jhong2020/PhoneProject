// Size.cpp

#include "Form1.h"
#include "Size.h"
#include <opencv2/opencv.hpp>

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
        double area = contourArea(contour);
        if (area < 4000)  // 작은 면적은 로고로 간주
            return true;

        RotatedRect rect = minAreaRect(contour);
        double aspectRatio = max(rect.size.width, rect.size.height) / min(rect.size.width, rect.size.height);
        if (aspectRatio > 4.0)  // 가로 세로 비율이 지나치게 다른 경우 제거
            return true;

        Point2f center = rect.center;
        double centerX = imageSize.width / 2.0;
        double centerY = imageSize.height / 2.0;
        double distanceFromCenter = getDistance(center, Point2f(centerX, centerY));

        if (distanceFromCenter < imageSize.width * 0.2 && area < imageSize.width * imageSize.height * 0.1) {
            return true;   // 중앙에서 너무 가까운 작은 물체는 로고로 간주
        }

        return false;  // 로고가 아닌 경우
    }


    // size_detect 함수 수정
    bool size_detect(Form1^ form, VideoCapture& cap, double& width, double& height) {

        int quit = 0;
        static auto start_time = steady_clock::now();

        double ratio = 0;  // 가로/세로 비율을 저장할 변수
        bool detected = false;   // 크기를 이미 검출했는지 확인하는 플래그
        bool size_output_done = false;   // 크기 정보를 출력했는지 확인하는 플래그
        Point2f vertices[4];  // 사각형의 좌표를 저장할 배열

        while (true)
        {
            // 현재 시간 측정
            auto now = steady_clock::now();
            auto elapsed_seconds = duration_cast<seconds>(now - start_time).count();

            Mat frame, gray, edges, binary, morph;
            cap >> frame;   // 카메라에서 프레임 가져오기

            if (frame.empty()) {
                form->AppendTextToOutputBox("영상이 없습니다.");
                return false;   // 검출 실패
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

            // 5초 후부터 width와 height를 받아들이고 초록색 네모를 출력
            if (elapsed_seconds >= 5 && !detected) { // 아직 크기를 검출하지 않았다면

                // 윤곽선 검출
                double maxArea = 0;
                vector<cv::Point> largestContour;
                for (size_t i = 0; i < contours.size(); i++) {
                    if (isLogoContour(contours[i], frame.size())) {
                        continue;  // 로고로 간주된 윤곽선은 무시
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

                    // 가로, 세로 길이 계산
                    width = round(rect.size.width * 100) / 100;  //★☆★
                    height = round(rect.size.height * 100) / 100;  //★☆★

                    // 가로, 세로 바뀌는 경우 대비
                    if (width < height) {
                        swap(width, height);
                    }

                    // 사각형 좌표 계산
                    rect.points(vertices);

                    // 크기를 한 번 검출한 후 다시 검출하지 않도록 설정
                    detected = true;

                }
            }

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

            if (width > 250 && height > 150) {

                // 초록색 네모를 그리기
                for (int i = 0; i < 4; i++) {
                    line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0), 2);
                }

                // 가로, 세로 콘솔 출력 (한 번만 출력)
                if (!size_output_done) {
                    ratio = round((width / height) * 100) / 100;
                    cout << "Width: " << width << ", Height: " << height << endl << "Ratio(Width/Height): " << ratio << endl;
                    size_output_done = true;  // 이 부분에서 플래그를 true로 설정
                    cout << size_output_done << endl;  // 플래그 값 출력 (디버그용)
                }

                // 검출된 크기를 화면에 텍스트로 표시
                Point2f text_loc(15, 30);
                putText(frame, format("W: %.2f px, H: %.2f px", width, height), text_loc, FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 2);
            }

            // 결과 화면 출력
            imshow("Detection", frame);

            // 10초 후 꺼짐
            if (elapsed_seconds >= 10) {
                static bool size_unsuitable = false; // 적절하지 않은 물체 판별하는 플래그
                form->AppendTextToOutputBox("width " + System::Convert::ToString(width) + "height " + System::Convert::ToString(height));

                if (width < 250 || height < 150) {
                    cout << "검출 대상이 아닙니다.\n" << endl;
                    size_unsuitable = true;
                    if (size_unsuitable == true)
                        return false;
                }
                break;
            }


            // 'Esc' 키를 누르면 종료
            if (waitKey(1) == 27) {
                cout << "프로그램을 종료합니다.\n" << endl;
                quit = -1;
                break;
            }
        }

        if (width < 250 || height < 150 || quit == -1)
            return false;


        return true;
    }

}
