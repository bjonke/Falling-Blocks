//////////////////////////////////////////////////////////////////////////////////
// Project: Falling Blocks (Tetris)
// File:    Main.cpp
//////////////////////////////////////////////////////////////////////////////////

// These three lines link in the required SDL components for our project. //
// Alternatively, we could have linked them in our project settings.      //
// -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer                            //
//#pragma comment(lib, "SDL.lib")
//#pragma comment(lib, "SDLmain.lib")
//#pragma comment(lib, "SDL_TTF.lib")


#include <iostream>  // For console output mostly
#include <stack>     // We'll use the STL stack to store our function pointers
#include <vector>    // An STL vector will store the squares that are not part of the focus block
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <chrono>
#include <ratio>
#include <cmath>    // We'll be using the abs() function located in "math.h"
#include <ctime>    // We use time(), located in "time.h", to seed our random generator

// SDL2 Specific include files used in the project
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>


#include "Defines.h" // Our defines header
#include "Enums.h"   // Our enums header
#include "cBlock.h"  // Contains the class that represents a game block

using namespace std;
using namespace std::chrono;

// Global data //
struct StateStruct;
//Volume control type: 1 = SDL (only affects Mupen64Plus output)  2 = OSS
// mixer (adjusts master PC volume)
int VOLUME_CONTROL_TYPE = 1;
// Percentage change each time the volume is increased or decreased
int VOLUME_ADJUST = 1;
// Default volume when a game is started.Only used if VOLUME_CONTROL_TYPE is 1
int VOLUME_DEFAULT = 0;
// ghost piece showing =1 or 0 for not showing
int GHOST_PIECE = 1;
// POINTS PER LINE
int PointsPerLine;
// POINTS PER LEVEL
int PointsPerLevel;
// INITIAL SPEED
int InitialSpeed;
// SPEED CHANGE
int SpeedChange;
// SLIDE TIME
int SlideTime;

std::vector<int> ConfigData;

std::stack<StateStruct> g_StateStack;        // Our state stack
std::deque<std::string> g_StateQueue;        // Our state stack

SDL_Surface*       g_Bitmap = NULL;     // Our back and squares bitmap
SDL_Surface*       g_Window = NULL;     // Our backbuffer
SDL_Event		   g_Event;             // An SDL event structure for input
int				   g_Timer = 0;             // Our timer is just an integer
cBlock*			   g_FocusBlock = NULL; // The block the player is controlling
cBlock*			   g_NextBlock  = NULL; // The next block to be the focus block

std::vector<cSquare*>   g_OldSquares;        // The squares that no longer form the focus block

int				   g_Score = 0;         // Players current score
int				   g_Level = 1;         // Current level player is on
int				   g_FocusBlockSpeed = INITIAL_SPEED; // Speed of the focus block
int                g_Lines = 0; // Players amount of lines removed

SDL_Renderer * renderer;
SDL_Window * window;

TTF_Font* font;

SDL_Texture* solidTexture;
SDL_Texture* blendedTexture;
SDL_Texture* shadedTexture;
SDL_Texture* Message;

SDL_Rect solidRect;
SDL_Rect blendedRect;
SDL_Rect shadedRect;
SDL_Rect Message_rect;

SDL_Surface* surfaceMessage;

TTF_Font* Sans;
int w,h;


// The STL stack can't take a function pointer as a type //
// so we encapsulate a function pointer within a struct. //
struct StateStruct
{
    int id;
    std::string description;

    //StateStruct() { std::cout << "State constructor" <<  id << std::endl; }
    StateStruct(double c = 2000, std::string desc = "default") : id(c) , description(desc) { std::cout << "State constructor: "  << id << " : " << description << std::endl; }
    ~StateStruct() { std::cout << "State desctructor: " << id << " : " << description << std::endl; }
	void (*StatePointer)();
};

bool SetupTTF( const std::string &fontName, int fontSize );
SDL_Texture* SurfaceToTexture( SDL_Surface* surf );
void CreateTextTextures(std::string inText, int inX, int inY);


// Init and Shutdown functions //
void Init();
void Shutdown();

// Functions to handle the states of the game //
void Menu();
void Game();
void Exit();
void GameWon();
void GameLost();
void Option();

// Helper functions for the main game state functions //
void DrawBackground();

void HandleMenuInput();
void HandleGameInput();
void HandleExitInput();
void HandleWinLoseInput();
void HandleOptionInput();

bool CheckEntityCollisions(cSquare* square, Direction dir);
bool CheckWallCollisions(cSquare* square, Direction dir);
bool CheckEntityCollisions(cBlock* block, Direction dir);
bool CheckWallCollisions(cBlock* block, Direction dir);
bool CheckRotationCollisions(cBlock* block);

void CheckWin();
void CheckLoss();
void HandleBottomCollision();
void ChangeFocusBlock();
int CheckCompletedLines();
void ClearOldSquares();
void ClearAllStates();
void Debugger();
std::string GetDateTime();

Mix_Chunk *sfx[100] = {};
Mix_Music *audio[100] = {};
// a StateStruct for usage all over
StateStruct aState(1,"global");

bool quit = true;


int main(int argc, char **argv)
{
	Init();

    renderer = SDL_CreateRenderer(window, -1, 0);

    g_StateQueue.push_front("MAIN");

    //std::cout << str << std::endl;
    SetupTTF( "./assets/fonts/RPGSystem.ttf", 24 );

	while(quit)
	{
        g_StateStack.top().StatePointer();
        SDL_RenderPresent(renderer);
    }

	Shutdown();

	return 0;
}

// This function initializes our game //
void Init()
{
    string line;
    ifstream myfile ("game.ini");
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            //the following line trims white space from the beginning of the string
            line.erase(line.begin(), find_if(line.begin(), line.end(), not1(ptr_fun<int, int>(isspace))));


            if(line[0] == '#') continue;
            cout << line << '\n';
            ConfigData.push_back(atoi(line.c_str()));
        }
        myfile.close();
    }
    else
    {
        cout << "Unable to open file";
    }

	// Initiliaze SDL video and our timer //
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0 )
	{
        std::cout << "Unable to initialize SDL2: " << SDL_GetError() << std::endl;
        quit = true;
	};

    // Creates a window object
	window = SDL_CreateWindow(WINDOW_CAPTION,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

    // Loading and using a logo image for the application running
    SDL_Surface* LogoSurface = IMG_Load( "./assets/atari.png" );

    // The icon is attached to the window pointer
    SDL_SetWindowIcon(window, LogoSurface);

    SDL_FreeSurface(LogoSurface);
    // Loading and using a logo image for the application running END

	// Get the number of ticks since SDL was initialized //
	g_Timer = SDL_GetTicks();

	// Seed our random number generator //
	srand( time(0) );

	//Initialize SDL_mixer
	if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
	{
        std::cout << "FAIL OPEN AUDIO" << std::endl;
    }

    Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    // Load our sound effect
	sfx[0] = Mix_LoadWAV("./sfx/SFX_ButtonHover.ogg");
	sfx[1] = Mix_LoadWAV("./sfx/SFX_ButtonUp.ogg");
	sfx[2] = Mix_LoadWAV("./sfx/SFX_GameOver.ogg");
	sfx[3] = Mix_LoadWAV("./sfx/SFX_GameStart.ogg");
	sfx[4] = Mix_LoadWAV("./sfx/SFX_LevelUp.ogg");
	sfx[5] = Mix_LoadWAV("./sfx/SFX_PieceFall.ogg");
	sfx[6] = Mix_LoadWAV("./sfx/SFX_PieceHardDrop.ogg");
	sfx[7] = Mix_LoadWAV("./sfx/SFX_PieceHold.ogg");
	sfx[8] = Mix_LoadWAV("./sfx/SFX_PieceLockdown.ogg");
	sfx[9] = Mix_LoadWAV("./sfx/SFX_PieceMoveLR.ogg");
	sfx[10] = Mix_LoadWAV("./sfx/SFX_PieceRotateFail.ogg");
	sfx[11] = Mix_LoadWAV("./sfx/SFX_PieceRotateLR.ogg");
	sfx[12] = Mix_LoadWAV("./sfx/SFX_PieceSoftDrop.ogg");
	sfx[13] = Mix_LoadWAV("./sfx/SFX_PieceTouchDown.ogg");
	sfx[14] = Mix_LoadWAV("./sfx/SFX_PieceTouchLR.ogg");
	sfx[15] = Mix_LoadWAV("./sfx/SFX_SpecialLineClearDouble.ogg");
	sfx[16] = Mix_LoadWAV("./sfx/SFX_SpecialLineClearSingle.ogg");
	sfx[17] = Mix_LoadWAV("./sfx/SFX_SpecialLineClearTriple.ogg");
	sfx[18] = Mix_LoadWAV("./sfx/SFX_SpecialTetris.ogg");
	sfx[19] = Mix_LoadWAV("./sfx/SFX_SpecialTSpin.ogg");
	sfx[20] = Mix_LoadWAV("./sfx/SFX_SpecialTSpinDouble.ogg");
	sfx[21] = Mix_LoadWAV("./sfx/SFX_SpecialTSpinEZ.ogg");
	sfx[22] = Mix_LoadWAV("./sfx/SFX_SpecialTSpinEZSingle.ogg");
	sfx[23] = Mix_LoadWAV("./sfx/SFX_SpecialTSpinSingle.ogg");
	sfx[24] = Mix_LoadWAV("./sfx/SFX_SpecialTSpinTriple.ogg");
	sfx[25] = Mix_LoadWAV("./sfx/SFX_Splash.ogg");

	// Voices
	sfx[26] = Mix_LoadWAV("./sfx/VO_01TIC.ogg");
	sfx[27] = Mix_LoadWAV("./sfx/VO_02TIC.ogg");
	sfx[28] = Mix_LoadWAV("./sfx/VO_03TIC.ogg");
	sfx[29] = Mix_LoadWAV("./sfx/VO_04TIC.ogg");
	sfx[30] = Mix_LoadWAV("./sfx/VO_05TIC.ogg");
	sfx[31] = Mix_LoadWAV("./sfx/VO_WOW.ogg");
	sfx[32] = Mix_LoadWAV("./sfx/VO_WONDRFL.ogg");
	sfx[33] = Mix_LoadWAV("./sfx/VO_VRYGOOD.ogg");
	sfx[34] = Mix_LoadWAV("./sfx/VO_TIME.ogg");
	sfx[35] = Mix_LoadWAV("./sfx/VO_THTGREAT.ogg");
	sfx[36] = Mix_LoadWAV("./sfx/VO_RECTIME.ogg");
	sfx[37] = Mix_LoadWAV("./sfx/VO_FANTSTC.ogg");
	sfx[38] = Mix_LoadWAV("./sfx/VO_EXLNT.ogg");
	sfx[39] = Mix_LoadWAV("./sfx/VO_COUNTDWN.ogg");
	sfx[40] = Mix_LoadWAV("./sfx/VO_BRILLIANT.ogg");
	sfx[41] = Mix_LoadWAV("./sfx/VO_BTBTSPIN.ogg");

	// Load our music
	audio[0] = Mix_LoadMUS("./sfx/Tetris.ogg");
	if (audio[0] == NULL)
	{
		std::cout << "FAIL OPEN AUDIO" << std::endl;
    }

	if ( Mix_PlayChannel(-1, sfx[0], 0) == -1 )
	{
		std::cout << "FAIL OPEN AUDIO" << std::endl;
    }

	if ( Mix_PlayMusic( audio[0], -1) == -1 )
    {
        std::cout << "FAIL OPEN AUDIO" << std::endl;
    }

	// Initialize blocks and set them to their proper locations. //
	g_FocusBlock = new cBlock(BLOCK_START_X, BLOCK_START_Y, g_Bitmap, (BlockType)(rand()%7));
	g_NextBlock  = new cBlock(NEXT_BLOCK_CIRCLE_X, NEXT_BLOCK_CIRCLE_Y, g_Bitmap, (BlockType)(rand()%7));

	// We start by adding a pointer to our exit state, this way //
	// it will be the last thing the player sees of the game.   //

    // Adding a Exit state to the state queue
	StateStruct state(1000,GetDateTime());
	state.StatePointer = Exit;
	g_StateStack.push(state);
	g_StateQueue.push_front("Exit");

	// Then we add a pointer to our menu state, this will //
	// be the first thing the player sees of our game.    //

	state.StatePointer = Menu;
	g_StateStack.push(state);
	g_StateQueue.push_front("Menu");

	// Initialize the true type font library //
	//SetupTTF("./assets/fonts/replay.ttf");
    //std::cout << TTF_GetError() << std::endl;

    Mix_Volume(-1, MIX_MAX_VOLUME/4);
}

std::string GetDateTime()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
    auto str = oss.str();
    return str;
}

// This function shuts down our game //
void Shutdown()
{
	// Get pointers to the squares in our focus and next blocks so we can delete them. //
	// We must do this before we delete our blocks so we don't lose references to the squares. //
	// Note that these are pointers to arrays of pointers. //
	cSquare** temp_array_1 = g_FocusBlock->GetSquares();
	cSquare** temp_array_2 = g_NextBlock->GetSquares();

	// Delete the temporary arrays of squares

	for (unsigned int i=0; i < 4; i++)
	{
		delete temp_array_1[i];
		delete temp_array_2[i];
	}

	// Delete our blocks //
	delete g_FocusBlock;
	delete g_NextBlock;

	// Delete the squares that are in the game area //
	for (unsigned int i=0; i < g_OldSquares.size(); i++)
	{
		delete g_OldSquares[i];
	}

	// clean up our resources

    for (unsigned int i=0; i < 100; i++)
    {
        Mix_FreeMusic(audio[i]);
        audio[i]=NULL; // to be safe..
    }
    for (unsigned int i=0; i < 100; i++)
    {
        Mix_FreeChunk(sfx[i]);
        sfx[i]=NULL; // to be safe..
    }

	// quit SDL_mixer
    Mix_CloseAudio();

	// Shutdown the true type SDL font library
	TTF_Quit();

	// Tell SDL to shutdown and free any resources it was using
	SDL_Quit();
    quit = false;
}

// This function handles the game's main menu. From here //
// the player can select to enter the game, or quit.     //
void Menu()
{
	// Here we compare the difference between the current time and the last time we //
	// handled a frame. If FRAME_RATE amount of time has passed, it's time for a new frame. //
	if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
	{
		// We start by calling our input function
		HandleMenuInput();

        SDL_Texture *img = NULL;
        int w, h; // texture width & height
        img = IMG_LoadTexture(renderer, "./data/FallingBlocks2.bmp");

        if (img == NULL)
            std::cout << "Couldn't load ./data/FallingBlocks2.bmp" << std::endl;

        SDL_QueryTexture(img, NULL, NULL, &w, &h); // get the width and height of the texture
        SDL_Rect texr; texr.x = 0; texr.y = 0; texr.w = w; texr.h = h;

		// copy the texture to the rendering context
		SDL_RenderCopy(renderer, img, NULL, &texr);

        CreateTextTextures("Start (G)ame",100,120);
		CreateTextTextures("(Q)uit game",100,150);
		CreateTextTextures("(O)ptions",100,180);

		g_Timer = SDL_GetTicks();
	}
    Debugger();
}

// This function handles the main game. We'll control the   //
// drawing of the game as well as any necessary game logic. //
void Game()
{
	// Every frame we increase this value until it is equal to g_FocusBlockSpeed. //
	// When it reaches that value, we force the focus block down. //
	static int force_down_counter = 0;

	// Every frame, we check to see if the focus block's bottom has hit something. If it    //
	// has, we decrement this counter. If the counter hits zero, the focus block needs to   //
	// be changed. We use this counter so the player can slide the block before it changes. //
	static int slide_counter = SLIDE_TIME;

	// Here we compare the difference between the current time and the last time we //
	// handled a frame. If FRAME_RATE amount of time has, it's time for a new frame. //
	if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
	{
        //ClearScreen(renderer);

		HandleGameInput();

		force_down_counter++;

		if (force_down_counter >= g_FocusBlockSpeed)
		{
			// Always check for collisions before moving anything //
			if ( !CheckWallCollisions(g_FocusBlock, DOWN) && !CheckEntityCollisions(g_FocusBlock, DOWN) )
			{
				g_FocusBlock->Move(DOWN); // move the focus block
				force_down_counter = 0;   // reset our counter
			}
		}

		// Check to see if focus block's bottom has hit something. If it has, we decrement our counter. //
		if ( CheckWallCollisions(g_FocusBlock, DOWN) || CheckEntityCollisions(g_FocusBlock, DOWN) )
		{
			slide_counter--;
		}
		// If there isn't a collision, we reset our counter.    //
		// This is in case the player moves out of a collision. //
		else
		{
			slide_counter = SLIDE_TIME;
		}
		// If the counter hits zero, we reset it and call our //
		// function that handles changing the focus block.    //
		if (slide_counter == 0)
		{
			slide_counter = SLIDE_TIME;
			HandleBottomCollision();
		}

        SDL_Texture *img = NULL;
        int w, h; // texture width & height
        img = IMG_LoadTexture(renderer, "./data/FallingBlocks2.bmp");

        if (img == NULL)
            std::cout << "Couldn't load ./data/FallingBlocks2.bmp" << std::endl;

        SDL_QueryTexture(img, NULL, NULL, &w, &h); // get the width and height of the texture
        SDL_Rect texr; texr.x = 0; texr.y = 0; texr.w = w; texr.h = h;

		// copy the texture to the rendering context
		SDL_RenderCopy(renderer, img, NULL, &texr);

		// Draw the focus block and next block. BUG ERROR BLACK BACKGROUND//
		g_FocusBlock->Draw(g_Window,renderer);
		g_NextBlock->Draw(g_Window,renderer);

		// Draw the old squares. //
		for (unsigned int i=0; i < g_OldSquares.size(); i++)
		{
			g_OldSquares[i]->Draw(g_Window,renderer);
		}

		string score = "Score: ";
		score.append( std::to_string(g_Score) );

		string nextscore = "Needed Score: ";
		nextscore.append( std::to_string(g_Level*POINTS_PER_LEVEL) );

		string level = "Level: ";
		level.append( std::to_string(g_Level) );

		string lines = "Lines: ";
		lines.append( std::to_string(g_Lines) );

		CreateTextTextures(nextscore.c_str(),NEEDED_SCORE_RECT_X,NEEDED_SCORE_RECT_Y);
		CreateTextTextures(score.c_str(),SCORE_RECT_X,SCORE_RECT_Y);
		CreateTextTextures(level.c_str(),LEVEL_RECT_X,LEVEL_RECT_Y);
		CreateTextTextures(lines.c_str(),LEVEL_RECT_X,LEVEL_RECT_Y+ 30);

		g_Timer = SDL_GetTicks();
	}
}

// This function handles the game's exit screen. It will display //
// a message asking if the player really wants to quit.          //
void Exit()
{
    quit = false;
	// Here we compare the difference between the current time and the last time we //
	// handled a frame. If FRAME_RATE amount of time has, it's time for a new frame. //
	if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
	{
		HandleExitInput();

        //ClearScreen(renderer);

        CreateTextTextures("(Q)uit Game (Y or N)",100,150);

		g_Timer = SDL_GetTicks();
	}
}

// Display a victory message. //
void GameWon()
{
	if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
	{
		HandleWinLoseInput();

		//ClearScreen(renderer);

		CreateTextTextures("You Win!!!",100,120);
		CreateTextTextures("(Q)uit Game (Y or N)",100,150);

		g_Timer = SDL_GetTicks();
	}
}

// Display a game over message. //
void GameLost()
{
	if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
	{
		HandleWinLoseInput();

		//ClearScreen(renderer);

        SDL_Texture *img = NULL;
        int w, h; // texture width & height
        img = IMG_LoadTexture(renderer, "./data/FallingBlocks2.bmp");

        if (img == NULL)
            std::cout << "Couldn't load ./data/FallingBlocks2.bmp" << std::endl;

        SDL_QueryTexture(img, NULL, NULL, &w, &h); // get the width and height of the texture
        SDL_Rect texr; texr.x = 0; texr.y = 0; texr.w = w; texr.h = h;

		// copy the texture to the rendering context
		SDL_RenderCopy(renderer, img, NULL, &texr);

		CreateTextTextures("You Lose.",100,120);
		CreateTextTextures("(Q)uit Game (Y or N)",100,150);

        static int runonce = 0;
        if(runonce == 0)
        {
            if ( Mix_PlayChannel(2, sfx[2], 0) == -1 )
            {
                std::cout << "FAIL OPEN AUDIO" << std::endl;
            }
            runonce = 1;
        }
		g_Timer = SDL_GetTicks();
	}
}

// This function draws the background //
void DrawBackground()
{
	SDL_Rect source;

	// Set our source rectangle to the current level's background //
	switch (g_Level)
	{
	case 1:
		{
		SDL_Rect temp = { LEVEL_ONE_X, LEVEL_ONE_Y, WINDOW_WIDTH, WINDOW_HEIGHT };
		source = temp;
		} break;
	case 2:
		{
		SDL_Rect temp = { LEVEL_TWO_X, LEVEL_TWO_Y, WINDOW_WIDTH, WINDOW_HEIGHT };
		source = temp;
		} break;
	case 3:
		{
		SDL_Rect temp = { LEVEL_THREE_X, LEVEL_THREE_Y, WINDOW_WIDTH, WINDOW_HEIGHT };
		source = temp;
		} break;
	case 4:
		{
		SDL_Rect temp = { LEVEL_FOUR_X, LEVEL_FOUR_Y, WINDOW_WIDTH, WINDOW_HEIGHT };
		source = temp;
		} break;
	case 5:
		{
		SDL_Rect temp = { LEVEL_FIVE_X, LEVEL_FIVE_Y, WINDOW_WIDTH, WINDOW_HEIGHT };
		source = temp;
		} break;
	}

	//SDL_Rect destination = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
}

// This function receives player input and //
// handles it for the game's menu screen.  //
void HandleMenuInput()
{
	// Fill our event structure with event information. //
	if ( SDL_PollEvent(&g_Event) )
	{
		// Handle user manually closing game window //
		if (g_Event.type == SDL_QUIT)
		{
			// While state stack isn't empty, pop //
			while (!g_StateStack.empty())
			{
				g_StateStack.pop();
			}

			return;  // game is over, exit the function
		}

		// Handle keyboard input here //
		if (g_Event.type == SDL_KEYDOWN)
		{
			if (g_Event.key.keysym.sym == SDLK_ESCAPE)
			{
				g_StateStack.pop();
				return;  // this state is done, exit the function
			}
			// Quit //
			if (g_Event.key.keysym.sym == SDLK_q)
			{
				g_StateStack.pop();
				return;  // game is over, exit the function
			}
			// Start Game //
			if (g_Event.key.keysym.sym == SDLK_g)
			{
				StateStruct temp;
				temp.StatePointer = Game;
				g_StateStack.push(temp);
				return;  // this state is done, exit the function
			}
			if (g_Event.key.keysym.sym == SDLK_o)
			{
				StateStruct temp;
				temp.StatePointer = Option;
				g_StateStack.push(temp);
				return;  // this state is done, exit the function
			}
		}
	}
}

// This function receives player input and //
// handles it for the main game state.     //
void HandleGameInput()
{
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if(keystates[SDL_SCANCODE_LEFT])
	{
		if ( !CheckWallCollisions(g_FocusBlock, LEFT) &&
			 !CheckEntityCollisions(g_FocusBlock, LEFT) )
		{
			g_FocusBlock->Move(LEFT);
		}
        else
        {
            if ( Mix_PlayChannel(-1, sfx[10], 0) == -1 )
            {
                std::cout << "FAIL OPEN AUDIO" << std::endl;
            }
        }
	}
    else if(keystates[SDL_SCANCODE_RIGHT])
	{
		if ( !CheckWallCollisions(g_FocusBlock, RIGHT) &&
			 !CheckEntityCollisions(g_FocusBlock, RIGHT) )
		{
			g_FocusBlock->Move(RIGHT);
		}
		else
		{
            if ( Mix_PlayChannel(-1, sfx[10], 0) == -1 )
            {
                std::cout << "FAIL OPEN AUDIO" << std::endl;
            }
		}
	}
    else if(keystates[SDL_SCANCODE_UP])
    {
        // Check collisions before rotating //
        if (!CheckRotationCollisions(g_FocusBlock))
        {
            g_FocusBlock->Rotate();
            SDL_Delay(50);
        }
        else
        {
            if ( Mix_PlayChannel(-1, sfx[10], 0) == -1 )
            {
                std::cout << "FAIL OPEN AUDIO" << std::endl;
            }
        }
    }
    else if(keystates[SDL_SCANCODE_DOWN])
    {
		if ( !CheckWallCollisions(g_FocusBlock, DOWN) &&
			 !CheckEntityCollisions(g_FocusBlock, DOWN) )
		{
			g_FocusBlock->Move(DOWN);
		}
        else
        {
            if ( Mix_PlayChannel(-1, sfx[10], 0) == -1 )
            {
                std::cout << "FAIL OPEN AUDIO" << std::endl;
            }
        }
    }

    else if(keystates[SDL_SCANCODE_ESCAPE])
        g_StateStack.pop();

	// These variables allow the user to hold the arrow keys down //
	static bool down_pressed  = false;
	static bool left_pressed  = false;
	static bool right_pressed = false;

	// Fill our event structure with event information. //
	if ( SDL_PollEvent(&g_Event) )
	{
		// Handle user manually closing game window //
		if (g_Event.type == SDL_QUIT)
		{
			// While state stack isn't empty, pop //
			while (!g_StateStack.empty())
			{
				g_StateStack.pop();
			}

			return;  // game is over, exit the function
		}

		// Handle keyboard input here //
		if (g_Event.type == SDL_KEYDOWN)
		{
			if (g_Event.key.keysym.sym == SDLK_ESCAPE)
			{
				//g_StateStack.pop();

				return;  // this state is done, exit the function
			}

			if (g_Event.key.keysym.sym == SDLK_UP)
			{
				// Check collisions before rotating //
				if (!CheckRotationCollisions(g_FocusBlock))
				{
				//	g_FocusBlock->Rotate();
				}
				else
				{
				}
			}

			// For the left, right, and down arrow keys, we just set a bool variable //
			if (g_Event.key.keysym.sym == SDLK_LEFT)
			{
				left_pressed = true;
			}
			if (g_Event.key.keysym.sym == SDLK_RIGHT)
			{
				right_pressed = true;
			}
			if (g_Event.key.keysym.sym == SDLK_DOWN)
			{
				down_pressed = true;
			}
		}

		// If player lifts key, set bool variable to false //
		if (g_Event.type == SDL_KEYUP)
		{
			if (g_Event.key.keysym.sym == SDLK_LEFT)
			{
				left_pressed = false;
			}
			if (g_Event.key.keysym.sym == SDLK_RIGHT)
			{
				right_pressed = false;
			}
			if (g_Event.key.keysym.sym == SDLK_DOWN)
			{
				down_pressed = false;
			}
		}
	}

	// Now we handle the arrow keys, making sure to check for collisions //
	if (down_pressed)
	{
		if ( !CheckWallCollisions(g_FocusBlock, DOWN) &&
			 !CheckEntityCollisions(g_FocusBlock, DOWN) )
		{
			//g_FocusBlock->Move(DOWN);
		}
        else
        {
        }
	}
	if (left_pressed)
	{
		if ( !CheckWallCollisions(g_FocusBlock, LEFT) &&
			 !CheckEntityCollisions(g_FocusBlock, LEFT) )
		{
			//g_FocusBlock->Move(LEFT);
		}
        else
        {
        }
	}
	if (right_pressed)
	{
		if ( !CheckWallCollisions(g_FocusBlock, RIGHT) &&
			 !CheckEntityCollisions(g_FocusBlock, RIGHT) )
		{
			//g_FocusBlock->Move(RIGHT);
		}
		else
		{
		}
	}
}

// This function receives player input and //
// handles it for the game's exit screen.  //
void HandleExitInput()
{
	// Fill our event structure with event information. //
	if ( SDL_PollEvent(&g_Event) )
	{
		// Handle user manually closing game window //
		if (g_Event.type == SDL_QUIT)
		{
			// While state stack isn't empty, pop //
			while (!g_StateStack.empty())
			{
				g_StateStack.pop();
			}
			return;  // game is over, exit the function
		}

		// Handle keyboard input here //
		if (g_Event.type == SDL_KEYDOWN)
		{
			if (g_Event.key.keysym.sym == SDLK_ESCAPE)
			{
				g_StateStack.pop();

				return;  // this state is done, exit the function
			}
			// Yes //
			if (g_Event.key.keysym.sym == SDLK_y)
			{
				g_StateStack.pop();
				return;  // game is over, exit the function
			}
			// No //
			if (g_Event.key.keysym.sym == SDLK_n)
			{
				StateStruct temp;
				temp.StatePointer = Menu;
				g_StateStack.push(temp);
				return;  // this state is done, exit the function
			}
		}
	}
}

// Input handling for win/lose screens. //
void HandleWinLoseInput()
{
	if ( SDL_PollEvent(&g_Event) )
	{
		// Handle user manually closing game window //
		if (g_Event.type == SDL_QUIT)
		{
			// While state stack isn't empty, pop //
			while (!g_StateStack.empty())
			{
				g_StateStack.pop();
			}
			return;
		}

		// Handle keyboard input here //
		if (g_Event.type == SDL_KEYDOWN)
		{
			if (g_Event.key.keysym.sym == SDLK_ESCAPE)
			{
				g_StateStack.pop();
				return;
			}
			if (g_Event.key.keysym.sym == SDLK_y)
			{
				g_StateStack.pop();
				return;
			}
			// If player chooses to continue playing, we pop off    //
			// current state and push exit and menu states back on. //
			if (g_Event.key.keysym.sym == SDLK_n)
			{
				g_StateStack.pop();

				StateStruct temp;
				temp.StatePointer = Exit;
				g_StateStack.push(temp);

				temp.StatePointer = Menu;
				g_StateStack.push(temp);
				return;
			}
		}
	}
}

// Check collisions between a given square and the squares in g_OldSquares //
bool CheckEntityCollisions(cSquare* square, Direction dir)
{
	// Width/height of a square. Also the distance //
	// between two squares if they've collided.    //
	int distance = SQUARE_MEDIAN * 2;

	// Center of the given square //
	int centerX = square->GetCenterX();
	int centerY = square->GetCenterY();

	// Determine the location of the square after moving //
	switch (dir)
	{
	case DOWN:
		{
			centerY += distance;
		} break;

	case LEFT:
		{
			centerX -= distance;
		} break;

	case RIGHT:
		{
			centerX += distance;
		} break;
	}

	// Iterate through the old squares vector, checking for collisions //
	for (unsigned int i=0; i < g_OldSquares.size(); i++)
	{
		if ( ( abs(centerX - g_OldSquares[i]->GetCenterX() ) < distance ) &&
			 ( abs(centerY - g_OldSquares[i]->GetCenterY() ) < distance ) )
		{
			return true;
		}
	}

	return false;
}

// Check collisions between a given block and the squares in g_OldSquares //
bool CheckEntityCollisions(cBlock* block, Direction dir)
{
	// Get an array of the squares that make up the given block //
	cSquare** temp_array = block->GetSquares();

	// Now just call the other CheckEntityCollisions() on each square //
	for (int i=0; i<4; i++)
	{
		if ( CheckEntityCollisions(temp_array[i], dir) )
		{
            return true;
        }
	}
	return false;
}

// Check collisions between a given square and the sides of the game area //
bool CheckWallCollisions(cSquare* square, Direction dir)
{
	// Get the center of the square //
	int x = square->GetCenterX();
	int y = square->GetCenterY();

	// Get the location of the square after moving and see if its out of bounds //
	switch (dir)
	{
	case DOWN:
		{
			if ( (y + (SQUARE_MEDIAN*2)) > GAME_AREA_BOTTOM )
			{
				return true;
			}
			else
			{
				return false;
			}
		} break;

	case LEFT:
		{
			if ( (x - (SQUARE_MEDIAN*2)) < GAME_AREA_LEFT )
			{
				return true;
			}
			else
			{
				return false;
			}
		} break;

	case RIGHT:
		{
			if ( (x + (SQUARE_MEDIAN*2)) > GAME_AREA_RIGHT )
			{
				return true;
			}
			else
			{
				return false;
			}
		} break;
	}

	return false;
}

// Check for collisions between a given block a the sides of the game area //
bool CheckWallCollisions(cBlock* block, Direction dir)
{
	// Get an array of squares that make up the given block //
	cSquare** temp_array = block->GetSquares();

	// Call other CheckWallCollisions() on each square //
	for (int i=0; i<4; i++)
	{
		if ( CheckWallCollisions(temp_array[i], dir) )
			return true;
	}
	return false;
}

// Check for collisions when a block is rotated //
bool CheckRotationCollisions(cBlock* block)
{
	// Get an array of values for the locations of the rotated block's squares //
	int* temp_array = block->GetRotatedSquares();

	// Distance between two touching squares //
	int distance = SQUARE_MEDIAN * 2;

	for (int i=0; i<4; i++)
	{
		// Check to see if the block will go out of bounds //
        if ( (temp_array[i*2] < GAME_AREA_LEFT) || (temp_array[i*2] > GAME_AREA_RIGHT) )
		{
			delete temp_array;
			return true;
		}

		if ( temp_array[i*2+1] > GAME_AREA_BOTTOM )
		{
			delete temp_array;
			return true;
		}

		// Check to see if the block will collide with any squares //
		for (unsigned int index=0; index < g_OldSquares.size(); index++)
		{
			if ( ( abs(temp_array[i*2]   - g_OldSquares[index]->GetCenterX()) < distance ) &&
				 ( abs(temp_array[i*2+1] - g_OldSquares[index]->GetCenterY()) < distance ) )
			{
				delete temp_array;
				return true;
			}
		}
	}

	delete temp_array;
	return false;
}

// This function handles all of the events that   //
// occur when the focus block can no longer move. //
void HandleBottomCollision()
{
	ChangeFocusBlock();

	// Check for completed lines and store the number of lines completed //
	int num_lines = CheckCompletedLines();

	if ( num_lines > 0 )
	{
        // increase player's number of lines
        g_Lines += num_lines;
		// Increase player's score according to number of lines completed //
		g_Score += POINTS_PER_LINE * num_lines;

		// Check to see if it's time for a new level //
		if (g_Score >= g_Level * POINTS_PER_LEVEL)
		{
            if ( Mix_PlayChannel(-1, sfx[4], 0) == -1 )
            {
                std::cout << "FAIL OPEN AUDIO" << std::endl;
            }
			g_Level++;
			CheckWin(); // check for a win after increasing the level
			g_FocusBlockSpeed -= SPEED_CHANGE; // shorten the focus blocks movement interval
		}
	}

	// Now would be a good time to check to see if the player has lost //
	CheckLoss();
}

// Add the squares of the focus block to g_OldSquares //
// and set the next block as the focus block. //
void ChangeFocusBlock()
{
	// Get an array of pointers to the focus block squares //
	cSquare** square_array = g_FocusBlock->GetSquares();


	// Add focus block squares to g_OldSquares //
	for (int i=0; i<4; i++)
	{
		g_OldSquares.push_back(square_array[i]);
	}

	delete g_FocusBlock;        // delete the current focus block
	g_FocusBlock = g_NextBlock; // set the focus block to the next block
	g_FocusBlock->SetupSquares(BLOCK_START_X, BLOCK_START_Y, g_Bitmap);

	// Set the next block to a new block of random type //
	g_NextBlock = new cBlock(NEXT_BLOCK_CIRCLE_X, NEXT_BLOCK_CIRCLE_Y, g_Bitmap, (BlockType)(rand()%7));
}

// Return amount of lines cleared or zero if no lines were cleared //
int CheckCompletedLines()
{
	// Store the amount of squares in each row in an array and zero it //
	int squares_per_row[SQUARES_IN_ROW] = {};

	int row_size   = SQUARE_MEDIAN * 2;                // pixel size of one row
	int bottom     = GAME_AREA_BOTTOM - SQUARE_MEDIAN; // center of bottom row
	int top        = bottom - 12 * row_size;		   // center of top row

	int num_lines = 0; // number of lines cleared
	int row;           // multipurpose variable

	// Check for full lines //
	for (unsigned int i=0; i < g_OldSquares.size(); i++)
	{
		// Get the row the current square is in //
		row = (g_OldSquares[i]->GetCenterY() - top) / row_size;

		// Increment the appropriate row counter //
		squares_per_row[row]++;
	}

	// Erase any full lines //
	for (int line=0; line < SQUARES_IN_ROW; line++)
	{
		// Check for completed lines //
		if (squares_per_row[line] == SQUARES_PER_ROW)
		{
			// Keep track of how many lines have been completed //
			num_lines++;

			// Find any squares in current row and remove them //
			for (unsigned int index=0; index < g_OldSquares.size(); index++)
			{
				if ( ( (g_OldSquares[index]->GetCenterY() - top) / row_size ) == line )
				{
					delete g_OldSquares[index]; // delete the square
					g_OldSquares.erase(g_OldSquares.begin() + index); // remove it from the vector
					index--; // make sure we don't skip anything
				}
			}
		}
	}

	// Move squares above cleared line down //
	for (unsigned int index=0; index < g_OldSquares.size(); index++)
	{
		for (int line=0; line < SQUARES_IN_ROW; line++)
		{
			// Determine if this row was filled //
			if (squares_per_row[line] == SQUARES_PER_ROW)
			{
				// If it was, get the location of it within the game area //
				row = (g_OldSquares[index]->GetCenterY() - top) / row_size;

				// Now move any squares above that row down one //
				if ( row < line )
				{
					g_OldSquares[index]->Move(DOWN);
				}
			}
		}
	}

	if(num_lines > 0 )
	{
        if ( Mix_PlayChannel(num_lines, sfx[25+num_lines], 0) == -1 )
        {
            std::cout << "FAIL OPEN AUDIO" << std::endl;
        }
    }

	return num_lines;
}

// Check to see if player has won. Handle winning condition if needed. //
// Every turn
void CheckWin()
{
    std::cout << "CheckWin" << std::endl;
	// If current level is greater than number of levels, player has won //
	if (g_Level > NUM_LEVELS)
	{
        ClearOldSquares();
		// Reset score and level //
		g_Score = 0;
		g_Level = 1;

		ClearAllStates();

		// Push the victory state onto the stack //
		//StateStruct win;
		//win.StatePointer = GameWon;
		aState.StatePointer = GameWon;

		g_StateStack.push(aState);
	}
}

// Check to see if player has lost. Handle losing condition if needed. //
// Every turn
void CheckLoss()
{
	// We call this function when the focus block is at the top of that //
	// game area. If the focus block is stuck now, the game is over.    //
	if ( CheckEntityCollisions(g_FocusBlock, DOWN) )
	{
        ClearOldSquares();
		// Reset score and level //
		g_Score = 0;
		g_Level = 1;

        ClearAllStates();

		// Push the losing state onto the stack //
		//StateStruct lose;

		//lose.StatePointer = GameLost;
		aState.StatePointer = GameLost;

		g_StateStack.push(aState);
	}
}

void ClearOldSquares()
{
		// Clear the old squares vector //
		for (unsigned int i=0; i < g_OldSquares.size(); i++)
		{
			delete g_OldSquares[i];
		}
		g_OldSquares.clear();
}

void ClearAllStates()
{
		// Pop all states //
		while (!g_StateStack.empty())
		{
			g_StateStack.pop();
		}
}

void Debugger()
{
    std::cout << "Debugging stuff" << std::endl;
    std::cout << "---------------" << std::endl;
    std::cout << "g_Level: " << g_Level << std::endl;
    std::cout << "g_Score: " << g_Score << std::endl;
    for(auto it = g_StateQueue.begin(); it != g_StateQueue.end(); it++){
        std::cout << *(it) << std::endl;
    }
}

void CreateTextTextures(std::string inText, int inX, int inY)
{
    SDL_Color textColor  = { 255, 255, 255};   // text color
    SDL_Color backgroundColor  = { 0, 0, 0};   // text color
    SDL_Surface* solid = TTF_RenderText_Solid( font, inText.c_str(), textColor );
    solidTexture = SurfaceToTexture( solid );

    SDL_QueryTexture( solidTexture, NULL, NULL, &solidRect.w, &solidRect.h );
    solidRect.x = inX;
    solidRect.y = inY;

    SDL_Surface* blended = TTF_RenderText_Blended( font, inText.c_str(), textColor );
    blendedTexture = SurfaceToTexture( blended );

    SDL_QueryTexture( blendedTexture, NULL, NULL, &blendedRect.w, &blendedRect.h );
    blendedRect.x = 0;
    blendedRect.y = solidRect.y + solidRect.h +  20;

    SDL_Surface* shaded = TTF_RenderText_Shaded( font, inText.c_str(), textColor, backgroundColor );
    shadedTexture = SurfaceToTexture( shaded );

    SDL_QueryTexture( shadedTexture , NULL, NULL, &shadedRect.w, &shadedRect.h );
    shadedRect.x = 0;
    shadedRect.y = blendedRect.y + blendedRect.h + 20;

    SDL_RenderCopy( renderer, solidTexture, nullptr, &solidRect );
    //SDL_RenderCopy( renderer, blendedTexture, nullptr, &blendedRect );
    //SDL_RenderCopy( renderer, shadedTexture, nullptr, &shadedRect );
}

// Convert an SDL_Surface to SDL_Texture.
SDL_Texture* SurfaceToTexture( SDL_Surface* surf )
{
    SDL_Texture* text;
    text = SDL_CreateTextureFromSurface( renderer, surf );
    SDL_FreeSurface( surf );
    return text;
}


void HandleOptionInput()
{
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if(keystates[SDL_SCANCODE_KP_MINUS])
    {
         Mix_Volume(-1,Mix_Volume(-1,-1) - VOLUME_ADJUST);
    }
    else if(keystates[SDL_SCANCODE_KP_PLUS])
    {
        Mix_Volume(-1,Mix_Volume(-1,-1) + VOLUME_ADJUST);
    }
    else if(keystates[SDL_SCANCODE_G])
    {
        GHOST_PIECE = -GHOST_PIECE;
    }
	// Fill our event structure with event information. //
    else if(keystates[SDL_SCANCODE_Q])
    {
        g_StateStack.pop();
    }
    else if(keystates[SDL_SCANCODE_ESCAPE])
    {
        g_StateStack.pop();
    }

	if ( SDL_PollEvent(&g_Event) )
	{
		// Handle user manually closing game window //
		if (g_Event.type == SDL_QUIT)
		{
			// While state stack isn't empty, pop //
			while (!g_StateStack.empty())
			{
				g_StateStack.pop();
			}

			return;  // game is over, exit the function
		}
	}
	SDL_Delay(10);
}


void Option()
{
	// Here we compare the difference between the current time and the last time we //
	// handled a frame. If FRAME_RATE amount of time has passed, it's time for a new frame. //
	if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
	{
		// We start by calling our input function
		HandleOptionInput();

        SDL_Texture *img = NULL;
        int w, h; // texture width & height
        img = IMG_LoadTexture(renderer, "./data/FallingBlocks2.bmp");

        if (img == NULL)
        {
            std::cout << "Couldn't load ./data/FallingBlocks2.bmp" << std::endl;
        }

        SDL_QueryTexture(img, NULL, NULL, &w, &h); // get the width and height of the texture
        SDL_Rect texr; texr.x = 0; texr.y = 0; texr.w = w; texr.h = h;

		// copy the texture to the rendering context
		SDL_RenderCopy(renderer, img, NULL, &texr);
        int Music_Volume = Mix_VolumeMusic(-1);
        int SFX_Volume = Mix_Volume(-1,-1);
        CreateTextTextures("SOUND FX VOLUME",60,100);
        CreateTextTextures(std::to_string(SFX_Volume),60,130);
		CreateTextTextures("MUSIC VOLUME",60,160);
		CreateTextTextures(std::to_string(Music_Volume),60,190);
		CreateTextTextures("SHOW GHOST PIECE",60,220);
		CreateTextTextures(std::to_string(GHOST_PIECE),60,250);

		g_Timer = SDL_GetTicks();
	}
}

// Initializing SDL Font for usage in the game
bool SetupTTF( const std::string &fontName, int fontSize)
{
    // SDL2_TTF needs to be initialized just like SDL2
    if ( TTF_Init() == -1 )
    {
        std::cout << " Failed to initialize TTF : " << SDL_GetError() << std::endl;
        return false;
    }

    // Load our fonts, with a huge size
    font = TTF_OpenFont( fontName.c_str(), fontSize );

    // Error check
    if ( font == nullptr )
    {
        std::cout << " Failed to load font : " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}
