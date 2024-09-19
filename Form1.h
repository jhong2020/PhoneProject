#pragma once
#define WIN32_LEAN_AND_MEAN
#include "DB_head.h"
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>
#include <vector>
#include "Size.h"
#include <vcclr.h> // gcroot를 사용하기위해 포함
#include "VideoCaptureWrapper.h"
#include "DetectCircles.h"
#include "Color_detect.h"



namespace Project1 {

    using namespace System;
   
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    using std::string;
    using std::vector;
    using namespace chrono;



    public ref class Form1 : public System::Windows::Forms::Form
    {
    public:
        Form1(void)
        {
            InitializeComponent();
        }

        void AppendTextToOutputBox(System::String^ text)
        {
            if (this->outputBox->InvokeRequired)
            {

                this->outputBox->Invoke(gcnew Action<System::String^>(this, &Form1::AppendTextToOutputBox), text);
            }
            else
            {
                // RichTextBox에 텍스트를 추가합니다.
                this->outputBox->AppendText(text + Environment::NewLine);
            }
        }

    protected:
        ~Form1()
        {
            if (components)
            {
                delete components;
            }
            StopCamera();
        }

    private:
        System::ComponentModel::Container^ components;
        Button^ startButton;
        Button^ stopButton;
        Button^ matchingButton;
        Label^ label1;
        Label^ statusLabel;
        Panel^ statusIndicator;
        RichTextBox^ outputBox;

        VideoCaptureWrapper* cap;
        bool stopCapture = false;
        bool sizeDetected = false;
        bool isCameraRunning = false;
        //MySql::Database* db;
        int lensNum = 0;
        void InitializeComponent(void)
        {
            this->components = gcnew System::ComponentModel::Container();
            this->Text = L"Form1";
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(1000, 600);
            this->BackColor = System::Drawing::Color::White;

            // 실행 시작 버튼
            this->startButton = gcnew System::Windows::Forms::Button();
            this->startButton->Location = System::Drawing::Point(100, 150);  // 버튼 위치 설정
            this->startButton->Size = System::Drawing::Size(100, 50);        // 버튼 크기 설정
            this->startButton->Text = L"영상 촬영시작";

            this->startButton->Click += gcnew System::EventHandler(this, &Form1::startButton_Click);
            this->Controls->Add(this->startButton);

            // 실행 중지 버튼
            this->stopButton = gcnew System::Windows::Forms::Button();
            this->stopButton->Location = System::Drawing::Point(220, 250);   // 버튼 위치 설정
            this->stopButton->Size = System::Drawing::Size(100, 50);         // 버튼 크기 설정
            this->stopButton->Text = L"실행 중지";
            this->stopButton->Click += gcnew System::EventHandler(this, &Form1::stopButton_Click);
            this->Controls->Add(this->stopButton);

            // 매칭 버튼
            this->matchingButton = gcnew System::Windows::Forms::Button();
            this->matchingButton -> Location = System::Drawing::Point(350, 150);
            this->matchingButton->Size = System::Drawing::Size(100, 50);
            this->matchingButton->Text = L"재검출";
            this->matchingButton->Click += gcnew System::EventHandler(this, &Form1::matchingButton_Click);
            this->Controls->Add(this->matchingButton);

            // 라벨1
            this->label1 = gcnew System::Windows::Forms::Label();
            this->label1->Location = System::Drawing::Point(160, 50);  // Label 위치 설정
            this->label1->Size = System::Drawing::Size(300, 30);       // Label 크기 설정
            this->label1->Text = L"핸드폰 케이스 검출 앱";                // Label 텍스트 설정

            this->label1->Font = gcnew System::Drawing::Font(L"Arial", 20); // Arial 폰트, 20pt 크기 설정
            this->Controls->Add(this->label1);

            // 상태 Label 추가
            this->statusLabel = gcnew System::Windows::Forms::Label();
            this->statusLabel->Location = System::Drawing::Point(150, 400);   // Label 위치 설정
            this->statusLabel->Size = System::Drawing::Size(200, 30);       // Label 크기 설정
            this->statusLabel->Font = gcnew System::Drawing::Font(L"Arial", 20);  // 폰트와 크기 설정
            this->statusLabel->Text = L"현재 상태: 준비 중";                // 초기 텍스트 설정
            this->Controls->Add(this->statusLabel);

            // 상태 표시 Panel 추가
            this->statusIndicator = gcnew System::Windows::Forms::Panel();
            this->statusIndicator->Location = System::Drawing::Point(380, 405); // Panel 위치 설정
            this->statusIndicator->Size = System::Drawing::Size(30, 30);      // Panel 크기 설정 (정사각형으로 설정)
            this->statusIndicator->BackColor = System::Drawing::Color::Gray;  // 초기 배경색 설정 (회색)
            this->statusIndicator->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::statusIndicator_Paint);
            this->Controls->Add(this->statusIndicator);

            // RichTextBox 초기화
            this->outputBox = gcnew RichTextBox();
            this->outputBox->Location = System::Drawing::Point(550, 20);  // 위치 설정
            this->outputBox->Size = System::Drawing::Size(430, 550);      // 크기 설정
            this->outputBox->Font = gcnew System::Drawing::Font(L"Arial", 12); // 폰트 설정
            this->outputBox->ReadOnly = true;                            // 읽기 전용 설정
            this->outputBox->BackColor = System::Drawing::Color::FromArgb(102, 153, 204);
            this->Controls->Add(this->outputBox); // 폼에 추가

            cap = new VideoCaptureWrapper();
            //db = new MySql::Database();
        }

        void statusIndicator_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e)
        {
            // 원 그리기
            Graphics^ g = e->Graphics;
            Brush^ brush = gcnew SolidBrush(this->statusIndicator->BackColor);
            g->FillEllipse(brush, 0, 0, this->statusIndicator->Width, this->statusIndicator->Height);
        }

        void startButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            if (!isCameraRunning)
            {
                isCameraRunning = true;
                this->statusLabel->Text = L"현재 상태: 실행 중";  // Label 텍스트 업데이트
                this->statusIndicator->BackColor = System::Drawing::Color::Green;  // 초록색 원으로 변경
                this->statusIndicator->Invalidate(); // Paint 이벤트를 트리거하여 원을 다시 그림

                StartDetectionFlow();


            }
        }

        void stopButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            if (isCameraRunning)
            {
                StopCamera();
            }
        }

        void matchingButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            if (cap == nullptr)
            {
                AppendTextToOutputBox("캡쳐 객체가 초기화되지 않았습니다.");
                cap = new VideoCaptureWrapper();
            }

            if (!isCameraRunning)
            {
                if(!cap->open(0))
                {

                        AppendTextToOutputBox("카메라를 열 수 없습니다.");
                        return;
                }
                isCameraRunning = true;
                this->statusLabel->Text = L"현재 상태: 매칭 중";  // Label 텍스트 업데이트
                this->statusIndicator->BackColor = System::Drawing::Color::FromArgb(255, 165, 0);  // 주황색 원으로 변경
                this->statusIndicator->Invalidate();


                reDetectPhone();


            }
        }


        void StartDetectionFlow()
        {
            if (!cap->open(0))
            {
                AppendTextToOutputBox("카메라를 열 수 없습니다.");
                return;
            }

            DetectedPhone();


        }

        // 이미지를 메모리에 저장하고 주소값을 반환하는 함수
        vector<uchar> CaptureImageToMemory(const Mat& image)
        {
            vector<uchar> buffer;  // 이미지 데이터를 저장할 버퍼
            imencode(".png", image, buffer);  // 이미지를 PNG 형식으로 메모리에 저장
            return buffer;
        }

        


        void DetectedPhone()
        {
            
            Mat img_color, img_gray, img_blur, img_edges, img_contours, img_roi;
            vector<vector<cv::Point>> contours;
            vector<Vec4i> hierarchy;
            vector<Vec3f> circles;
            int stage = 0;
            double width = 0.0, height = 0.0;
            int lensNum = 0;
            bool sizeDetected = false;
            bool circleDetected = false;
            auto last_output_time = steady_clock::now();

            if (!cap->open(0)) {
                AppendTextToOutputBox("카메라를 열 수 없습니다.");
                return;
            }

            while (true)
            {
                
                bool ret = cap->read(img_color);
                if (!ret || img_color.empty()) {
                    AppendTextToOutputBox("캡쳐 실패 또는 빈 이미지");
                    continue;
                }

                if (stage == 0)
                {
                    sizeDetected = size_detect(this, cap->getCapture(),width, height);
                    imshow("Detection", img_color);
                    if (sizeDetected)
                    {
                        AppendTextToOutputBox("크기 검출 성공, 원 검출로 넘어갑니다.");
                        
                        this->statusLabel->Text = L"현재 상태: 원 검출 중";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                        stage = 1; // 다음 단계로 설정
                      
                    }

                    if (waitKey(1) == 27) // ESC 키로 단계 전환
                    {
                        stage = 1; // 강제 다음 단계로
                    }
                }
                else if (stage == 1)
                {
                    if (!circleDetected) // 렌즈 검출이 아직 되지 않은 경우에만 실행
                    {
                        cvtColor(img_color, img_gray, COLOR_BGR2GRAY);
                        medianBlur(img_gray, img_gray, 3);
                        HoughCircles(img_gray, circles, HOUGH_GRADIENT, 1, img_gray.rows / 8, 100, 50, 0, 0);

                       int lensNum = static_cast<int>(circles.size());

                        AppendTextToOutputBox("검출된 렌즈 개수 : " + lensNum.ToString());
                        for (size_t i = 0; i < circles.size(); i++) {
                            Vec3f c = circles[i];
                            cv::Point center(cvRound(c[0]), cvRound(c[1]));
                            int radius = cvRound(c[2]);
                            circle(img_color, center, radius, Scalar(0, 255, 0), 2);
                            circle(img_color, center, 2, Scalar(0, 0, 255), 3);
                        }

                        AppendTextToOutputBox("렌즈 검출 완료. ESC 키를 눌러서 다음 단계로");
                        imshow("Detection", img_color);

                        if (!circles.empty()) // 렌즈 검출이 성공적으로 이루어졌을 때
                        {
                            circleDetected = true; // 한 번만 수행하도록 플래그 설정
                        }
                    }

                    if (circleDetected && waitKey(1) == 27) // 렌즈 검출 후 ESC 키로 다음 단계로
                    {
                        if (stage == 1)
                        {
                            stage = 2;
                        }
                    }
                }
                else if (stage == 2)
                {
                    cvtColor(img_color, img_gray, COLOR_BGR2GRAY);
                    GaussianBlur(img_gray, img_blur, cv::Size(5, 5), 0);
                    Canny(img_blur, img_edges, 50, 150);
                    findContours(img_edges, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

                    img_contours = img_color.clone();

                    if (!contours.empty()) {
                        size_t largest_contour_index = 0;
                        double largest_area = 0;

                        for (size_t i = 0; i < contours.size(); i++) {
                            double area = contourArea(contours[i]);
                            if (area > largest_area) {
                                largest_area = area;
                                largest_contour_index = i;
                            }
                        }

                        Mat mask = Mat::zeros(img_color.size(), CV_8UC1);
                        drawContours(mask, contours, (int)largest_contour_index, Scalar(255), FILLED);

                        img_color.copyTo(img_roi, mask);

                        Scalar mean_color = mean(img_roi, mask);

                        int B = static_cast<int>(mean_color[0]);
                        int G = static_cast<int>(mean_color[1]);
                        int R = static_cast<int>(mean_color[2]);

                        string detected_color;
                        if (B >= 120 && B <= 180 && G < 100 && R < 100) {
                            detected_color = "Blue";
                        }
                        else if (G >= 120 && G <= 180 && B < 100 && R < 100) {
                            detected_color = "Green";
                        }
                        else if (R >= 150 && R <= 200 && B < 100 && G < 100) {
                            detected_color = "Red";
                        }
                        else if (B >= 220 && G >= 220 && R >= 220) {
                            detected_color = "White";  // 흰색
                        }
                        else if (B < 50 && G < 50 && R < 50) {
                            detected_color = "Black";  // 검은색
                        }
                        else if (B >= 150 && B <= 180 && G >= 150 && G <= 200 && R >= 150 && R <= 200) {
                            detected_color = "Yellow";  // 노란색
                        }
                        else if (B >= 180 && B <= 220 && G < 100 && R >= 150 && R <= 220) {
                            detected_color = "Magenta";  // 마젠타색
                        }
                        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 100) {
                            detected_color = "Cyan";  // 청록색
                        }
                        else if (B >= 100 && B <= 160 && G < 100 && R >= 100 && R <= 180) {
                            detected_color = "Purple";  // 보라색
                        }
                        else if (B >= 180 && B <= 220 && G >= 150 && G <= 200 && R >= 200 && R <= 230) {
                            detected_color = "Pink";  // 핑크색
                        }
                        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 200) {
                            detected_color = "Silver";  // 은색
                        }
                        else if (B >= 150 && B <= 200 && G >= 140 && G <= 180 && R >= 180 && R <= 220) {
                            detected_color = "Gold";  // 골드색
                        }
                        else if (B >= 90 && B <= 130 && G >= 90 && G <= 130 && R >= 90 && R <= 130) {
                            detected_color = "Gray";  // 회색
                        }
                        else {
                            detected_color = "Unknown";
                        }

                        AppendTextToOutputBox("Color: " + gcnew System::String(detected_color.c_str()) +
                            " (B: " + B.ToString() + ", G: " + G.ToString() + ", R: " + R.ToString() + ")");
                        drawContours(img_contours, contours, (int)largest_contour_index, Scalar(0, 255, 0), 2);
                        AppendTextToOutputBox("색 검출 완료");

                        imshow("Detection", img_contours);

                        std::vector<uchar> imageBuffer = CaptureImageToMemory(img_color);
                        AppendTextToOutputBox("원본 이미지 메모리 저장 완료 : " + imageBuffer.size().ToString());

                        string name = "s";
                        get_INFO(width, height, lensNum, detected_color, imageBuffer);
                       /* db->InsertData(width,height, lensNum, detected_color, imageBuffer);*/

                        waitKey(0); // 색 검출 완료 후 프로그램 종료를 기다림
                        AppendTextToOutputBox("프로그램 종료.");
                        return;
                    }
                }
            }
        }

        void reDetectPhone()
        {
            if (cap == nullptr)
            {
                AppendTextToOutputBox("캡쳐 객체가 초기화되지 않았습니다.");
                return;
            }
            Mat img_color, img_gray, img_blur, img_edges, img_contours, img_roi;
            vector<vector<cv::Point>> contours;
            vector<Vec4i> hierarchy;
            vector<Vec3f> circles;
            int stage = 0;
            double width = 0.0, height = 0.0;
            int lensNum = 0;
            bool sizeDetected = false;
            bool circleDetected = false;
            auto last_output_time = steady_clock::now();

            

            if (!cap->open(0)) {
                AppendTextToOutputBox("카메라를 열 수 없습니다.");
                return;
            }

            while (true)
            {

                bool ret = cap->read(img_color);
                if (!ret || img_color.empty()) {
                    AppendTextToOutputBox("캡쳐 실패 또는 빈 이미지");
                    continue;
                }

                if (stage == 0)
                {
                    sizeDetected = size_detect(this, cap->getCapture(), width, height);
                    imshow("Detection", img_color);
                    if (sizeDetected)
                    {
                        AppendTextToOutputBox("크기 검출 성공, 원 검출로 넘어갑니다.");

                        this->statusLabel->Text = L"현재 상태: 원 검출 중";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                        stage = 1; // 다음 단계로 설정

                    }

                    if (waitKey(1) == 27) // ESC 키로 단계 전환
                    {
                        stage = 1; // 강제 다음 단계로
                    }
                }
                else if (stage == 1)
                {
                    if (!circleDetected) // 렌즈 검출이 아직 되지 않은 경우에만 실행
                    {
                        cvtColor(img_color, img_gray, COLOR_BGR2GRAY);
                        medianBlur(img_gray, img_gray, 3);
                        HoughCircles(img_gray, circles, HOUGH_GRADIENT, 1, img_gray.rows / 8, 100, 50, 0, 0);

                        int lensNum = circles.size();

                        AppendTextToOutputBox("검출된 렌즈 개수 : " + lensNum.ToString());
                        for (size_t i = 0; i < circles.size(); i++) {
                            Vec3f c = circles[i];
                            cv::Point center(cvRound(c[0]), cvRound(c[1]));
                            int radius = cvRound(c[2]);
                            circle(img_color, center, radius, Scalar(0, 255, 0), 2);
                            circle(img_color, center, 2, Scalar(0, 0, 255), 3);
                        }

                        AppendTextToOutputBox("렌즈 검출 완료. ESC 키를 눌러서 다음 단계로");
                        imshow("Detection", img_color);

                        if (!circles.empty()) // 렌즈 검출이 성공적으로 이루어졌을 때
                        {
                            circleDetected = true; // 한 번만 수행하도록 플래그 설정
                        }
                    }

                    if (circleDetected && waitKey(1) == 27) // 렌즈 검출 후 ESC 키로 다음 단계로
                    {
                        if (stage == 1)
                        {
                            stage = 2;
                        }
                    }
                }
                else if (stage == 2)
                {
                    cvtColor(img_color, img_gray, COLOR_BGR2GRAY);
                    GaussianBlur(img_gray, img_blur, cv::Size(5, 5), 0);
                    Canny(img_blur, img_edges, 50, 150);
                    findContours(img_edges, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

                    img_contours = img_color.clone();

                    if (!contours.empty()) {
                        size_t largest_contour_index = 0;
                        double largest_area = 0;

                        for (size_t i = 0; i < contours.size(); i++) {
                            double area = contourArea(contours[i]);
                            if (area > largest_area) {
                                largest_area = area;
                                largest_contour_index = i;
                            }
                        }

                        Mat mask = Mat::zeros(img_color.size(), CV_8UC1);
                        drawContours(mask, contours, (int)largest_contour_index, Scalar(255), FILLED);

                        img_color.copyTo(img_roi, mask);

                        Scalar mean_color = mean(img_roi, mask);

                        int B = static_cast<int>(mean_color[0]);
                        int G = static_cast<int>(mean_color[1]);
                        int R = static_cast<int>(mean_color[2]);

                        string detected_color;
                        if (B >= 120 && B <= 180 && G < 100 && R < 100) {
                            detected_color = "Blue";
                        }
                        else if (G >= 120 && G <= 180 && B < 100 && R < 100) {
                            detected_color = "Green";
                        }
                        else if (R >= 150 && R <= 200 && B < 100 && G < 100) {
                            detected_color = "Red";
                        }
                        else if (B >= 220 && G >= 220 && R >= 220) {
                            detected_color = "White";  // 흰색
                        }
                        else if (B < 50 && G < 50 && R < 50) {
                            detected_color = "Black";  // 검은색
                        }
                        else if (B >= 150 && B <= 180 && G >= 150 && G <= 200 && R >= 150 && R <= 200) {
                            detected_color = "Yellow";  // 노란색
                        }
                        else if (B >= 180 && B <= 220 && G < 100 && R >= 150 && R <= 220) {
                            detected_color = "Magenta";  // 마젠타색
                        }
                        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 100) {
                            detected_color = "Cyan";  // 청록색
                        }
                        else if (B >= 100 && B <= 160 && G < 100 && R >= 100 && R <= 180) {
                            detected_color = "Purple";  // 보라색
                        }
                        else if (B >= 180 && B <= 220 && G >= 150 && G <= 200 && R >= 200 && R <= 230) {
                            detected_color = "Pink";  // 핑크색
                        }
                        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 200) {
                            detected_color = "Silver";  // 은색
                        }
                        else if (B >= 150 && B <= 200 && G >= 140 && G <= 180 && R >= 180 && R <= 220) {
                            detected_color = "Gold";  // 골드색
                        }
                        else if (B >= 90 && B <= 130 && G >= 90 && G <= 130 && R >= 90 && R <= 130) {
                            detected_color = "Gray";  // 회색
                        }
                        else {
                            detected_color = "Unknown";
                        }

                        AppendTextToOutputBox("Color: " + gcnew System::String(detected_color.c_str()) +
                            " (B: " + B.ToString() + ", G: " + G.ToString() + ", R: " + R.ToString() + ")");
                        drawContours(img_contours, contours, (int)largest_contour_index, Scalar(0, 255, 0), 2);
                        AppendTextToOutputBox("색 검출 완료");

                        imshow("Detection", img_contours);

                        std::vector<uchar> imageBuffer = CaptureImageToMemory(img_color);
                        AppendTextToOutputBox("원본 이미지 메모리 저장 완료 : " + imageBuffer.size().ToString());

                       
                        Matching_INFO(width, height, lensNum, detected_color, imageBuffer);


                        waitKey(0); // 색 검출 완료 후 프로그램 종료를 기다림
                        AppendTextToOutputBox("프로그램 종료.");
                        return;
                    }
                }
            }
        }
		
        void StopCamera()
        {
            isCameraRunning = false;
            stopCapture = true;  // 캡처 중지를 위한 플래그 설정

            
            if (cap != nullptr)
            {
                cap->clear();
                delete cap;
                cap = nullptr;
            }

            
            cv::destroyAllWindows();

            this->statusLabel->Text = L"현재 상태: 정지";
            this->statusIndicator->BackColor = System::Drawing::Color::Red;
            this->statusIndicator->Invalidate();
        }


    };
}
