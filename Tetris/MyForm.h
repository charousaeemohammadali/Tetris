#pragma once
#include"game.h"
#include<iostream>
#define PI 3.14159265358979323846
using namespace std;
namespace Tetris {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	const int field_width = 10;
	const int field_height = 20;
	const int blk_count = 4;
	int interval = 500;
	//Class to manage positions
	ref class Position {
	public:
		double x, y;
		Position() { x = 0.0; y = 0.0; }
		Position(double x, double y)
		{
			this->x = x;
			this->y = y;
		}
		Position(const Position% position)
		{
			this->x = position.x;
			this->y = position.y;
		}
	};
	//Class to manage blocks
	ref class Block {
		void change_blocks(Position^, Position^, Position^);
		int angle = 0;
	public:
		static Block^ controling;
		Position orig_position;
		Position center_position;
		cli::array<Position^>^ a_positions = gcnew cli::array<Position^>(blk_count);
		cli::array<Position^>^ rot_positions = gcnew cli::array<Position^>(blk_count);
		cli::array<Position^>^ abs_positions = gcnew cli::array<Position^>(blk_count);
		int type;
		Block(int type, Position position);
		property Position Orig_Position
		{
			void set(Position pos)
			{
				orig_position.x = pos.x;
				orig_position.y = pos.y;
				for (int i = 0; i < blk_count; i++)
					change_blocks(a_positions[i], abs_positions[i], %orig_position);
			}
			Position get()
			{
				return orig_position;
			}
		}
		bool Move(Position move);
		bool Rotate(int direction);
	};
	//Class to manage squares
	ref class Square {
		PictureBox^ box;
	public:
		Block^ block;
		Square(Control::ControlCollection^ controls, int x, int y, int width, int height)
		{
			box = gcnew PictureBox();
			box->Location = Point(x, y);
			box->Size = Size(width, height);
			Set_Block(nullptr);
			controls->Add(box);
		}
		void Set_Block(Block^);
	};
	//Static class to manage the game
	static ref class Game
	{
		static Random^ r;
		static Label^ label_l;
		static int score = 0;
		static void GameOver()
		{
			game_flag = false;
			Show_Label(" Game Over ! ");
		}
	public:
		static cli::array<Square ^, 2>^ squares;
		static bool game_flag = false;
		static void Start(Control::ControlCollection^ controls, Label^ label)
		{
			r = gcnew Random();
			label_l = label;
			squares = gcnew cli::array<Square^, 2>(field_width, field_height);
			for (int x = 0; x < field_width; x++)
				for (int y = 0; y < field_height; y++)
					squares[x, y] = gcnew Square(controls, 30 * x, 30 * y, 28, 28);
			game_flag = true;
		}
		//Add new block
		static bool Game::Add_Block()
		{
			int type = r->Next(1, 8);
			Position new_position(4, 1);
			Block^ Add_block = gcnew Block(type, new_position);
			//Game area check
			if (!Check_Field(Add_block->abs_positions, Add_block))
			{
				GameOver();
				return false;
			}
			for (int i = 0; i < blk_count; i++)
				squares[(int)Add_block->abs_positions[i]->x, (int)Add_block->abs_positions[i]->y]->Set_Block(Add_block);
			return true;
		}
		// Check colls with field walls and other blocks
		static bool Check_Field(cli::array<Position^>^ position, Block^ block)
		{
			for (int i = 0; i < blk_count; i++)
			{
				double x = position[i]->x;
				double y = position[i]->y;
				// Game area check
				if (x < 0.0 || x >= field_width || y < 0.0 || y >= field_height)
					return false;
				// Other block check
				if (Game::squares[(int)x, (int)y]->block != block && Game::squares[(int)x, (int)y]->block != nullptr)
					return false;
			}
			return true;
		}
		//Check if the line is filled
		static void Check_line()
		{
			for (int y = field_height - 1; y >= 0; y--)
			{
				int line_count = 0;
				for (int x = 0; x < field_width; x++)
				{
					if (squares[x, y]->block != nullptr)
						line_count++;
					else
						break;
				}
				if (line_count == field_width)
				{
					Game::game_flag = false;
					for (int x = 0; x < field_width; x++)
					{
						squares[x, y]->Set_Block(nullptr);
						Wait(10);
					}
					Game::Add_Score();
					Wait(100);
					for (int i = y; i >= 1; i--)
					{
						for (int x = 0; x < field_width; x++)
						{
							Block^ fall_blk = squares[x, i - 1]->block;
							squares[x, i]->Set_Block(fall_blk);
						}
					}
					y++;
					Game::game_flag = true;
				}
			}
		}
		// Check if own_block is accumulated
		static bool Check_Accumulate(Block^ own_block, cli::array<Position^>^ abs_positions)
		{
			for (int i = 0; i < blk_count; i++)
			{
				int x = (int)abs_positions[i]->x;
				int y = (int)abs_positions[i]->y;
				if (y == field_height - 1 || Game::squares[x, y + 1]->block != nullptr && Game::squares[x, y + 1]->block != own_block)
					return true;
			}
			return false;
		}
		static void Add_Score()
		{
			score += 100;
			Console::WriteLine("Score : " + score);
		}
		static void Show_Label(string text)
		{
			label_l->Text = gcnew String(text.c_str());
		}
		static void Wait(int milliseconds)
		{
			game_flag = false;
			Application::DoEvents();
			System::Threading::Thread::Sleep(milliseconds);
			game_flag = true;
		}
	};
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void) {
			InitializeComponent();
		}
	protected:
		~MyForm() {
			if (components) {
				delete components;
			}
		}
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Timer^  timer1;
#pragma region 
			 void InitializeComponent(void)
			 {
				 this->components = (gcnew System::ComponentModel::Container());
				 this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
				 this->label1 = (gcnew System::Windows::Forms::Label());
				 this->button1 = (gcnew System::Windows::Forms::Button());
				 this->SuspendLayout();
				 // 
				 // timer1
				 // 
				 this->timer1->Enabled = true;
				 this->timer1->Interval = 500;
				 this->timer1->Tick += gcnew System::EventHandler(this, &MyForm::timer1_Tick);
				 // 
				 // label1
				 // 
				 this->label1->AutoSize = true;
				 this->label1->Font = (gcnew System::Drawing::Font(L"ABC", 48, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
				 this->label1->Location = System::Drawing::Point(29, 447);
				 this->label1->Name = L"label1";
				 this->label1->Size = System::Drawing::Size(0, 112);
				 this->label1->TabIndex = 0;
				 // 
				 // button1
				 // 
				 this->button1->Font = (gcnew System::Drawing::Font(L"ABC", 9.857143F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
				 this->button1->Location = System::Drawing::Point(220, 447);
				 this->button1->Name = L"button1";
				 this->button1->Size = System::Drawing::Size(224, 75);
				 this->button1->TabIndex = 1;
				 this->button1->Text = L"Start the game";
				 this->button1->UseVisualStyleBackColor = true;
				 this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
				 // 
				 // MyForm
				 // 
				 this->AutoScaleDimensions = System::Drawing::SizeF(11, 21);
				 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				 this->ClientSize = System::Drawing::Size(685, 1404);
				 this->Controls->Add(this->button1);
				 this->Controls->Add(this->label1);
				 this->Name = L"MyForm";
				 this->Text = L"MyForm";
				 this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MyForm::MyForm_KeyDown);
				 this->ResumeLayout(false);
				 this->PerformLayout();
			 }
#pragma endregion
	private: System::Void MyForm_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
	{
		if (Game::game_flag)
		{
			Position move(0, 0);
			switch (e->KeyData)
			{
			case Keys::Up:
				move.y = -1;
				Block::controling->Move(move);
				break;
			case Keys::Down:
				move.y = 1;
				Block::controling->Move(move);
				break;
			case Keys::Left:
				move.x = -1;
				Block::controling->Move(move);
				break;
			case Keys::Right:
				move.x = 1;
				Block::controling->Move(move);
				break;
			case Keys::A:
				Block::controling->Rotate(-1);
				break;
			case Keys::D:
				Block::controling->Rotate(1);
				break;
			default:
				return;
			}
		}
	}
	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e)
	{
		if (Game::game_flag)
		{
			Position move(0, 1);
			Block::controling->Move(move);
		}
	}
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e)
	{
		button1->Hide();
		Game::Start(this->Controls, label1);
		Position pos(4, 1);
		Block^ block = gcnew Block(1, pos);
		Game::Add_Block();
		this->Focus();
	}
	};
}
