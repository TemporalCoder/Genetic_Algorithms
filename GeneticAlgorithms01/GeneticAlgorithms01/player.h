#pragma once

class player
{
public:
	//Life
	const static int numOfInst = 750;
	int instr[numOfInst];
	int currInst = 0;
	int distToGoal = 0;
	double fitness = 0;

	//POsition on screen
	int x = 5;
	int y = 5;
	bool jump = true;
	float jumpSpeed = 0;

	player()
	{
		//randomly initialise instructions
		for (int i = 0; i < numOfInst; i++)
		{
			//instr[i] = 0; //Born with the instinct to move right
			instr[i] = 3; //Sloth Mode - born to sit still
			//instr[i] = (rand() % 4); //Random genome
		}
		fitness = 0;
	}
	int operator == (const player& p1)
	{
		return instr == p1.instr;
	}
};
