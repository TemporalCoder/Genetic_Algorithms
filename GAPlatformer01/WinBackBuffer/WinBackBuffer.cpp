//-----------------------------------------------------------------------
//  
//  Name: GDI_Backbuffer2 example project  
//  Author: Mat Buckland 2002 - 
//  
//	Edited 2018 by D Parr to demonstrate G.A. Platform Game 
//
//------------------------------------------------------------------------
#include "stdafx.h"
#include <windows.h>
#include <time.h>

#include "defines.h"
#include "utils.h"
#include <list>

#include <iostream>
using namespace std;

// MACROS /////////////////////////////////////////////////
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)


//--------------------------------- Globals ------------------------------
//
//------------------------------------------------------------------------


//Forward Declaration of functions
void updateGame();
void drawGame();
void drawFrame(HDC &hdc, HWND);
void getKeys();
void getPopAI(); //get population ai

int tx = 0;
int ty = 0;

const int mapRow = 6;
const int mapCol = 24;

int map[mapRow][mapCol] =
{
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,2 },
	{ 1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,0 },
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 }
};

const int numOfInst = 750;
int currInst = 0;

wchar_t* text = {0};	//for the text out


#define CROSSOVER_RATE	0.7

//#define MUTATION_RATE	0.005			//const mutation rate
float MUTATION_RATE = 0.005;			//User definable
const int chromoLength = numOfInst;			//Number of Instructions
const int population = 20;

bool gaSuccess = false;
int genCount = 0;
bool success = true;	//have he plants reachec the sun?


class player
{
	public:
		//Life
		const static int numOfInst = chromoLength;
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
			for(int i = 0; i < numOfInst; i++)
			{
				//instr[i] = 0; //born with the instinct to move right
				instr[i] = 3; //SLoth Mode - born to sit still
				//instr[i] = (rand() % 4); //random genome
			}
			fitness = 0;
		}
		int operator == (const player &p1)
		{
			return instr == p1.instr;
		}
};

void initGame();
void drawGame();



player players[population];
int generation;
float totalFitness;

void evolve();
void decode(player); //Not in use yet

player& selection();
void crossOver(player&, player&, player&, player&);
void mutate(player&);
void updateFitness();


//---------------------------- WindowProc ---------------------------------
//	
//	This is the callback function which handles all the windows messages
//-------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND   hwnd,
	UINT   msg,
	WPARAM wParam,
	LPARAM lParam)
{
	//create some pens to use for drawing
	static HPEN BluePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	static HPEN RedPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	static HPEN OldPen = NULL;

	//create a solid brush
	static HBRUSH RedBrush = CreateSolidBrush(RGB(200, 128, 0));
	static HBRUSH YellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	static HBRUSH ltBlueBrush = CreateSolidBrush(RGB(200, 200, 255));
	static HBRUSH ltGreenBrush = CreateSolidBrush(RGB(50, 255, 255));

	static HBRUSH OldBrush = NULL;

	//these hold the dimensions of the client window area
	static int cxClient, cyClient;

	
	//used to create the back buffer
	static HDC		hdcBackBuffer;
	static HBITMAP	hBitmap;
	static HBITMAP	hOldBitmap;

	switch (msg)
	{

		//A WM_CREATE msg is sent when your application window is first
		//created
	case WM_CREATE:
	{
		//to get get the size of the client window first we need  to create
		//a RECT and then ask Windows to fill in our RECT structure with
		//the client window size. Then we assign to cxClient and cyClient 
		//accordingly
		RECT rect;

		GetClientRect(hwnd, &rect);

		cxClient = rect.right;
		cyClient = rect.bottom;

		//seed random number generator
		srand((unsigned)time(NULL));

		//---------------create a surface for us to render to(backbuffer)

		//create a memory device context
		hdcBackBuffer = CreateCompatibleDC(NULL);

		//get the DC for the front buffer
		HDC hdc = GetDC(hwnd);

		hBitmap = CreateCompatibleBitmap(hdc,
			cxClient,
			cyClient);


		//select the bitmap into the memory device context
		hOldBitmap = (HBITMAP)SelectObject(hdcBackBuffer, hBitmap);

		//don't forget to release the DC
		ReleaseDC(hwnd, hdc);

		initGame();
	}

	break;

	case WM_KEYUP:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			PostQuitMessage(0);
		}

		break;
		}
	}

	case WM_PAINT:
	{

		PAINTSTRUCT ps;

		BeginPaint(hwnd, &ps);

		//fill our backbuffer with white
		BitBlt(hdcBackBuffer,
			0,
			0,
			cxClient,
			cyClient,
			NULL,
			NULL,
			NULL,
			WHITENESS);

		(HBRUSH)SelectObject(hdcBackBuffer, ltBlueBrush);

		Rectangle(hdcBackBuffer, 0, 0, 600, 120);
		
		
		//first select a pen to draw with and store a copy
		//of the pen we are swapping it with
		
		OldPen = (HPEN)SelectObject(hdcBackBuffer, BluePen);

		//do the same for our brush
		OldBrush = (HBRUSH)SelectObject(hdcBackBuffer, RedBrush);


		//Draw Platforms - MOVE!!
		for (int row = 0; row < mapRow; row++)
		{
			for (int col = 0; col < mapCol; col++)
			{
				if (map[row][col] == 1)
				{
					Rectangle(hdcBackBuffer, col * 20, row * 20, col * 20 + 20, row * 20 + 20);
				}
				if (map[row][col] == 2)
				{
					(HBRUSH)SelectObject(hdcBackBuffer, ltGreenBrush);
					Rectangle(hdcBackBuffer, col * 20, row * 20, col * 20 + 20, row * 20 + 20);
					OldBrush = (HBRUSH)SelectObject(hdcBackBuffer, RedBrush);

				}
			}
		}

		SelectObject(hdcBackBuffer, RedPen);
		SelectObject(hdcBackBuffer, YellowBrush);

		//Draw population
		for (int p = 0; p < population; p++)
		{
			Rectangle(hdcBackBuffer, players[p].x, players[p].y, players[p].x + 6, players[p].y + 6);
		}


		//replace the original pen
		SelectObject(hdcBackBuffer, OldPen);
		//and brush
		SelectObject(hdcBackBuffer, OldBrush);

		
		LPCWSTR buffer = L"Generation: ";
		wchar_t buffer2[5];
		
		TextOut(hdcBackBuffer, 5, 5, buffer, wcslen(buffer) ) ;
		
		wsprintfW(buffer2, L"%d", genCount);
		TextOut(hdcBackBuffer, 100, 5, buffer2, wcslen(buffer2));

		wsprintfW(buffer2, L"%d", (chromoLength-currInst));
		TextOut(hdcBackBuffer, 150, 5, buffer2, wcslen(buffer2));


		//now blit backbuffer to front
		BitBlt(ps.hdc, 0, 100, cxClient, cyClient, hdcBackBuffer, 0, 0, SRCCOPY);

		EndPaint(hwnd, &ps);

		//delay a little
		Sleep(2);

	}

	break;

	//has the user resized the client area?
	case WM_SIZE:
	{
		//if so we need to update our variables so that any drawing
		//we do using cxClient and cyClient is scaled accordingly
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		//now to resize the backbuffer accordingly. First select
		//the old bitmap back into the DC
		SelectObject(hdcBackBuffer, hOldBitmap);

		//don't forget to do this or you will get resource leaks
		DeleteObject(hBitmap);

		//get the DC for the application
		HDC hdc = GetDC(hwnd);

		//create another bitmap of the same size and mode
		//as the application
		hBitmap = CreateCompatibleBitmap(hdc,
			cxClient,
			cyClient);

		ReleaseDC(hwnd, hdc);

		//select the new bitmap into the DC
		SelectObject(hdcBackBuffer, hBitmap);

	}

	break;

	case WM_DESTROY:
	{
		//delete the pens        
		DeleteObject(BluePen);
		DeleteObject(OldPen);

		//and the brushes
		DeleteObject(RedBrush);
		DeleteObject(OldBrush);


		//clean up our backbuffer objects
		SelectObject(hdcBackBuffer, hOldBitmap);

		DeleteDC(hdcBackBuffer);
		DeleteObject(hBitmap);

		// kill the application, this sends a WM_QUIT message  
		PostQuitMessage(0);
	}

	break;

	}//end switch

	 //this is where all the messages not specifically handled by our 
	 //winproc are sent to be processed
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//-------------------------------- WinMain -------------------------------
//
//	The entry point of the windows program
//------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     szCmdLine,
	int       iCmdShow)
{
	//handle to our window
	HWND						hWnd;

	//our window class structure
	WNDCLASSEX     winclass;

	// first fill in the window class stucture
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = NULL;
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = L"GA_Platformer";
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//register the window class
	if (!RegisterClassEx(&winclass))
	{
		MessageBox(NULL, L"Registration Failed!", L"Error", 0);

		//exit the application
		return 0;
	}

	//create the window and assign its ID to hwnd    
	hWnd = CreateWindowEx(NULL,                 // extended style
		L"GA_Platformer",  // window class name
		L"GA Platformer",  // window caption
		WS_OVERLAPPEDWINDOW,  // window style
		0,                    // initial x position
		0,                    // initial y position
		WINDOW_WIDTH,         // initial x size
		WINDOW_HEIGHT,        // initial y size
		NULL,                 // parent window handle
		NULL,                 // window menu handle
		hInstance,            // program instance handle
		NULL);                // creation parameters

							  //make sure the window creation has gone OK
	if (!hWnd)
	{
		MessageBox(NULL, L"CreateWindowEx Failed!", L"Error!", 0);
	}

	//make the window visible
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	// Enter the message loop
	bool bDone = false;

	MSG msg;

	while (!bDone)
	{

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				// Stop loop if it's a quit message
				bDone = true;
			}

			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		//this will call WM_PAINT which will render our scene
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);

		//*** your game loop goes here ***//
		updateGame();

	}//end while

	UnregisterClass(L"Backbuffer", winclass.hInstance);

	return msg.wParam;
}

void initGame()
{
	
}
void drawGame()
{

}




void updateGame()
{
	
	if (gaSuccess == false)
	{
		getPopAI();
	}
	
	//AI Population
	for (int p = 0;p < population; p++)
	{
		//check tile under character
		tx = players[p].x / 20;
		ty = (players[p].y + 6) / 20;
		int currTile = map[ty][tx];

		//check at goal?
		if (currTile == 2)
		{
			gaSuccess = true;
			MessageBox(0, L"Success", L"Success", MB_OK);
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

//Keyboard input
void getKeys()
{
	
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
			players[p].currInst =0;
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
			if (currTile == 0 && players[p].x<mapCol*20) { players[p].x += 2; }
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
	while (newPop<population)
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

	for (int i = 0; i<population; i++)
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

	for (int i = 0; i<population; i++)
	{
		//plant fitness between 0 and 1 based on height. Max 290 units above pot.
		//players[i].fitness = ((100 / maxHeight)*players[i].height) / 100;

		/*
		float diffx = abs(players[i].x - 480);
		float diffy = abs(players[i].y - 20);
		double fit = 1 / (diffx + diffy + 1);
		*/
		double fit = 1 / (double)(480 - players[i].x + 1);

		players[i].fitness = fit;

		totalFitness += fit;
	}
}


//-------------
//Selection
//------------

player& selection() 
{
	float fSlice = (rand()) / (RAND_MAX + 1.0)* totalFitness;
	//float fSlice = (rand()* totalFitness);
	float total = 0;
	int selectedPlayer = 0;

	for (int i = 0; i<population; i++)
	{
		total += players[i].fitness;

		if (total>fSlice)
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
void crossOver(player &mum, player &dad, player &baby1, player &baby2)
{
	if (((rand()) / (RAND_MAX + 1.0)>CROSSOVER_RATE) || mum == dad)
	{
		baby1 = mum;
		baby2 = dad;

		return;
	}

	int xover = rand() % chromoLength;

	for (int i = 0; i<xover; i++)
	{
		baby1.instr[i] = mum.instr[i];
		baby2.instr[i] = dad.instr[i];
	}

	for (int i = xover; i<chromoLength; i++)
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
	for (int bit = 0; bit<chromoLength; bit++)
	{
		if (((rand()) / (RAND_MAX + 1.0)<MUTATION_RATE))
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
