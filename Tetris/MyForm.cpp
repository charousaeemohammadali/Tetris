#include "MyForm.h"
using namespace Tetris;

int main()
{
	MyForm ^ form = gcnew MyForm();
	form->ShowDialog();
	return 0;
}
//manage blocks
Block::Block(int type, Position position)
{
	//bedune khate zir = error dar move
	Block::controling = this;
	this->type = type;
	orig_position.x = position.x;
	orig_position.y = position.y;
	center_position.x = 0;
	center_position.y = 0;
	cli::array<Position^, 2>^ blocks = gcnew cli::array<Position^, 2>
	{
		{ gcnew Position(-1, 0), gcnew Position(0, 0), gcnew Position(1, 0), gcnew Position(2, 0)},
		{ gcnew Position(-1, -1), gcnew Position(-1, 0), gcnew Position(0, 0), gcnew Position(1, 0) },
		{ gcnew Position(-1,0), gcnew Position(0,0), gcnew Position(1,0), gcnew Position(1,-1) },
		{ gcnew Position(-1,0),gcnew Position(0,0),gcnew Position(0,-1),gcnew Position(1,-1) },
		{ gcnew Position(-1,-1),gcnew Position(0,-1),gcnew Position(0,0),gcnew Position(1,0) },
		{ gcnew Position(-1,0),gcnew Position(0,0),gcnew Position(1,0),gcnew Position(0,-1) },
		{ gcnew Position(-0.5,-0.5),gcnew Position(-0.5,0.5),gcnew Position(0.5,-0.5),gcnew Position(0.5,0.5) }
	};
	for (int i = 0; i < blk_count; i++)
	{
		//index bound = [type -1]
		a_positions[i] = blocks[type - 1, i];
		rot_positions[i] = blocks[type - 1, i];
		abs_positions[i] = gcnew Position(0, 0);
		change_blocks(rot_positions[i], abs_positions[i], %orig_position);
	}
}
void Block::change_blocks(Position^ rot_pos, Position^ result_pos, Position^ org_position)
{
	result_pos->x = org_position->x + rot_pos->x;
	result_pos->y = org_position->y + rot_pos->y;
}
bool Block::Move(Position move)
{
	if (Game::game_flag)
	{
		Position new_pos(orig_position.x + move.x, orig_position.y + move.y);
		cli::array<Position^>^ rot = gcnew cli::array<Position^>(blk_count);
		cli::array<Position^>^ abses = gcnew cli::array<Position^>(blk_count);
		for (int i = 0; i < blk_count; i++)
		{
			rot[i] = rot_positions[i];
			abses[i] = gcnew Position();
			change_blocks(rot[i], abses[i], %new_pos);
		}
		if (Game::Check_Field(abses, this))
		{
			for (int i = 0; i < blk_count; i++)
				Game::squares[(int)abs_positions[i]->x, (int)abs_positions[i]->y]->Set_Block(nullptr);
			for (int i = 0; i < blk_count; i++)
			{
				orig_position.x = new_pos.x;
				orig_position.y = new_pos.y;
				abs_positions[i]->x = abses[i]->x;
				abs_positions[i]->y = abses[i]->y;
				Game::squares[(int)abs_positions[i]->x, (int)abs_positions[i]->y]->Set_Block(this);
			}
			return true;
		}
		else
		{
			if (Game::Check_Accumulate(this, abs_positions))
			{
				Game::Check_line();
				Game::Wait(100);
				Game::Add_Block();
			}
			return false;
		}
	}
	else
		return false;
}
bool Block::Rotate(int direction)
{
	if (Game::game_flag)
	{
		cli::array<Position^>^ new_rot = gcnew cli::array<Position^>(blk_count);
		cli::array<Position^>^ new_abs = gcnew cli::array<Position^>(blk_count);
		int sin_c = (int)sin(PI / 2.0 * direction);
		int cos_c = (int)cos(PI / 2.0 * direction);
		for (int i = 0; i < blk_count; i++)
		{
			double x = a_positions[i]->x;
			double y = a_positions[i]->y;
			new_rot[i] = gcnew Position((x * cos_c) - (y * sin_c), (x * sin_c) + (y * cos_c));
			new_abs[i] = gcnew Position(0, 0);
			change_blocks(new_rot[i], new_abs[i], %orig_position);
		}
		if (Game::Check_Field(new_abs, this))
		{
			angle += direction;
			for (int i = 0; i < blk_count; i++)
			{
				Game::squares[(int)abs_positions[i]->x, (int)abs_positions[i]->y]->Set_Block(nullptr);
			}
			for (int i = 0; i < blk_count; i++)
			{
				rot_positions[i]->x = new_rot[i]->x;
				rot_positions[i]->y = new_rot[i]->y;
				abs_positions[i]->x = new_abs[i]->x;
				abs_positions[i]->y = new_abs[i]->y;
				Game::squares[(int)abs_positions[i]->x, (int)abs_positions[i]->y]->Set_Block(this);
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
//Class to manage squares
void Square::Set_Block(Block^ block)
{
	this->block = block;
	cli::array<System::Drawing::Color>^ colors = gcnew cli::array<System::Drawing::Color>
	{
		System::Drawing::Color::FromArgb(180, 180, 180),
			System::Drawing::Color::FromArgb(0, 255, 255),
			System::Drawing::Color::FromArgb(0, 0, 255),
			System::Drawing::Color::FromArgb(255, 102, 0),
			System::Drawing::Color::FromArgb(0, 255, 0),
			System::Drawing::Color::FromArgb(255, 0, 0),
			System::Drawing::Color::FromArgb(128, 0, 128),
			System::Drawing::Color::FromArgb(255, 255, 0)
	};
	if (this->block == nullptr)
		box->BackColor = colors[0];
	else
		box->BackColor = colors[this->block->type];
}
