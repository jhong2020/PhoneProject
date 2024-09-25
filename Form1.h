#pragma once
#define WIN32_LEAN_AND_MEAN
#include "DB_head.h"
#include "NameInputForm.h"
#include "VideoCaptureWrapper.h"
#include "Size.h"
#include "hough_circle.h"
#include "Color_detect.h"
#include "Image_search.h"


namespace Project1 {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace chrono;
    using std::string;
    using std::vector;

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

        void InitializeComponent(void)
        {
            this->components = gcnew System::ComponentModel::Container();
            this->Text = L"Form1";
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(1000, 600);
            this->BackColor = System::Drawing::Color::White;

            // 실행 시작 버튼
            this->startButton = gcnew System::Windows::Forms::Button();
            this->startButton->Location = System::Drawing::Point(100, 200);  // 버튼 위치 설정
            this->startButton->Size = System::Drawing::Size(100, 50);        // 버튼 크기 설정
            this->startButton->Text = L"등  록";

            this->startButton->Click += gcnew System::EventHandler(this, &Form1::startButton_Click);
            this->Controls->Add(this->startButton);

            // 실행 중지 버튼
            //this->stopButton = gcnew System::Windows::Forms::Button();
            //this->stopButton->Location = System::Drawing::Point(220, 250);   // 버튼 위치 설정
            //this->stopButton->Size = System::Drawing::Size(100, 50);         // 버튼 크기 설정
            //this->stopButton->Text = L"실행 중지";
            //this->stopButton->Click += gcnew System::EventHandler(this, &Form1::stopButton_Click);
            //this->Controls->Add(this->stopButton);

            // 매칭 버튼
            this->matchingButton = gcnew System::Windows::Forms::Button();
            this->matchingButton->Location = System::Drawing::Point(350, 200);
            this->matchingButton->Size = System::Drawing::Size(100, 50);
            this->matchingButton->Text = L"매   칭";
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
                DetectedPhone();
            }
        }

        /*void stopButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            if (isCameraRunning)
            {
                StopCamera();
            }
        }*/

        void matchingButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            if (!isCameraRunning)
            {
                isCameraRunning = true;
                this->statusLabel->Text = L"현재 상태: 매칭 중";  // Label 텍스트 업데이트
                this->statusIndicator->BackColor = System::Drawing::Color::FromArgb(255, 165, 0);  // 주황색 원으로 변경
                this->statusIndicator->Invalidate();
                reDetectPhone();
            }
        }

        // 이미지를 메모리에 저장하고 주소값을 반환하는 함수
        vector<uchar> CaptureImageToMemory(const Mat& image)
        {
            vector<uchar> buffer;  // 이미지 데이터를 저장할 버퍼
            imencode(".png", image, buffer);  // 이미지를 PNG 형식으로 메모리에 저장
            return buffer;
        }

        // 검출
        void DetectedPhone()
        {
            Mat img_color;
            vector<uchar> imageBuffer;
            int stage = 0;
            double width = 0.0, height = 0.0;
            int lensNum = 0;
            int frameCounter = 0;
            bool sizeDetected = false;
            bool circleDetected = false;
            string detectedColor;

            if (cap == nullptr)
            {
                cap = new VideoCaptureWrapper;
            }

            if (!cap->open(0)) {
                AppendTextToOutputBox("카메라를 열 수 없습니다.");
                return;
            }

            while (1)
            {

                if (stage == 0)
                {
                    sizeDetected = size_detect(this, cap->getCapture(), width, height);
                    //imshow("Detection", img_color);
                    if (sizeDetected)
                    {
                        AppendTextToOutputBox("크기 검출 성공, 원 검출로 넘어갑니다.");
                        stage = 1;
                        this->statusLabel->Text = L"현재 상태: 원 검출 중";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // 검출 실패 시
                        AppendTextToOutputBox("사이즈 검출 실패.");
                        this->statusLabel->Text = L"현재 상태: 사이즈 검출 실패";
                        this->statusIndicator->BackColor = System::Drawing::Color::Red;
                        this->statusIndicator->Invalidate();
                        sizeDetected = false;
                        if (sizeDetected == false) {
                            StopCamera();
                            break;
                        }
                    }
                }
                else if (stage == 1)
                {
                    circleDetected = hough_circle(this, cap->getCapture(), lensNum);
                    if (circleDetected)
                    {
                        AppendTextToOutputBox("원 성공, 컬러 검출로 넘어갑니다.");
                        stage = 2;
                        this->statusLabel->Text = L"현재 상태: 컬러 검출 중";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // 검출 실패 시
                        AppendTextToOutputBox("원 검출 실패.");
                        this->statusLabel->Text = L"현재 상태: 원 검출 실패";
                        this->statusIndicator->BackColor = System::Drawing::Color::Red;
                        this->statusIndicator->Invalidate();
                        circleDetected = false;
                        if (circleDetected == false) {
                            StopCamera();
                            break;
                        }
                    }
                }

                else if (stage == 2)
                {
                    detectedColor = color_detect(this, cap->getCapture());

                    if (!detectedColor.empty()) { // 검출 성공 시
                        stage = 3;
                        this->statusLabel->Text = L"현재 상태: 컬러 검출 중";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // 검출 실패 시
                        AppendTextToOutputBox("컬러 검출 실패.");
                        this->statusLabel->Text = L"현재 상태: 컬러 검출 실패";
                        this->statusIndicator->BackColor = System::Drawing::Color::Red;
                        this->statusIndicator->Invalidate();
                    }
                }

                else if (stage == 3)
                {
                    bool ret = cap->read(img_color);
                    if (!ret) {
                        cout << "화면 안켜짐" << endl;
                    }

                    imshow("Detection", img_color);

                    imageBuffer = CaptureImageToMemory(img_color);
                    AppendTextToOutputBox("원본 이미지 메모리 저장 완료 : " + imageBuffer.size().ToString());

                    stage = 4;
                }

                else if (stage == 4)
                {
                    Project1::NameInputForm^ nameInputForm = gcnew Project1::NameInputForm();
                    if (nameInputForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                        // System::String^ -> std::string 변환
                        System::String^ managedName = nameInputForm->GetName();
                        IntPtr ptrToNativeString = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(managedName);
                        std::string nameStd = static_cast<const char*>(ptrToNativeString.ToPointer());
                        System::Runtime::InteropServices::Marshal::FreeHGlobal(ptrToNativeString);

                        // get_INFO 함수 호출 시 std::string을 사용                  
                        if (get_INFO(nameStd, width, height, lensNum, detectedColor, imageBuffer) == 1) {
                            AppendTextToOutputBox("DB에 데이터를 전송했습니다.");
                            AppendTextToOutputBox("카메라 종료");
                            stage = 0;
                            this->statusLabel->Text = L"대기중";
                            this->statusIndicator->BackColor = System::Drawing::Color::Red;
                            this->statusIndicator->Invalidate();
                            StopCamera();
                            break;
                        }
                    }
                }
            }
        }

        void reDetectPhone()
        {

            Mat img_color, capturedImage;
            vector<uchar> imageBuffer;

            int stage = 0, lensNum = 0;
            double width = 0.0, height = 0.0;
            bool sizeDetected = false;
            bool circleDetected = false;
            bool Matcing_img = false;
            string detectedcolor;  // 검출된 색상 변수 선언
            std::string nameStd;

            if (cap == nullptr)
            {
                cap = new VideoCaptureWrapper;
            }

            if (!cap->open(0)) {
                AppendTextToOutputBox("카메라를 열 수 없습니다.");
                StopCamera();
                return;
            }

            while (true)
            {
                bool ret = cap->read(capturedImage);
                if (!ret || capturedImage.empty()) {
                    AppendTextToOutputBox("캡쳐 실패 또는 빈 이미지");
                    continue;
                }

                if (stage == 0)
                {
                    sizeDetected = size_detect(this, cap->getCapture(), width, height);
                    if (sizeDetected)
                    {
                        AppendTextToOutputBox("크기 검출 성공, 원 검출로 넘어갑니다.");
                        stage = 1;
                        this->statusLabel->Text = L"현재 상태: 원 검출 중";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // 검출 실패 시
                        AppendTextToOutputBox("사이즈 검출 실패.");
                        this->statusLabel->Text = L"현재 상태: 사이즈 검출 실패";
                        this->statusIndicator->BackColor = System::Drawing::Color::Red;
                        this->statusIndicator->Invalidate();
                        sizeDetected = false;
                        if (sizeDetected == false) {
                            StopCamera();
                            break;
                        }
                    }
                }
                else if (stage == 1)
                {
                    circleDetected = hough_circle(this, cap->getCapture(), lensNum);
                    if (circleDetected)
                    {
                        AppendTextToOutputBox("원 성공, 컬러 검출로 넘어갑니다.");
                        stage = 2;
                        this->statusLabel->Text = L"현재 상태: 컬러 검출 중";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // 검출 실패 시
                        AppendTextToOutputBox("원 검출 실패.");
                        this->statusLabel->Text = L"현재 상태: 원 검출 실패";
                        this->statusIndicator->BackColor = System::Drawing::Color::Red;
                        this->statusIndicator->Invalidate();
                        circleDetected = false;
                        if (circleDetected == false) {
                            StopCamera();
                            break;
                        }
                    }

                }

                else if (stage == 2)
                {
                    detectedcolor = color_detect(this, cap->getCapture());

                    if (!detectedcolor.empty()) { // 검출 성공 시
                        stage = 3;
                        this->statusLabel->Text = L"현재 상태: 컬러 검출 중";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // 검출 실패 시
                        AppendTextToOutputBox("컬러 검출 실패.");
                        this->statusLabel->Text = L"현재 상태: 컬러 검출 실패";
                        this->statusIndicator->BackColor = System::Drawing::Color::Red;
                        this->statusIndicator->Invalidate();
                        StopCamera();
                    }
                }

                else if (stage == 3) {
                    bool ret = cap->read(capturedImage);
                    if (!ret) {
                        cout << "화면 안켜짐" << endl;
                    }

                    imshow("Detection", capturedImage);
                    imageBuffer = CaptureImageToMemory(capturedImage);
                    AppendTextToOutputBox("복사본 메모리 저장 완료 : " + imageBuffer.size().ToString());
                    stage = 4;
                }

                else if (stage == 4)
                {
                    /*Matcing_img = compareImages(img_color, capturedImage);*/
                    std::string matched_name;
                    double matched_width = 0.0, matched_height = 0.0;
                    int matched_lensNum = 0;
                    std::string matched_color;

                    bool match_found = Matching_INFO(nameStd, width, height, lensNum, detectedcolor, imageBuffer, capturedImage, matched_name, matched_width, matched_height, matched_lensNum, matched_color);
                    if (match_found)
                    {
                        AppendTextToOutputBox("입력한 정보와 일치하는 DB 정보를 찾았습니다.");
                        AppendTextToOutputBox("기종 : " + gcnew System::String(matched_name.c_str()));
                        AppendTextToOutputBox("Width : " + matched_width.ToString());
                        AppendTextToOutputBox("Height : " + matched_height.ToString());
                        AppendTextToOutputBox("LensNum : " + matched_lensNum.ToString());
                        AppendTextToOutputBox("Color : " + gcnew System::String(matched_color.c_str()));
                        StopCamera();
                        break;
                    }
                    else
                    {
                        AppendTextToOutputBox("입력한 정보와 일치하는 DB 정보를 찾지 못했습니다.");
                        StopCamera();
                        break;
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
                cap->release();
                delete cap;
                cap = nullptr;
                if (cap == nullptr) {
                    AppendTextToOutputBox("카메라 초기화 완료");
                };
            }

            cv::destroyAllWindows();

            this->statusLabel->Text = L"현재 상태: 정지";
            this->statusIndicator->BackColor = System::Drawing::Color::Red;
            this->statusIndicator->Invalidate();
        }
    }; // NAMESPACE
}
