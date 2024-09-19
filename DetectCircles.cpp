#include "DetectCircles.h"


 // 시간 관련 함수 사용

namespace Project1 {

    int Project1::detectCircles(Project1::Form1^ form, cv::VideoCapture& cap)
    {
        if (!cap.isOpened()) {
            form->AppendTextToOutputBox("카메라를 열 수 없습니다.");
            return -1;
        }

        Mat img_color;
        Mat img_gray;

        // 마지막 출력 시간을 저장할 변수. 3초마다 출력하는 타이머 용도
        auto last_output_time = steady_clock::now(); // 현재 시간을 steady_clock으로 기록

        while (true) {

            bool ret = cap.read(img_color);
            if (!ret) {
                form->AppendTextToOutputBox("캡처 실패");
                break;
            }
            if (img_color.empty()) {
                form->AppendTextToOutputBox("이미지 파일을 읽을 수 없음");
                return -1;
            }
            cvtColor(img_color, img_gray, COLOR_BGR2GRAY);

            medianBlur(img_gray, img_gray, 3);

            vector<Vec3f> circles;
            HoughCircles(img_gray, circles, HOUGH_GRADIENT, 1, img_gray.rows / 8, 100, 50, 0, 0);

            sort(circles.begin(), circles.end(), [](const Vec3f& a, const Vec3f& b) {
                return a[0] < b[0];
                });

            // 현재 시간 계산
            auto current_time = steady_clock::now(); // 현재 시간을 기록
            auto elapsed_seconds = duration_cast<seconds>(current_time - last_output_time).count();
            // 현재 시간과 마지막 출력 시간 사이의 경과 시간을 초 단위로 계산

            // 5초마다 출력 (elapsed_seconds가 5초 이상일 때 실행)
            if (elapsed_seconds >= 5) {
                form->AppendTextToOutputBox("검출된 렌즈 개수 : " + circles.size().ToString());
                for (size_t i = 0; i < circles.size(); i++) {
                    Vec3f c = circles[i];

                    cv::Point center(cvRound(c[0]), cvRound(c[1]));
                    int radius = cvRound(c[2]);

                    circle(img_color, center, radius, Scalar(0, 255, 0), 2);
                    circle(img_color, center, 2, Scalar(0, 0, 255), 3);

                    // 렌즈의 위치 좌표 (x, y)와 반지름을 출력
                    form->AppendTextToOutputBox("렌즈 " + (i + 1).ToString() + " - 중심 좌표: ("
                        + center.x.ToString() + ", " + center.y.ToString() + "), 반지름: " + radius.ToString());
                }
                // 마지막 출력 시간을 현재 시간으로 업데이트 (5초 주기로 출력되도록 하기 위함)
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
