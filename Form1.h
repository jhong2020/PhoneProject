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

        void InitializeComponent(void)
        {
            this->components = gcnew System::ComponentModel::Container();
            this->Text = L"Form1";
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(1000, 600);
            this->BackColor = System::Drawing::Color::White;

            // ���� ���� ��ư
            this->startButton = gcnew System::Windows::Forms::Button();
            this->startButton->Location = System::Drawing::Point(100, 200);  // ��ư ��ġ ����
            this->startButton->Size = System::Drawing::Size(100, 50);        // ��ư ũ�� ����
            this->startButton->Text = L"��  ��";

            this->startButton->Click += gcnew System::EventHandler(this, &Form1::startButton_Click);
            this->Controls->Add(this->startButton);

            // ���� ���� ��ư
            //this->stopButton = gcnew System::Windows::Forms::Button();
            //this->stopButton->Location = System::Drawing::Point(220, 250);   // ��ư ��ġ ����
            //this->stopButton->Size = System::Drawing::Size(100, 50);         // ��ư ũ�� ����
            //this->stopButton->Text = L"���� ����";
            //this->stopButton->Click += gcnew System::EventHandler(this, &Form1::stopButton_Click);
            //this->Controls->Add(this->stopButton);

            // ��Ī ��ư
            this->matchingButton = gcnew System::Windows::Forms::Button();
            this->matchingButton->Location = System::Drawing::Point(350, 200);
            this->matchingButton->Size = System::Drawing::Size(100, 50);
            this->matchingButton->Text = L"��   Ī";
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
                this->statusLabel->Text = L"���� ����: ��Ī ��";  // Label �ؽ�Ʈ ������Ʈ
                this->statusIndicator->BackColor = System::Drawing::Color::FromArgb(255, 165, 0);  // ��Ȳ�� ������ ����
                this->statusIndicator->Invalidate();
                reDetectPhone();
            }
        }

        // �̹����� �޸𸮿� �����ϰ� �ּҰ��� ��ȯ�ϴ� �Լ�
        vector<uchar> CaptureImageToMemory(const Mat& image)
        {
            vector<uchar> buffer;  // �̹��� �����͸� ������ ����
            imencode(".png", image, buffer);  // �̹����� PNG �������� �޸𸮿� ����
            return buffer;
        }

        // ����
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
                AppendTextToOutputBox("ī�޶� �� �� �����ϴ�.");
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
                        AppendTextToOutputBox("ũ�� ���� ����, �� ����� �Ѿ�ϴ�.");
                        stage = 1;
                        this->statusLabel->Text = L"���� ����: �� ���� ��";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // ���� ���� ��
                        AppendTextToOutputBox("������ ���� ����.");
                        this->statusLabel->Text = L"���� ����: ������ ���� ����";
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
                        AppendTextToOutputBox("�� ����, �÷� ����� �Ѿ�ϴ�.");
                        stage = 2;
                        this->statusLabel->Text = L"���� ����: �÷� ���� ��";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // ���� ���� ��
                        AppendTextToOutputBox("�� ���� ����.");
                        this->statusLabel->Text = L"���� ����: �� ���� ����";
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

                    if (!detectedColor.empty()) { // ���� ���� ��
                        stage = 3;
                        this->statusLabel->Text = L"���� ����: �÷� ���� ��";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // ���� ���� ��
                        AppendTextToOutputBox("�÷� ���� ����.");
                        this->statusLabel->Text = L"���� ����: �÷� ���� ����";
                        this->statusIndicator->BackColor = System::Drawing::Color::Red;
                        this->statusIndicator->Invalidate();
                    }
                }

                else if (stage == 3)
                {
                    bool ret = cap->read(img_color);
                    if (!ret) {
                        cout << "ȭ�� ������" << endl;
                    }

                    imshow("Detection", img_color);

                    imageBuffer = CaptureImageToMemory(img_color);
                    AppendTextToOutputBox("���� �̹��� �޸� ���� �Ϸ� : " + imageBuffer.size().ToString());

                    stage = 4;
                }

                else if (stage == 4)
                {
                    Project1::NameInputForm^ nameInputForm = gcnew Project1::NameInputForm();
                    if (nameInputForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                        // System::String^ -> std::string ��ȯ
                        System::String^ managedName = nameInputForm->GetName();
                        IntPtr ptrToNativeString = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(managedName);
                        std::string nameStd = static_cast<const char*>(ptrToNativeString.ToPointer());
                        System::Runtime::InteropServices::Marshal::FreeHGlobal(ptrToNativeString);

                        // get_INFO �Լ� ȣ�� �� std::string�� ���                  
                        if (get_INFO(nameStd, width, height, lensNum, detectedColor, imageBuffer) == 1) {
                            AppendTextToOutputBox("DB�� �����͸� �����߽��ϴ�.");
                            AppendTextToOutputBox("ī�޶� ����");
                            stage = 0;
                            this->statusLabel->Text = L"�����";
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
            string detectedcolor;  // ����� ���� ���� ����
            std::string nameStd;

            if (cap == nullptr)
            {
                cap = new VideoCaptureWrapper;
            }

            if (!cap->open(0)) {
                AppendTextToOutputBox("ī�޶� �� �� �����ϴ�.");
                StopCamera();
                return;
            }

            while (true)
            {
                bool ret = cap->read(capturedImage);
                if (!ret || capturedImage.empty()) {
                    AppendTextToOutputBox("ĸ�� ���� �Ǵ� �� �̹���");
                    continue;
                }

                if (stage == 0)
                {
                    sizeDetected = size_detect(this, cap->getCapture(), width, height);
                    if (sizeDetected)
                    {
                        AppendTextToOutputBox("ũ�� ���� ����, �� ����� �Ѿ�ϴ�.");
                        stage = 1;
                        this->statusLabel->Text = L"���� ����: �� ���� ��";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // ���� ���� ��
                        AppendTextToOutputBox("������ ���� ����.");
                        this->statusLabel->Text = L"���� ����: ������ ���� ����";
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
                        AppendTextToOutputBox("�� ����, �÷� ����� �Ѿ�ϴ�.");
                        stage = 2;
                        this->statusLabel->Text = L"���� ����: �÷� ���� ��";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // ���� ���� ��
                        AppendTextToOutputBox("�� ���� ����.");
                        this->statusLabel->Text = L"���� ����: �� ���� ����";
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

                    if (!detectedcolor.empty()) { // ���� ���� ��
                        stage = 3;
                        this->statusLabel->Text = L"���� ����: �÷� ���� ��";
                        this->statusIndicator->BackColor = System::Drawing::Color::Green;
                        this->statusIndicator->Invalidate();
                    }
                    else { // ���� ���� ��
                        AppendTextToOutputBox("�÷� ���� ����.");
                        this->statusLabel->Text = L"���� ����: �÷� ���� ����";
                        this->statusIndicator->BackColor = System::Drawing::Color::Red;
                        this->statusIndicator->Invalidate();
                        StopCamera();
                    }
                }

                else if (stage == 3) {
                    bool ret = cap->read(capturedImage);
                    if (!ret) {
                        cout << "ȭ�� ������" << endl;
                    }

                    imshow("Detection", capturedImage);
                    imageBuffer = CaptureImageToMemory(capturedImage);
                    AppendTextToOutputBox("���纻 �޸� ���� �Ϸ� : " + imageBuffer.size().ToString());
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
                        AppendTextToOutputBox("�Է��� ������ ��ġ�ϴ� DB ������ ã�ҽ��ϴ�.");
                        AppendTextToOutputBox("���� : " + gcnew System::String(matched_name.c_str()));
                        AppendTextToOutputBox("Width : " + matched_width.ToString());
                        AppendTextToOutputBox("Height : " + matched_height.ToString());
                        AppendTextToOutputBox("LensNum : " + matched_lensNum.ToString());
                        AppendTextToOutputBox("Color : " + gcnew System::String(matched_color.c_str()));
                        StopCamera();
                        break;
                    }
                    else
                    {
                        AppendTextToOutputBox("�Է��� ������ ��ġ�ϴ� DB ������ ã�� ���߽��ϴ�.");
                        StopCamera();
                        break;
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
                cap->release();
                delete cap;
                cap = nullptr;
                if (cap == nullptr) {
                    AppendTextToOutputBox("ī�޶� �ʱ�ȭ �Ϸ�");
                };
            }

            cv::destroyAllWindows();

            this->statusLabel->Text = L"���� ����: ����";
            this->statusIndicator->BackColor = System::Drawing::Color::Red;
            this->statusIndicator->Invalidate();
        }
    }; // NAMESPACE
}
