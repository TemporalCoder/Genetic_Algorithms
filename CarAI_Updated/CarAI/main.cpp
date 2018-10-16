//-----------------------------------------------------------------------
//  
//------------------------------------------------------------------------
//#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <time.h>
#include <SDKDDKVer.h>

// Windows Header Files:

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <math.h>
#include <sstream>
#include <string>


#include <list>
#include <iostream>

using namespace std;

// MACROS /////////////////////////////////////////////////
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 400

//--------------------------------- Globals ------------------------------
//
//------------------------------------------------------------------------

//Forward Declaration of functions
void initGame();
void updateGame();
void drawGame();
void drawGame(HDC &hdc);
void drawFrame(HDC &hdc, HWND);
void getKeys();
void getAI();

//Variables
float playerx = WINDOW_WIDTH/2;
float playery = WINDOW_HEIGHT/2;
float px=15;
float py=0;
float playerRot = 0;

const int numOfInst = 300;
int instructions[numOfInst];
int instPtr = 0;


class thing
{
public: 
	float x;
	float y;
	float width;
	float height;

	thing()
	{
		x = 0; y = 0;
		width = 0; height = 0;
	}
	thing(int nx, int ny, int nwidth, int nheight)
	{
		x = nx; y = ny;
		width = nwidth; height = nheight;
	}
};


thing trevor(400, 200, 50, 50); //trevor cos paul named him!

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

		(HBRUSH)SelectObject(hdcBackBuffer, ltGreenBrush);

		Rectangle(hdcBackBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		drawGame(hdcBackBuffer);
		
		//now blit backbuffer to front
		BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdcBackBuffer, 0, 0, SRCCOPY);

		EndPaint(hwnd, &ps);

		//delay a little
		Sleep(20);

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
	winclass.lpszClassName = "GA_Car";
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//register the window class
	if (!RegisterClassEx(&winclass))
	{
		MessageBox(NULL, "Registration Failed!", "Error", 0);

		//exit the application
		return 0;
	}

	//create the window and assign its ID to hwnd    
	hWnd = CreateWindowEx(NULL,                 // extended style
		"GA_Car",  // window class name
		"GA Car",  // window caption
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
		MessageBox(NULL, "CreateWindowEx Failed!", "Error!", 0);
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

	UnregisterClass("Backbuffer", winclass.hInstance);

	return msg.wParam;
}

void initGame()
{
	//reset all variables
	playerx = 200	;
	playery = 200;

	for (int i = 0; i < numOfInst; i++)
	{
		instructions[i] = rand() % 4;
	}

}
void drawGame(HDC &hdcBackBuffer)
{
	//player
	//Rectangle(hdcBackBuffer, playerx, playery, playerx + 20, playery + 20);
	static HBRUSH ltBlueBrush = CreateSolidBrush(RGB(200, 200, 255));
	SelectObject(hdcBackBuffer, ltBlueBrush);

	Ellipse(hdcBackBuffer, playerx, playery, playerx + 20, playery + 20);
	//draw line/arrow
	MoveToEx(hdcBackBuffer, playerx+10, playery+10, NULL); //+10 is offset
	LineTo(hdcBackBuffer, playerx+10+px, playery+10+py);
	
	Rectangle(hdcBackBuffer, trevor.x, trevor.y, trevor.x + trevor.width, trevor.y + trevor.height);


	//Draw text
	LPCSTR buffer = "AICar: ";
	TextOut(hdcBackBuffer, 5, 5, buffer, lstrlen(buffer));

	TCHAR buf[5];
	snprintf(buf, 5, "%f", playerRot);
	TextOut(hdcBackBuffer, 100, 5, buf, 5);

}


void updateGame()
{
	getKeys();
	//getAI();

	//check collision
	if (playerx > trevor.x && playerx < trevor.x + trevor.width)
	{
		if (playery > trevor.y && playery < trevor.y + trevor.height)
		{
			MessageBox(NULL, "STOP", "at goal", 0);
		}
	}
}

void getAI()
{
	int currentInstruction = instructions[instPtr];
	instPtr++;
	if (instPtr > numOfInst) { instPtr = 0; }

	if (currentInstruction==2)
	{
		//playerx++;
		playerRot += 0.1f;
		//2d point rotation - Radians not degrees
		px = 15 * cos(playerRot) - (0 * sin(playerRot));
		py = 15 * sin(playerRot) + (0 * cos(playerRot));

	}
	if (currentInstruction ==0)
	{
		//playerx--;
		playerRot -= 0.1f;
		px = 15 * cos(playerRot) - (0 * sin(playerRot));
		py = 15 * sin(playerRot) + (0 * cos(playerRot));

	}

	if (currentInstruction ==1)
	{
		//translate 2d point
		//newx = oldx + (velocity*cos(angle))
		//newy = oldy + (velocity*sin(angle))
		float x = playerx + (2 * cos(playerRot));
		float y = playery + (2 * sin(playerRot));

		playerx = x;
		playery = y;
	}
	if (currentInstruction==3)
	{
		float x = playerx + (-2 * cos(playerRot));
		float y = playery + (-2 * sin(playerRot));

		playerx = x;
		playery = y;
	}

}

//Keyboard input
void getKeys()
{
	
	if (KEYDOWN(VK_RIGHT) && playerx<600)
	{
		//playerx++;
		playerRot+=0.1f;
		//2d point rotation - Radians not degrees
		px = 15 * cos(playerRot) - (0 * sin(playerRot));
		py = 15 * sin(playerRot) + (0 * cos(playerRot));

	}
	if (KEYDOWN(VK_LEFT) && playerx>0)
	{
		//playerx--;
		playerRot -= 0.1f;
		px = 15 * cos(playerRot) - (0 * sin(playerRot));
		py = 15 * sin(playerRot) + (0 * cos(playerRot));

	}

	if (KEYDOWN(VK_UP) && playery>0)
	{
		//translate 2d point
		//newx = oldx + (velocity*cos(angle))
		//newy = oldy + (velocity*sin(angle))
		float x = playerx + (2 * cos(playerRot));
		float y = playery + (2 * sin(playerRot));

		playerx = x;
		playery = y;
	}
	if (KEYDOWN(VK_DOWN) && playery<600)
	{
		playery++;
	}

	if (KEYDOWN(VK_SPACE))
	{
	}
	if (KEYDOWN(VK_RETURN))
	{
		initGame();
	}
	// https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
	if (KEYDOWN(0x52)) //'R'
	{
		initGame();
	}

	if (KEYDOWN(0x53)) //'S'
	{
	}
}
