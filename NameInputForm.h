#pragma once

namespace Project1 {

    using namespace System;
    using namespace System::Windows::Forms;

    public ref class NameInputForm : public Form
    {
    public:
        TextBox^ nameTextBox;
        Button^ okButton;
        System::String^ name;

        NameInputForm()
        {
            InitializeComponent();
        }

        System::String^ GetName()
        {
            return nameTextBox->Text;
        }

    private:
        void InitializeComponent(void)
        {
            this->nameTextBox = gcnew TextBox();
            this->okButton = gcnew Button();


            this->nameTextBox->Location = System::Drawing::Point(20, 20);
            this->nameTextBox->Size = System::Drawing::Size(500, 300);


            this->okButton->Text = "OK";
            this->okButton->Location = System::Drawing::Point(80, 60);
            this->okButton->Click += gcnew System::EventHandler(this, &NameInputForm::okButton_Click);


            this->Controls->Add(this->nameTextBox);
            this->Controls->Add(this->okButton);
            this->Text = "기종 입력";
            this->Size = System::Drawing::Size(250, 150);
        }

        void okButton_Click(Object^ sender, EventArgs^ e)
        {
            name = nameTextBox->Text;
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }
    };
}