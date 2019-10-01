#pragma once

namespace GACarClr {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	//Global Variables
	int x = 100;
	int y = 100;


	/// <summary>
	/// Summary for MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::PictureBox^ pictureBox1;
	protected:
	private: System::Windows::Forms::Timer^ timer1;

	private: System::ComponentModel::IContainer^ components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::SystemColors::Info;
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox1->Location = System::Drawing::Point(12, 28);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(699, 403);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MainForm::PictureBox1_Paint);
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 30;
			this->timer1->Tick += gcnew System::EventHandler(this, &MainForm::Timer1_Tick);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->ClientSize = System::Drawing::Size(831, 527);
			this->Controls->Add(this->pictureBox1);
			this->KeyPreview = true;
			this->Name = L"MainForm";
			this->Text = L"GACar";
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MainForm::MainForm_KeyDown);
			this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MainForm::MainForm_KeyPress);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		//KEY PRESS EVENT
		private: System::Void MainForm_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e) 
		{
					
		}

		private: System::Void Timer1_Tick(System::Object^ sender, System::EventArgs^ e) 
		{
			Refresh();
		}

		private: System::Void MainForm_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) 
		{
			//MessageBox::Show("Key: " + e->KeyChar.ToString());

			if (e->KeyCode == Keys::Left)
			{
				x -= 10;
			}
			if (e->KeyCode == Keys::Right)
			{
				x += 10;
			}

			if (e->KeyCode == Keys::Enter)
			{

			}
		}
		private: System::Void PictureBox1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e)
		{
			//Draw Canvas
			Graphics^ g = e->Graphics;
			g->FillEllipse(Brushes::Blue, x, y, 25, 25);
			g->DrawEllipse(Pens::Black, x, y, 25, 25);

			g->DrawLine(Pens::Black, x+12, y+12, x+12, y + 30);
		}

};
}
