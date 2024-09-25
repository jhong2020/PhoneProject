#include "DB_head.h"
#include "hough_circle.h"
#include "Form1.h"
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace Project1 {
    bool detected = false;   // 크기를 이미 검출했는지 확인하는 플래그
    bool outputShown = false;  // 결과값이 한 번 출력되었는지 확인하는 플래그

    bool isLens(const Mat& img, const Vec3f& detectedCircle) {
        cv::Point center(cvRound(detectedCircle[0]), cvRound(detectedCircle[1])); // 원의 중심 좌표
        int radius = cvRound(detectedCircle[2]); // 원의 반지름

        // 중심부의 반지름을 원 반지름의 50%로 설정
        int inner_radius = radius * 0.5;

        // 원본 이미지와 동일한 크기의 단일 채널 마스크 생성 (CV_8UC1)
        Mat mask = Mat::zeros(img.rows, img.cols, CV_8UC1);

        // 중심부 마스크 생성 (마스크에 흰색으로 원을 그리기)
        circle(mask, center, inner_radius, Scalar(255), FILLED);

        // 중심부의 색상 평균 계산
        Scalar meanColor = mean(img, mask); // 중심부의 평균 색상 계산

        // 중심부가 어두운지 확인 (어두운 색이면 렌즈로 간주)
        return meanColor[0] < 100 && meanColor[1] < 100 && meanColor[2] < 100;
    }

    bool hough_circle(Form1^ form, VideoCapture& cap, int& lensNum) {

        if (!cap.isOpened()) {
            cout << "카메라를 열 수 없습니다." << endl;
            return false;
        }

        Mat img_color;  // 원본 컬러 이미지
        Mat img_gray;   // 그레이스케일 이미지
        auto start_time = steady_clock::now();

        // 5초 동안 원을 계속 검출하고 화면에 그리기
        while (true) {
            // 경과 시간 계산
            auto current_time = steady_clock::now();
            auto elapsed_seconds = duration_cast<seconds>(current_time - start_time).count();

            // 5초가 지나면 종료
            if (elapsed_seconds >= 5) {
                break;
            }

            bool ret = cap.read(img_color); // 카메라로부터 이미지를 읽음
            if (!ret || img_color.empty()) {
                cout << "캡쳐 실패" << endl;
                return false;
            }

            // 컬러 이미지를 그레이스케일 이미지로 변환
            cvtColor(img_color, img_gray, COLOR_BGR2GRAY);

            // 이미지에 Gaussian 블러를 적용하여 노이즈 감소
            GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);

            vector<Vec3f> circles;

            // Hough Circle Transform을 사용하여 원을 검출
            HoughCircles(img_gray, circles, HOUGH_GRADIENT, 1, img_gray.rows / 16, 110, 30, 5, 100);

            // 검출된 원을 필터링하여 렌즈만 선택
            vector<Vec3f> filtered_circles;
            for (const auto& c : circles) {
                int radius = cvRound(c[2]); // 원의 반지름
                if (radius > 10 && radius < 50 && isLens(img_color, c)) { // 반지름과 색상을 기준으로 필터링
                    filtered_circles.push_back(c); // 필터링된 원을 벡터에 추가
                }
            }

            // 렌즈 개수 저장
            lensNum = static_cast<int>(filtered_circles.size());

            // 이미지에 원을 계속 그리기
            for (const auto& c : filtered_circles) {
                cv::Point center(cvRound(c[0]), cvRound(c[1])); // 원의 중심 좌표
                int radius = cvRound(c[2]); // 원의 반지름

                // 원의 경계를 초록색으로 그리기
                circle(img_color, center, radius, Scalar(0, 255, 0), 2);
                // 원의 중심을 빨간색으로 그리기
                circle(img_color, center, 2, Scalar(0, 0, 255), 3);
            }

            // 결과값이 한 번만 출력되도록 설정
            if (!outputShown) {
                form->AppendTextToOutputBox("lensNum " + System::Convert::ToString(lensNum));
                outputShown = true;  // 결과 출력 후 다시 출력되지 않도록 설정
            }

            // 결과 화면 출력
            imshow("Detection", img_color);

            // ESC 키로 종료
            if (waitKey(1) == 27) {
                break;
            }
        }

        return true;
    }
}
