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
#include <vcclr.h> // gcroot�� ����ϱ����� ����
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
                // RichTextBox�� �ؽ�Ʈ�� �߰��մϴ�.
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

            // ���� ���� ��ư
            this->startButton = gcnew System::Windows::Forms::Button();
            this->startButton->Location = System::Drawing::Point(100, 150);  // ��ư ��ġ ����
            this->startButton->Size = System::Drawing::Size(100, 50);        // ��ư ũ�� ����
            this->startButton->Text = L"���� �Կ�����";

            this->startButton->Click += gcnew System::EventHandler(this, &Form1::startButton_Click);
            this->Controls->Add(this->startButton);

            // ���� ���� ��ư
            this->stopButton = gcnew System::Windows::Forms::Button();
            this->stopButton->Location = System::Drawing::Point(220, 250);   // ��ư ��ġ ����
            this->stopButton->Size = System::Drawing::Size(100, 50);         // ��ư ũ�� ����
            this->stopButton->Text = L"���� ����";
            this->stopButton->Click += gcnew System::EventHandler(this, &Form1::stopButton_Click);
            this->Controls->Add(this->stopButton);

            // ��Ī ��ư
            this->matchingButton = gcnew System::Windows::Forms::Button();
            this->matchingButton -> Location = System::Drawing::Point(350, 150);
            this->matchingButton->Size = System::Drawing::Size(100, 50);
            this->matchingButton->Text = L"�����";
            this->matchingButton->Click += gcnew System::EventHandler(this, &Form1::matchingButton_Click);
            this->Controls->Add(this->matchingButton);

            // ��1
            this->label1 = gcnew System::Windows::Forms::Label();
            this->label1->Location = System::Drawing::Point(160, 50);  // Label ��ġ ����
            this->label1->Size = System::Drawing::Size(300, 30);       // Label ũ�� ����
            this->label1->Text = L"�ڵ��� ���̽� ���� ��";                // Label �ؽ�Ʈ ����

            this->label1->Font = gcnew System::Drawing::Font(L"Arial", 20); // Arial ��Ʈ, 20pt ũ�� ����
            this->Controls->Add(this->label1);

            // ���� Label �߰�
            this->statusLabel = gcnew System::Windows::Forms::Label();
            this->statusLabel->Location = System::Drawing::Point(150, 400);   // Label ��ġ ����
            this->statusLabel->Size = System::Drawing::Size(200, 30);       // Label ũ�� ����
            this->statusLabel->Font = gcnew System::Drawing::Font(L"Arial", 20);  // ��Ʈ�� ũ�� ����
            this->statusLabel->Text = L"���� ����: �غ� ��";                // �ʱ� �ؽ�Ʈ ����
            this->Controls->Add(this->statusLabel);

            // ���� ǥ�� Panel �߰�
            this->statusIndicator = gcnew System::Windows::Forms::Panel();
            this->statusIndicator->Location = System::Drawing::Point(380, 405); // Panel ��ġ ����
            this->statusIndicator->Size = System::Drawing::Size(30, 30);      // Panel ũ�� ���� (���簢������ ����)
            this->statusIndicator->BackColor = System::Drawing::Color::Gray;  // �ʱ� ���� ���� (ȸ��)
            this->statusIndicator->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::statusIndicator_Paint);
            this->Controls->Add(this->statusIndicator);

            // RichTextBox �ʱ�ȭ
            this->outputBox = gcnew RichTextBox();
            this->outputBox->Location = System::Drawing::Point(550, 20);  // ��ġ ����
            this->outputBox->Size = System::Drawing::Size(430, 550);      // ũ�� ����
            this->outputBox->Font = gcnew System::Drawing::Font(L"Arial", 12); // ��Ʈ ����
            this->outputBox->ReadOnly = true;                            // �б� ���� ����
            this->outputBox->BackColor = System::Drawing::Color::FromArgb(102, 153, 204);
            this->Controls->Add(this->outputBox); // ���� �߰�

            cap = new VideoCaptureWrapper();
            //db = new MySql::Database();
        }

        void statusIndicator_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e)
        {
            // �� �׸���
            Graphics^ g = e->Graphics;
            Brush^ brush = gcnew SolidBrush(this->statusIndicator->BackColor);
            g->FillEllipse(brush, 0, 0, this->statusIndicator->Width, this->statusIndicator->Height);
        }

        void startButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            if (!isCameraRunning)
            {
                isCameraRunning = true;
                this->statusLabel->Text = L"���� ����: ���� ��";  // Label �ؽ�Ʈ ������Ʈ
                this->statusIndicator->BackColor = System::Drawing::Color::Green;  // �ʷϻ� ������ ����
                this->statusIndicator->Invalidate(); // Paint �̺�Ʈ�� Ʈ�����Ͽ� ���� �ٽ� �׸�

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
                AppendTextToOutputBox("ĸ�� ��ü�� �ʱ�ȭ���� �ʾҽ��ϴ�.");
                cap = new VideoCaptureWrapper();
            }

            if (!isCameraRunning)
            {
                if(!cap->open(0))
                {

                        AppendTextToOutputBox("ī�޶� �� �� �����ϴ�.");
                        return;
                }
                isCameraRunning = true;
                this->statusLabel->Text = L"���� ����: ��Ī ��";  // Label �ؽ�Ʈ ������Ʈ
                this->statusIndicator->BackColor = System::Drawing::Color::FromArgb(255, 165, 0);  // ��Ȳ�� ������ ����
                this->statusIndicator->Invalidate();


                reDetectPhone();


            }
        }


        void StartDetectionFlow()
        {
            if (!cap->open(0))
            {
                AppendTextToOutputBox("ī�޶� �� �� �����ϴ�.");
                return;
            }

            DetectedPhone();


        }

        // �̹����� �޸𸮿� �����ϰ� �ּҰ��� ��ȯ�ϴ� �Լ�
        vector<uchar> CaptureImageToMemory(const Mat& image)
        {
            vector<uchar> buffer;  // �̹��� �����͸� ������ ����
            imencode(".png", image, buffer);  // �̹����� PNG �������� �޸𸮿� ����
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
                AppendTextToOutputBox("ī�޶� �� �� �����ϴ�.");
                return;
            }

            while (true)
            {
                
                bool ret = cap->read(img_color);
                if (!ret || img_color.empty()) {
                    AppendTextToOutputBox("ĸ�� ���� �Ǵ� �� �̹���");
                    continue;
                }

                if (stage == 0)
                {
                    sizeDetected = size_detect(this, cap->getCapture(),width, height);
                    imshow("Detection", img_color);
                    if (sizeDetected)
                    {
                        AppendTextToOutputBox("ũ�� ���� ����, �� ����� �Ѿ�ϴ�.");
                        
                        this->statusLabel->Text = L"���� ����: �� ���� ��";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                        stage = 1; // ���� �ܰ�� ����
                      
                    }

                    if (waitKey(1) == 27) // ESC Ű�� �ܰ� ��ȯ
                    {
                        stage = 1; // ���� ���� �ܰ��
                    }
                }
                else if (stage == 1)
                {
                    if (!circleDetected) // ���� ������ ���� ���� ���� ��쿡�� ����
                    {
                        cvtColor(img_color, img_gray, COLOR_BGR2GRAY);
                        medianBlur(img_gray, img_gray, 3);
                        HoughCircles(img_gray, circles, HOUGH_GRADIENT, 1, img_gray.rows / 8, 100, 50, 0, 0);

                       int lensNum = static_cast<int>(circles.size());

                        AppendTextToOutputBox("����� ���� ���� : " + lensNum.ToString());
                        for (size_t i = 0; i < circles.size(); i++) {
                            Vec3f c = circles[i];
                            cv::Point center(cvRound(c[0]), cvRound(c[1]));
                            int radius = cvRound(c[2]);
                            circle(img_color, center, radius, Scalar(0, 255, 0), 2);
                            circle(img_color, center, 2, Scalar(0, 0, 255), 3);
                        }

                        AppendTextToOutputBox("���� ���� �Ϸ�. ESC Ű�� ������ ���� �ܰ��");
                        imshow("Detection", img_color);

                        if (!circles.empty()) // ���� ������ ���������� �̷������ ��
                        {
                            circleDetected = true; // �� ���� �����ϵ��� �÷��� ����
                        }
                    }

                    if (circleDetected && waitKey(1) == 27) // ���� ���� �� ESC Ű�� ���� �ܰ��
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
                            detected_color = "White";  // ���
                        }
                        else if (B < 50 && G < 50 && R < 50) {
                            detected_color = "Black";  // ������
                        }
                        else if (B >= 150 && B <= 180 && G >= 150 && G <= 200 && R >= 150 && R <= 200) {
                            detected_color = "Yellow";  // �����
                        }
                        else if (B >= 180 && B <= 220 && G < 100 && R >= 150 && R <= 220) {
                            detected_color = "Magenta";  // ����Ÿ��
                        }
                        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 100) {
                            detected_color = "Cyan";  // û�ϻ�
                        }
                        else if (B >= 100 && B <= 160 && G < 100 && R >= 100 && R <= 180) {
                            detected_color = "Purple";  // �����
                        }
                        else if (B >= 180 && B <= 220 && G >= 150 && G <= 200 && R >= 200 && R <= 230) {
                            detected_color = "Pink";  // ��ũ��
                        }
                        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 200) {
                            detected_color = "Silver";  // ����
                        }
                        else if (B >= 150 && B <= 200 && G >= 140 && G <= 180 && R >= 180 && R <= 220) {
                            detected_color = "Gold";  // ����
                        }
                        else if (B >= 90 && B <= 130 && G >= 90 && G <= 130 && R >= 90 && R <= 130) {
                            detected_color = "Gray";  // ȸ��
                        }
                        else {
                            detected_color = "Unknown";
                        }

                        AppendTextToOutputBox("Color: " + gcnew System::String(detected_color.c_str()) +
                            " (B: " + B.ToString() + ", G: " + G.ToString() + ", R: " + R.ToString() + ")");
                        drawContours(img_contours, contours, (int)largest_contour_index, Scalar(0, 255, 0), 2);
                        AppendTextToOutputBox("�� ���� �Ϸ�");

                        imshow("Detection", img_contours);

                        std::vector<uchar> imageBuffer = CaptureImageToMemory(img_color);
                        AppendTextToOutputBox("���� �̹��� �޸� ���� �Ϸ� : " + imageBuffer.size().ToString());

                        string name = "s";
                        get_INFO(width, height, lensNum, detected_color, imageBuffer);
                       /* db->InsertData(width,height, lensNum, detected_color, imageBuffer);*/

                        waitKey(0); // �� ���� �Ϸ� �� ���α׷� ���Ḧ ��ٸ�
                        AppendTextToOutputBox("���α׷� ����.");
                        return;
                    }
                }
            }
        }

        void reDetectPhone()
        {
            if (cap == nullptr)
            {
                AppendTextToOutputBox("ĸ�� ��ü�� �ʱ�ȭ���� �ʾҽ��ϴ�.");
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
                AppendTextToOutputBox("ī�޶� �� �� �����ϴ�.");
                return;
            }

            while (true)
            {

                bool ret = cap->read(img_color);
                if (!ret || img_color.empty()) {
                    AppendTextToOutputBox("ĸ�� ���� �Ǵ� �� �̹���");
                    continue;
                }

                if (stage == 0)
                {
                    sizeDetected = size_detect(this, cap->getCapture(), width, height);
                    imshow("Detection", img_color);
                    if (sizeDetected)
                    {
                        AppendTextToOutputBox("ũ�� ���� ����, �� ����� �Ѿ�ϴ�.");

                        this->statusLabel->Text = L"���� ����: �� ���� ��";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                        stage = 1; // ���� �ܰ�� ����

                    }

                    if (waitKey(1) == 27) // ESC Ű�� �ܰ� ��ȯ
                    {
                        stage = 1; // ���� ���� �ܰ��
                    }
                }
                else if (stage == 1)
                {
                    if (!circleDetected) // ���� ������ ���� ���� ���� ��쿡�� ����
                    {
                        cvtColor(img_color, img_gray, COLOR_BGR2GRAY);
                        medianBlur(img_gray, img_gray, 3);
                        HoughCircles(img_gray, circles, HOUGH_GRADIENT, 1, img_gray.rows / 8, 100, 50, 0, 0);

                        int lensNum = circles.size();

                        AppendTextToOutputBox("����� ���� ���� : " + lensNum.ToString());
                        for (size_t i = 0; i < circles.size(); i++) {
                            Vec3f c = circles[i];
                            cv::Point center(cvRound(c[0]), cvRound(c[1]));
                            int radius = cvRound(c[2]);
                            circle(img_color, center, radius, Scalar(0, 255, 0), 2);
                            circle(img_color, center, 2, Scalar(0, 0, 255), 3);
                        }

                        AppendTextToOutputBox("���� ���� �Ϸ�. ESC Ű�� ������ ���� �ܰ��");
                        imshow("Detection", img_color);

                        if (!circles.empty()) // ���� ������ ���������� �̷������ ��
                        {
                            circleDetected = true; // �� ���� �����ϵ��� �÷��� ����
                        }
                    }

                    if (circleDetected && waitKey(1) == 27) // ���� ���� �� ESC Ű�� ���� �ܰ��
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
                            detected_color = "White";  // ���
                        }
                        else if (B < 50 && G < 50 && R < 50) {
                            detected_color = "Black";  // ������
                        }
                        else if (B >= 150 && B <= 180 && G >= 150 && G <= 200 && R >= 150 && R <= 200) {
                            detected_color = "Yellow";  // �����
                        }
                        else if (B >= 180 && B <= 220 && G < 100 && R >= 150 && R <= 220) {
                            detected_color = "Magenta";  // ����Ÿ��
                        }
                        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 100) {
                            detected_color = "Cyan";  // û�ϻ�
                        }
                        else if (B >= 100 && B <= 160 && G < 100 && R >= 100 && R <= 180) {
                            detected_color = "Purple";  // �����
                        }
                        else if (B >= 180 && B <= 220 && G >= 150 && G <= 200 && R >= 200 && R <= 230) {
                            detected_color = "Pink";  // ��ũ��
                        }
                        else if (B >= 180 && B <= 220 && G >= 180 && G <= 220 && R < 200) {
                            detected_color = "Silver";  // ����
                        }
                        else if (B >= 150 && B <= 200 && G >= 140 && G <= 180 && R >= 180 && R <= 220) {
                            detected_color = "Gold";  // ����
                        }
                        else if (B >= 90 && B <= 130 && G >= 90 && G <= 130 && R >= 90 && R <= 130) {
                            detected_color = "Gray";  // ȸ��
                        }
                        else {
                            detected_color = "Unknown";
                        }

                        AppendTextToOutputBox("Color: " + gcnew System::String(detected_color.c_str()) +
                            " (B: " + B.ToString() + ", G: " + G.ToString() + ", R: " + R.ToString() + ")");
                        drawContours(img_contours, contours, (int)largest_contour_index, Scalar(0, 255, 0), 2);
                        AppendTextToOutputBox("�� ���� �Ϸ�");

                        imshow("Detection", img_contours);

                        std::vector<uchar> imageBuffer = CaptureImageToMemory(img_color);
                        AppendTextToOutputBox("���� �̹��� �޸� ���� �Ϸ� : " + imageBuffer.size().ToString());

                       
                        Matching_INFO(width, height, lensNum, detected_color, imageBuffer);


                        waitKey(0); // �� ���� �Ϸ� �� ���α׷� ���Ḧ ��ٸ�
                        AppendTextToOutputBox("���α׷� ����.");
                        return;
                    }
                }
            }
        }
		
        void StopCamera()
        {
            isCameraRunning = false;
            stopCapture = true;  // ĸó ������ ���� �÷��� ����

            
            if (cap != nullptr)
            {
                cap->clear();
                delete cap;
                cap = nullptr;
            }

            
            cv::destroyAllWindows();

            this->statusLabel->Text = L"���� ����: ����";
            this->statusIndicator->BackColor = System::Drawing::Color::Red;
            this->statusIndicator->Invalidate();
        }


    };
}
