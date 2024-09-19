#include "color_detect.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

namespace Project1 {
    // 평균 색상 값에 따라 색상을 결정하는 함수 정의
    string getColorName(const Scalar& mean_color) {
        int B = static_cast<int>(mean_color[0]);
        int G = static_cast<int>(mean_color[1]);
        int R = static_cast<int>(mean_color[2]);

        if (B >= 120 && B <= 180 && G < 100 && R < 100) {
            return "Blue";  // 파란색
        }
        else if (G >= 120 && G <= 180 && B < 100 && R < 100) {
            return "Green";  // 초록색
        }
        else if (R >= 150 && R <= 200 && B < 100 && G < 100) {
            return "Red";  // 빨간색
        }
        else if (B >= 220 && G >= 220 && R >= 220) {
            return "White";  // 흰색
        }
        else if (B < 50 && G < 50 && R < 50) {
            return "Black";  // 검은색
        }
        else if (B >= 150 && B <= 180 && G >= 150 && G <= 200 && R >= 150 && R <= 200) {
            return "Yellow";  // 노란색
        }
        else if (B >= 180 && B <= 220 && G < 100 && R >= 150 && R <= 220) {
            return "Magenta";  // 마젠타색
        }
        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 100) {
            return "Cyan";  // 청록색
        }
        else if (B >= 100 && B <= 160 && G < 100 && R >= 100 && R <= 180) {
            return "Purple";  // 보라색
        }
        else if (B >= 180 && B <= 220 && G >= 150 && G <= 200 && R >= 200 && R <= 230) {
            return "Pink";  // 핑크색
        }
        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 200) {
            return "Silver";  // 은색
        }
        else if (B >= 150 && B <= 200 && G >= 140 && G <= 180 && R >= 180 && R <= 220) {
            return "Gold";  // 골드색
        }
        else if (B >= 90 && B <= 130 && G >= 90 && G <= 130 && R >= 90 && R <= 130) {
            return "Gray";  // 회색
        }

        return "Unknown";  // 인식되지 않는 색상
    }

    int hsv_Color() {
        VideoCapture cap(0);
        if (!cap.isOpened()) {
            cerr << "Error: 카메라를 열 수 없습니다." << endl;
            return -1;
        }

        while (true) {
            Mat frame, img_gray, img_blur, img_edges, img_contours;
            bool ret = cap.read(frame);
            if (!ret) {
                cerr << "Error: 이미지를 캡처할 수 없습니다." << endl;
                break;
            }

            // 컬러 이미지를 그레이스케일로 변환
            cvtColor(frame, img_gray, COLOR_BGR2GRAY);
            // 그레이스케일 이미지에 가우시안 블러 적용
            GaussianBlur(img_gray, img_blur, cv::Size(5, 5), 0);
            // 블러 처리된 이미지에서 에지 검출
            Canny(img_blur, img_edges, 50, 150);

            // 검출된 에지에서 윤곽선 찾기
            vector<vector<cv::Point>> contours;
            vector<Vec4i> hierarchy;
            findContours(img_edges, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

            // 원본 컬러 이미지 복사본 생성
            img_contours = frame.clone();

            if (!contours.empty()) {
                // 가장 큰 윤곽선을 찾기 위한 변수 초기화
                size_t largest_contour_index = 0;
                double largest_area = 0;

                for (size_t i = 0; i < contours.size(); i++) {
                    double area = contourArea(contours[i]);
                    if (area > largest_area) {
                        largest_area = area;
                        largest_contour_index = i;
                    }
                }

                // 가장 큰 윤곽선을 마스크로 생성
                Mat mask = Mat::zeros(frame.size(), CV_8UC1);
                drawContours(mask, contours, (int)largest_contour_index, Scalar(255), FILLED);

                // 마스크를 사용하여 관심 영역 추출
                Mat img_roi;
                frame.copyTo(img_roi, mask);

                // 관심 영역의 평균 색상 계산
                Scalar mean_color = mean(img_roi, mask);
                // 평균 색상에 따라 색상 이름 얻기
                string detected_color = getColorName(mean_color);

                // 결과 텍스트 출력 (색상 이름과 BGR 값)
                string result_text = "Color: " + detected_color + " (B: " + to_string(static_cast<int>(mean_color[0])) +
                    ", G: " + to_string(static_cast<int>(mean_color[1])) +
                    ", R: " + to_string(static_cast<int>(mean_color[2])) + ")";
                putText(img_contours, result_text, cv::Point(10, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

                // 가장 큰 윤곽선을 이미지에 그리기
                drawContours(img_contours, contours, (int)largest_contour_index, Scalar(0, 255, 0), 2);
            }

            // 결과 이미지 출력
            imshow("Detected Phone Area", img_contours);

            int key = waitKey(1);
            if (key == 27)  // ESC 키를 누르면 루프 종료
                break;
        }

        cap.release();  // 카메라 자원 해제
        return 0;
    }

}
