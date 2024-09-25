#include <opencv2/opencv.hpp>
#include <iostream>
#include <mysql.h>
#include "Color_detect.h"
#include "Form1.h"

using namespace cv;
using namespace std;

namespace Project1 {

    // 함수: 평균 색상 값에 따라 색상을 결정하는 함수
    // 주어진 색상(Scalar 형식)의 평균 색상 값을 분석하여 해당 색상의 이름을 반환합니다.
    // 평균 색상 값을 B, G, R 채널로 나누어 색상을 결정합니다.
    string getColorName(const Scalar& mean_color) {
        int B = static_cast<int>(mean_color[0]); // 평균 색상의 B 채널 값
        int G = static_cast<int>(mean_color[1]); // 평균 색상의 G 채널 값
        int R = static_cast<int>(mean_color[2]); // 평균 색상의 R 채널 값

        // 색상 결정 조건을 설정하여 색상 이름을 반환
        if (B > G + 20 && B > R + 20) {
            return "Blue";  // 파란색
        }
        else if (G > B + 20 && G > R + 20) {
            return "Green";  // 초록색
        }
        else if (R > B + 20 && R > G + 20) {
            return "Red";  // 빨간색
        }
        else if (B > 200 && G > 200 && R > 200) {
            return "White";  // 흰색
        }
        else if (B < 80 && G < 80 && R < 80) {
            return "Black";  // 검은색
        }
        else if (B > 150 && G > 150 && R < 150) {
            return "Cyan";  // 청록색
        }
        else if (R > 150 && G > 150 && B < 100) {
            return "Yellow";  // 노란색
        }
        else if (R > 100 && B > 100 && G < 100) {
            return "Purple";  // 보라색
        }
        else if (B > 150 && G > 150 && R > 150 && (abs(B - G) < 30) && (abs(G - R) < 30)) {
            return "Gray";  // 회색
        }
        else if (R > 130 && G > 110 && B < 110) {
            return "Orange";  // 주황색
        }
        else if (B > 150 && R > 150 && G > 150) {
            return "Silver";  // 은색
        }
        else if (R > 170 && G > 150 && B > 100) {
            return "Pink";  // 핑크색
        }
        else {
            return "Gray";  // 기본적으로 회색으로 처리
        }
    }

    // 함수: 영역 감지 및 색상 분석
    // 웹캠에서 프레임을 캡처하고, 해당 프레임에서 특정 영역을 검출하여 평균 색상을 계산합니다.
    // 평균 색상 값을 이용해 색상을 판별하고, 그 결과를 콘솔에 출력합니다.
    string color_detect(Form1^ form, VideoCapture& cap) {
        if (!cap.isOpened()) {
            cerr << "Error: Could not open camera" << endl;
            return ""; // 검출 실패 시 빈 문자열 반환
        }

        Mat frame, img_gray, img_blur, img_edges;
        bool ret = cap.read(frame); // 웹캠에서 프레임 캡처
        if (!ret) {
            cerr << "Error: Failed to capture image" << endl;
            return ""; // 프레임 캡처 실패 시 빈 문자열 반환
        }

        // 컬러 이미지를 그레이스케일로 변환 및 블러/에지 처리
        cvtColor(frame, img_gray, COLOR_BGR2GRAY);
        GaussianBlur(img_gray, img_blur, cv::Size(5, 5), 0);
        Canny(img_blur, img_edges, 65, 200);

        vector<cv::Point> edge_points; // 에지 픽셀 좌표 저장
        for (int y = 0; y < img_edges.rows; y++) {
            for (int x = 0; x < img_edges.cols; x++) {
                if (img_edges.at<uchar>(y, x) > 0) {
                    edge_points.push_back(cv::Point(x, y));
                }
            }
        }

        if (!edge_points.empty()) {
            Rect largest_rect = boundingRect(edge_points);
            if (largest_rect.area() > 1000) { // 면적이 1000 이상일 때만 처리
                Mat img_roi = frame(largest_rect); // 사각형 영역 이미지 추출
                Scalar mean_color = mean(img_roi); // 평균 색상 계산

                // 평균 색상에 따른 색상 이름 판별
                string detected_color = getColorName(mean_color);

                // 검출 성공 시 메시지 출력
                cout << "컬러 검출 성공: " << detected_color << endl;
                form->AppendTextToOutputBox("컬러 검출 성공: " + gcnew System::String(detected_color.c_str()));

                return detected_color; // 색상 이름 반환
            }
        }

        return ""; // 검출 실패 시 빈 문자열 반환
    }

}