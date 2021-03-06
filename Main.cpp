
#include "GameConstants.h"
#include "cD3DManager.h"
#include "cD3DXSpriteMgr.h"
#include "cD3DXTexture.h"
#include "cEnemy.h"
#include "cD3DXFont.h"
#include "cSprite.h"

using namespace std;

HINSTANCE hInst; // global handle to hold the application instance
HWND wndHandle; // global variable to hold the window handle

// Get a reference to the DirectX Manager
static cD3DManager* d3dMgr = cD3DManager::getInstance();

// Get a reference to the DirectX Sprite renderer Manager 
static cD3DXSpriteMgr* d3dxSRMgr = cD3DXSpriteMgr::getInstance();
	
D3DXVECTOR2 playerTrans = D3DXVECTOR2(300,300);
D3DXVECTOR2 enemyTrans = D3DXVECTOR2(300,10);

vector<cEnemy*> aEnemy;
vector<cEnemy*>::iterator iter;
vector<cEnemy*>::iterator index;

RECT clientBounds;

cD3DXTexture* enemyTextures[4];
char* enemyTxture[] = {"Images\\Monster", "Images\\Monster2", "Images\\Monster3", "Images\\Monster4"};

float playerScore;

//Bool variables used to determine the current stae of the game
bool IntroState = true;
bool GameState = false;
bool EndState = false;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check any available messages from the queue
	switch (message)
	{
		case WM_KEYDOWN:
			{
				if(wParam == 'A' && IntroState == true) 
				{
					GameState = true;
					IntroState = false;
					return 0;
				}
				if (wParam == 'A' && GameState == true)
				{
					playerTrans.x -= 10.0f;
					return 0;
				}
				if (wParam == 'W' && GameState == true)
				{
					playerTrans.y -= 10.0f;
					return 0;
				}
					if (wParam == 'S' && GameState == true)
				{
					playerTrans.y += 10.0f;
					return 0;
				}
				if (wParam == 'D' && GameState == true)
				{
					playerTrans.x += 10.0f;
					return 0;
				}
				return 0;
			}
		case WM_CLOSE:
			{
			// Exit the Game
				PostQuitMessage(0);
				 return 0;
			}

		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool initWindow( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	// Fill in the WNDCLASSEX structure. This describes how the window
	// will look to the system
	wcex.cbSize = sizeof(WNDCLASSEX); // the size of the structure
	wcex.style = CS_HREDRAW | CS_VREDRAW; // the class style
	wcex.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
	wcex.cbClsExtra = 0; // extra bytes to allocate for this class
	wcex.cbWndExtra = 0; // extra bytes to allocate for this instance
	wcex.hInstance = hInstance; // handle to the application instance
	wcex.hIcon = LoadIcon(hInstance,"image\\DaggerIcon.ico"); // icon to associate with the application
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);// the default cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
	wcex.lpszMenuName = NULL; // the resource name for the menu
	wcex.lpszClassName = "RoguePit"; // the class name being created
	wcex.hIconSm = LoadIcon(hInstance,"DaggerIcon.ico"); // the handle to the small icon

	RegisterClassEx(&wcex);
	// Create the window
	wndHandle = CreateWindow("RoguePit",			// the window class to use
							 "Rogue Pit",			// the title bar text
							WS_OVERLAPPEDWINDOW,	// the window style
							CW_USEDEFAULT, // the starting x coordinate
							CW_USEDEFAULT, // the starting y coordinate
							1000, // the pixel width of the window
							750, // the pixel height of the window
							NULL, // the parent window; NULL for desktop
							NULL, // the menu for the application; NULL for none
							hInstance, // the handle to the application instance
							NULL); // no values passed to the window
	// Make sure that the window handle that is created is valid
	if (!wndHandle)
		return false;
	// Display the window on the screen
	ShowWindow(wndHandle, SW_SHOW);
	UpdateWindow(wndHandle);
	return true;
}

/*
==================================================================
// This is winmain, the main entry point for Windows applications
==================================================================
*/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	// Initialize the window
	if ( !initWindow( hInstance ) )
		return false;
	// called after creating the window
	if ( !d3dMgr->initD3DManager(wndHandle) )
		return false;
	if ( !d3dxSRMgr->initD3DXSpriteMgr(d3dMgr->getTheD3DDevice()))
		return false;

	// Grab the frequency of the high def timer
	__int64 freq = 0;				// measured in counts per second;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	float sPC = 1.0f / (float)freq;			// number of seconds per count

	__int64 currentTime = 0;				// current time measured in counts per second;
	__int64 previousTime = 0;				// previous time measured in counts per second;

	float numFrames   = 0.0f;				// Used to hold the number of frames
	float timeElapsed = 0.0f;				// cumulative elapsed time




	GetClientRect(wndHandle,&clientBounds);

	float fpsRate = 1.0f/25.0f;

	D3DXVECTOR3 aenemyPos;

	int numenemys = 10;

	for(int loop = 0; loop < numenemys; loop++)
	{
		aenemyPos = D3DXVECTOR3((float)clientBounds.right/(loop+2),(float)clientBounds.bottom/(loop+2),0);
		aEnemy.push_back(new cEnemy());
		aEnemy[loop]->setSpritePos(aenemyPos);
		aEnemy[loop]->setTranslation(D3DXVECTOR2(5.0f,0.0f));
		aEnemy[loop]->setTexture(enemyTextures[(loop % 3)]);
	}

	LPDIRECT3DSURFACE9 aSurface;				// the Direct3D surface
	LPDIRECT3DSURFACE9 theBackbuffer = NULL;  // This will hold the back buffer
	
	// Initial starting position for Player
	D3DXVECTOR3 playerPos = D3DXVECTOR3(300,300,0);
	cSprite thePlayer(playerPos,d3dMgr->getTheD3DDevice(),"Images\\Player.png");

	thePlayer.setTranslation(D3DXVECTOR2(5.0f,0.0f));

	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

	// Create the background surface
	// Depending on the current game state, different images for the surface are loaded
	if( IntroState == true)
	{
	aSurface = d3dMgr->getD3DSurfaceFromFile("Images\\IntroScreen.png");
	}

	if (GameState == true)
	{
		aSurface = d3dMgr->getD3DSurfaceFromFile("Images\\Level.png");

	// load custom font
	cD3DXFont* gameFont = new cD3DXFont(d3dMgr->getTheD3DDevice(),hInstance, "ZOMBIE");

	RECT textPos; //Creates a text box to display the player's score
	SetRect(&textPos, 50, 10, 400, 100);
	}

	while( msg.message!=WM_QUIT )
	{
		// Check the message queue
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// Game code goes here
			if(GameState == true)
			{
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			float dt = (currentTime - previousTime)*sPC;
			timeElapsed += dt; // Calculates amount of time elapsed since the player has started the game scene
			
			playerScore = timeElapsed;				// Copy the time elapsed to display as the players score

			/*
			Moves the enemy and then checks to see if it has collided with the outside of the level
			*/
			if(timeElapsed > fpsRate)
			{
				for(iter = aEnemy.begin(); iter != aEnemy.end(); ++iter)
				{
					(*iter)->update(timeElapsed);			// update enemy
					aenemyPos = (*iter)->getSpritePos();  // get the position of the current enemy
					/*
					Checks to see if the monsters have hit the edge of the area and then changes there direction if they have
					*/
					if (aenemyPos.x>(clientBounds.right - 60) || aenemyPos.x < 10 )
					{
						(*iter)->setTranslation((*iter)->getTranslation()*(-1));
					}

					if (aenemyPos.y>(clientBounds.bottom - 40) || aenemyPos.y < 10)
					{
						(*iter)->setTranslation((*iter)->getTranslation()*(-1));
					}

					}
				playerPos = D3DXVECTOR3(playerTrans.x,playerTrans.y,0);
				thePlayer.setSpritePos(playerPos);

			d3dMgr->beginRender();
			theBackbuffer = d3dMgr->getTheBackBuffer();
			d3dMgr->updateTheSurface(aSurface, theBackbuffer);
			d3dMgr->releaseTheBackbuffer(theBackbuffer);
				
			d3dxSRMgr->beginDraw();
			vector<cEnemy*>::iterator iterB = aEnemy.begin();
				for(iterB = aEnemy.begin(); iterB != aEnemy.end(); ++iterB)
				{
					d3dxSRMgr->setTheTransform((*iterB)->getSpriteTransformMatrix());  
					d3dxSRMgr->drawSprite((*iterB)->getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
				}

			d3dxSRMgr->drawSprite(thePlayer.getTexture(),NULL,NULL,&thePlayer.getSpritePos(),0xFFFFFFFF);
			d3dxSRMgr->endDraw();
			d3dMgr->endRender();

			}
		
			}
			previousTime = currentTime;
		}

	}
	d3dxSRMgr->cleanUp();
	d3dMgr->clean();
	return (int) msg.wParam;
}
