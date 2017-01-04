//////////////////////////////////////////////////////////////////////////////////
// cBlock.h
// - represents a block in Falling Blocks
// - this class is just four cSquare's placed in a specific shape
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "cSquare.h"

class cBlock
{
private:
	// Location of the center of the block //
	int m_CenterX;
	int m_CenterY;

	// Type of block //
	BlockType m_Type;

	// Array of squares that make up the block //
	cSquare* m_Squares[4];
	//std::vector<cSquare>;

public:
	// The constructor just sets the block location and calls SetupSquares //
	cBlock(int x, int y, SDL_Surface* bitmap, BlockType type)  : m_CenterX(x), m_CenterY(y),
		m_Type(type)
	{
		for (int i=0; i<4; i++)
		{
			m_Squares[i] = NULL;
		}

		SetupSquares(x, y, bitmap);
	}

	// Setup our block according to its location and type. Note that the squares //
	// are defined according to their distance from the block's center. This     //
	// function takes a surface that gets passed to cSquare's constructor.       //
	void SetupSquares(int x, int y, SDL_Surface* bitmap)
	{
		// This function takes the center location of the block. We set our data //
		// members to these values to make sure our squares don't get defined    //
		// around a new center without our block's center values changing too.   //
		m_CenterX = x;
		m_CenterY = y;

        SDL_Color aRGB;

		// Make sure that any current squares are deleted //
		for (int i=0; i<4; i++)
		{
			if (m_Squares[i])
				delete m_Squares[i];
		}

		switch (m_Type)
		{
		case SQUARE_BLOCK:
			{
                //#F7D308
				// Upper left //
				aRGB.r = 247;
				aRGB.g = 211;
				aRGB.b = 8;
				m_Squares[0] = new cSquare(x - SQUARE_MEDIAN, y - SQUARE_MEDIAN, bitmap, m_Type, aRGB );
				// Lower Left //
				m_Squares[1] = new cSquare(x - SQUARE_MEDIAN, y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Upper right //
				m_Squares[2] = new cSquare(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Lower right //
				m_Squares[3] = new cSquare(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
			} break;
		case T_BLOCK:
			{
                // #AD4D9C
				// Top //
				aRGB.r = 173;
				aRGB.g = 77;
				aRGB.b = 156;
				m_Squares[0] = new cSquare(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Middle //
				m_Squares[1] = new cSquare(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Left //
				m_Squares[2] = new cSquare(x - SQUARE_MEDIAN, y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Right //
				m_Squares[3] = new cSquare(x + (SQUARE_MEDIAN * 3), y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
			} break;
		case L_BLOCK:
			{
                // #EF7921
				// |  //
				aRGB.r = 239;
				aRGB.g = 121;
				aRGB.b = 33;
				m_Squares[0] = new cSquare(x - SQUARE_MEDIAN, y - SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// |  //
				m_Squares[1] = new cSquare(x - SQUARE_MEDIAN, y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// |_ //
				m_Squares[2] = new cSquare(x - SQUARE_MEDIAN, y + (SQUARE_MEDIAN * 3), bitmap, m_Type, aRGB);
				      // __ //
				m_Squares[3] = new cSquare(x + SQUARE_MEDIAN, y + (SQUARE_MEDIAN * 3), bitmap, m_Type, aRGB);
			} break;
		case BACKWARDS_L_BLOCK:
			{
                // #2100F9
				    //  | //
				aRGB.r = 33;
				aRGB.g = 0;
				aRGB.b = 249;
				m_Squares[0] = new cSquare(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				    //  | //
				m_Squares[1] = new cSquare(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				    // _| //
				m_Squares[2] = new cSquare(x + SQUARE_MEDIAN, y + (SQUARE_MEDIAN * 3), bitmap, m_Type, aRGB);
				// __ //
				m_Squares[3] = new cSquare(x - SQUARE_MEDIAN, y + (SQUARE_MEDIAN * 3), bitmap, m_Type, aRGB);
			} break;
		case STRAIGHT_BLOCK:
			{
                // #31C7EF
				// Top //
				aRGB.r = 49;
				aRGB.g = 199;
				aRGB.b = 239;
				m_Squares[0] = new cSquare(x + SQUARE_MEDIAN, y - (SQUARE_MEDIAN * 3), bitmap, m_Type, aRGB);
				m_Squares[1] = new cSquare(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				m_Squares[2] = new cSquare(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				m_Squares[3] = new cSquare(x + SQUARE_MEDIAN, y + (SQUARE_MEDIAN * 3), bitmap, m_Type, aRGB);
				// Bottom //
			} break;
		case S_BLOCK:
			{
                // #00FF3B
				// Top right //
				aRGB.r = 0;
				aRGB.g = 255;
				aRGB.b = 59;
				m_Squares[0] = new cSquare(x + (SQUARE_MEDIAN * 3), y - SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Top middle //
				m_Squares[1] = new cSquare(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Bottom middle //
				m_Squares[2] = new cSquare(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Bottom left //
				m_Squares[3] = new cSquare(x - SQUARE_MEDIAN, y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
			} break;
		case BACKWARDS_S_BLOCK:
			{
                // #FF0623
				// Top left //
				aRGB.r = 255;
				aRGB.g = 6;
				aRGB.b = 35;
				m_Squares[0] = new cSquare(x - SQUARE_MEDIAN, y - SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Top middle //
				m_Squares[1] = new cSquare(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Bottom middle //
				m_Squares[2] = new cSquare(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
				// Bottom right //
				m_Squares[3] = new cSquare(x + (SQUARE_MEDIAN * 3), y + SQUARE_MEDIAN, bitmap, m_Type, aRGB);
			} break;
		}
	}

	// Draw() simply iterates through the squares and calls their Draw() functions. //
	void Draw(SDL_Surface* Window, SDL_Renderer* MainRenderer)
	{
		for (int i=0; i<4; i++)
		{
			m_Squares[i]->Draw(Window,MainRenderer);
		}
	}

	// Move() simply changes the block's center and calls the squares' move functions. //
	void Move(Direction dir)
	{
		switch (dir)
		{
		case LEFT:
			{
				m_CenterX -= SQUARE_MEDIAN * 2;
			} break;
		case RIGHT:
			{
				m_CenterX += SQUARE_MEDIAN * 2;
			} break;
		case DOWN:
			{
				m_CenterY += SQUARE_MEDIAN*2;
			} break;
		}

		for (int i=0; i<4; i++)
		{
			m_Squares[i]->Move(dir);
		}
	}

	// This function is explained in the tutorial. //
	void Rotate()
	{
		int x1, y1, x2, y2;

		for (int i=0; i<4; i++)
		{
			x1 = m_Squares[i]->GetCenterX();
			y1 = m_Squares[i]->GetCenterY();

			x1 -= m_CenterX;
			y1 -= m_CenterY;

			x2 = - y1;
			y2 = x1;

			x2 += m_CenterX;
			y2 += m_CenterY;

			m_Squares[i]->SetCenterX(x2);
			m_Squares[i]->SetCenterY(y2);
		}
	}

	// This function gets the locations of the squares after //
	// a rotation and returns an array of those values.      //
	int* GetRotatedSquares()
	{
		int* temp_array = new int[8];
		int x1, y1, x2, y2;

		for (int i=0; i<4; i++)
		{
			x1 = m_Squares[i]->GetCenterX();
			y1 = m_Squares[i]->GetCenterY();

			x1 -= m_CenterX;
			y1 -= m_CenterY;

			x2 = - y1;
			y2 = x1;

			x2 += m_CenterX;
			y2 += m_CenterY;

			temp_array[i*2]   = x2;
			temp_array[i*2+1] = y2;
		}

		return temp_array;
	}

	// This returns an array of pointers to the squares of the block. //
	cSquare** GetSquares()
	{
		return m_Squares;
	}
};
