#pragma once
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "player.h";


namespace GeneticAlgorithms01 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	
	//Forward Declaration of functions
	void updateGame();
	void drawGame();
	void getPopAI(); //get population ai

	
	//Very Global Variables
	int tx = 0; //Tiles x and y
	int ty = 0;

	static const int mapRow = 6;
	static const int mapCol = 24;

	//'Game Map'
	int map[mapRow][mapCol] =
	{
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,2 },
		{ 1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 }
	};

	
	const int numOfInst = 750; //needs to be the same as player class!! 
	int currInst = 0;

	#define CROSSOVER_RATE	0.7

	//#define MUTATION_RATE	0.005			//const mutation rate
	float MUTATION_RATE = 0.005f;			//User definable
	const int chromoLength = numOfInst;		//Number of Instructions
	const int population = 20;

	bool gaSuccess = false;
	int genCount = 0;
	bool success = false;	//has a player arrived at the target

		
	player players[population];
	int generation;
	float totalFitness;

	void evolve();
	void decode(player); //Not in use yet

	player& selection();
	void crossOver(player&, player&, player&, player&);
	void mutate(player&);
	void updateFitness();
	


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
	private: System::Windows::Forms::Timer^ timer1;
	private: System::ComponentModel::IContainer^ components;
	protected:

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
			this->pictureBox1->BackColor = System::Drawing::Color::LightBlue;
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox1->Location = System::Drawing::Point(25, 12);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(673, 168);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MainForm::PictureBox1_Paint);
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 1;
			this->timer1->Tick += gcnew System::EventHandler(this, &MainForm::Timer1_Tick);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->ClientSize = System::Drawing::Size(849, 429);
			this->Controls->Add(this->pictureBox1);
			this->Name = L"MainForm";
			this->Text = L"Platform Evolution";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
		//---
		//Draw Game/Simulation
		private: System::Void PictureBox1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) 
		{

			Graphics^ g = e->Graphics;
			
			//Draw Platforms - MOVE!!
			for (int row = 0; row < mapRow; row++)
			{
				for (int col = 0; col < mapCol; col++)
				{
					if (map[row][col] == 1)
					{
						g->FillRectangle(Brushes::Goldenrod, col * 20, row * 20, 20, 20);
						g->DrawRectangle(Pens::Black, col * 20, row * 20, 20, 20);

					}
					if (map[row][col] == 2)
					{
						g->FillRectangle(Brushes::LightGreen, col * 20, row * 20, 20, 20);
						g->DrawRectangle(Pens::Black, col * 20, row * 20, 20, 20);
					}
				}
			}

			//Draw population
			for (int p = 0; p < population; p++)
			{
				g->FillRectangle(Brushes::Yellow, players[p].x, players[p].y,6,6);
				g->DrawRectangle(Pens::Black, players[p].x, players[p].y, 6, 6);
			}

		}
		private: System::Void Timer1_Tick(System::Object^ sender, System::EventArgs^ e) 
		{
			updateGame();
			Refresh();
			if (success == true)
			{
				timer1->Enabled = false;
			}

			
		}
	};  //End of Main Form Class

	
//GA Functions - Move to MainForm.cpp!
	void updateGame()
	{
		if (gaSuccess == false)
		{
			getPopAI();
		}

		//AI Population
		for (int p = 0; p < population; p++)
		{
			//check tile under character *under not below, layers not height
			tx = players[p].x / 20;
			ty = (players[p].y + 6) / 20;
			int currTile = map[ty][tx];

			//check at goal?
			if (currTile == 2)
			{
				gaSuccess = true;				
				//MessageBox::Show("Success!");
				MessageBox::Show("Success", "Evolution Achieved",MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				success = true;//Stop
			}

			if (currTile == 0)
			{
				players[p].jump = true;
			}


			if (players[p].jump == true)
			{
				players[p].jumpSpeed -= 0.3f;
				players[p].y -= players[p].jumpSpeed;

				//falling		
				if (players[p].jumpSpeed < 0 && currTile == 1)
				{
					players[p].y = (ty * 20) - 6;
					players[p].jumpSpeed = 0;
					players[p].jump = false;
				}
			}
		}
	}


	void getPopAI()
	{
		//Update Instruction counter for display
		if (currInst < numOfInst)
		{
			currInst++;
		}

		//check for new generation
		if (players[0].currInst >= chromoLength)
		{
			evolve();
			genCount++;
			for (int p = 0; p < population; p++)
			{
				players[p].x = 5;
				players[p].y = 5;
				players[p].currInst = 0;
			}
			currInst = 0;
		}

		for (int p = 0; p < population; p++)
		{
			//get instruction
			int inst = players[p].instr[players[p].currInst];
			if (players[p].currInst < players[p].numOfInst)
			{
				players[p].currInst++;
			}
			else
			{
				players[p].currInst = 0;
			}


			//right
			if (inst == 0)
			{
				tx = (players[p].x + 8) / 20;
				ty = (players[p].y) / 20;
				int currTile = map[ty][tx];
				if (currTile == 0 && players[p].x < mapCol * 20) { players[p].x += 2; }
			}

			//left
			if (inst == 1)
			{
				tx = (players[p].x - 2) / 20;
				ty = (players[p].y) / 20;
				int currTile = map[ty][tx];
				if (currTile == 0 && players[p].x > 0) { players[p].x -= 2; }
			}

			//up
			if (inst == 2)
			{
				if (players[p].jump == false)
				{
					players[p].jump = true;
					players[p].jumpSpeed = 5;
				}
			}
			//down
			if (inst == 3)
			{
				//do nothing...
			}
		}
	}

	
	//--------------
	//EVOLVE 
	//--------------

	void evolve()
	{
		updateFitness();

		player babyPlayers[population];

		int newPop = 0;
		while (newPop < population)
		{
			player mum = selection();
			player dad = selection();

			player baby1, baby2;
			crossOver(mum, dad, baby1, baby2);

			//mutate
			mutate(baby1);
			mutate(baby2);

			//copy babys into babypopulation
			babyPlayers[newPop] = baby1;
			babyPlayers[newPop + 1] = baby2;
			newPop += 2;
		}

		for (int i = 0; i < population; i++)
		{
			players[i] = babyPlayers[i];
		}

	}
	void decode(player)
	{
		//convert instructions to something usable  
		//not needed? 
	}


	//-------------------
	// Update Fitness
	//-------------------
	void updateFitness()
	{
		totalFitness = 0;

		for (int i = 0; i < population; i++)
		{
			
			int successValue = 480; //value of x coordinate 
			double fit = 1 / (double)(successValue - players[i].x + 1);

			players[i].fitness = fit;

			totalFitness += fit;
		}
	}


	//-------------
	//Selection
	//------------

	player& selection()
	{
		float fSlice = (rand()) / (RAND_MAX + 1.0) * totalFitness;
		//float fSlice = (rand()* totalFitness);
		float total = 0;
		int selectedPlayer = 0;

		for (int i = 0; i < population; i++)
		{
			total += players[i].fitness;

			if (total > fSlice)
			{
				selectedPlayer = i;
				break;
			}
		}
		return players[selectedPlayer];
	}

	//-----------------
	//Crossover
	//-------------------
	void crossOver(player& mum, player& dad, player& baby1, player& baby2)
	{
		if (((rand()) / (RAND_MAX + 1.0) > CROSSOVER_RATE) || mum == dad)
		{
			baby1 = mum;
			baby2 = dad;

			return;
		}

		int xover = rand() % chromoLength;

		for (int i = 0; i < xover; i++)
		{
			baby1.instr[i] = mum.instr[i];
			baby2.instr[i] = dad.instr[i];
		}

		for (int i = xover; i < chromoLength; i++)
		{
			baby1.instr[i] = dad.instr[i];
			baby2.instr[i] = mum.instr[i];
		}
	}

	//------------
	//MUTATE
	//------------
	void mutate(player& currPlayer)
	{
		for (int bit = 0; bit < chromoLength; bit++)
		{
			if (((rand()) / (RAND_MAX + 1.0) < MUTATION_RATE))
			{
				//original
				//currPlayer.instr[bit] = (rand() % 4);//!currPlayer.instr[bit];

				//new - prevents same instruction from being chosen. 
				int newInst = 0;
				do
				{
					newInst = (rand() % 4);
					//if (newInst == 1) { newInst = 2; }
				} while (newInst == currPlayer.instr[bit]);
				currPlayer.instr[bit] = newInst;

			}
		}
	}

	
}//End of Namespace
