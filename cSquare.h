//////////////////////////////////////////////////////////////////////////////////
// cSquare.h
// - represents an individual square in Falling Blocks
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Enums.h"
#include "Defines.h"

class cSquare
{
private:
	// Location of the center of the square //
	int m_CenterX;
	int m_CenterY;

	// Type of block. Needed to locate the correct square in our bitmap //
	BlockType m_BlockType;

	// A pointer to our bitmap surface from "Main.cpp" //
	//SDL_Surface* m_Bitmap;

	int m_RedColor;
	int m_GreenColor;
	int m_BlueColor;

public:
	// Default constructor, your compiler will probably require this //
	cSquare()
	{
	}

	// Main constructor takes location and type of block, //
	// and pointer to our bitmap surface. //
	cSquare(int x, int y, SDL_Surface* bitmap, BlockType type, int red, int green, int blue) : m_CenterX(x), m_CenterY(y),
		m_BlockType(type), m_RedColor(red), m_GreenColor(green), m_BlueColor(blue)
	{
        //std::cout << "Main constructor called" << std::endl;
        //std::cout << "x:" << x << "y:" << y << " RGB(" << red << "," << green << "," << blue << ")" << std::endl;
        //std::cout << "CenterX: " << m_CenterX << " CenterY:" << m_CenterY << std::endl;
        //std::cout << "Type: " << getType(type)<< std::endl;
	}

	std::string getType(int type)
	{
		switch (m_BlockType)
		{
		case SQUARE_BLOCK:
            return "SQUARE_BLOCK";
		case T_BLOCK:
            return "T_BLOCKe";
		case L_BLOCK:
            return "L_BLOCK";
		case BACKWARDS_L_BLOCK:
            return "BACKWARDS_L_BLOCK";
		case STRAIGHT_BLOCK:
            return "STRAIGHT_BLOCK";
		case S_BLOCK:
            return "S_BLOCK";
		case BACKWARDS_S_BLOCK:
            return "BACKWARDS_S_BLOCK";
        default:
            return "BAD_BLOCK";
		}
	}

	// Draw() takes a pointer to the surface to draw to (our window) //
	void Draw(SDL_Surface* window, SDL_Renderer* MainRenderer)
	{
        SDL_Rect source;

		// switch statement to determine the location of the square within our bitmap //
		switch (m_BlockType)
		{
		case SQUARE_BLOCK:
			{
                SDL_Rect temp = { RED_SQUARE_X, RED_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
                source = temp;
			} break;
		case T_BLOCK:
			{
                SDL_Rect temp = { PURPLE_SQUARE_X, PURPLE_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
                source = temp;
			} break;
		case L_BLOCK:
			{
                SDL_Rect temp = { GREY_SQUARE_X, GREY_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
                source = temp;
			} break;
		case BACKWARDS_L_BLOCK:
			{
                SDL_Rect temp = { BLUE_SQUARE_X, BLUE_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
                source = temp;
			} break;
		case STRAIGHT_BLOCK:
			{
                SDL_Rect temp = { GREEN_SQUARE_X, GREEN_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
                source = temp;
			} break;
		case S_BLOCK:
			{
                SDL_Rect temp = { BLACK_SQUARE_X, BLACK_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
                source = temp;
			} break;
		case BACKWARDS_S_BLOCK:
			{
                SDL_Rect temp = { YELLOW_SQUARE_X, YELLOW_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
                source = temp;
			} break;
		}

		// Draw at square's current location. Remember that m_X and m_Y store the center of the square. //
		//SDL_Rect destination = { m_CenterX - SQUARE_MEDIAN, m_CenterY - SQUARE_MEDIAN, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };

        SDL_SetRenderDrawColor(MainRenderer, m_RedColor, m_GreenColor, m_BlueColor, 255);
        SDL_Rect rectangle;

        rectangle.x = m_CenterX - SQUARE_MEDIAN;
        rectangle.y = m_CenterY - SQUARE_MEDIAN;
        rectangle.w = SQUARE_MEDIAN * 2;
        rectangle.h = SQUARE_MEDIAN * 2;

        SDL_RenderFillRect(MainRenderer, &rectangle);

        /*
        SDL_Rect SquareBlock;
        SDL_Rect TBlock;
        SDL_Rect LBlock;
        SDL_Rect BackwardsLBlock;
        SDL_Rect StraightBlock;
        SDL_Rect SBlock;
        SDL_Rect BackwardsSBlock;

        SquareBlock.x = 0;
        SquareBlock.y = 0;
        SquareBlock.w = 32;
        SquareBlock.h = 32;

        TBlock.x = 0;
        TBlock.y = 0;
        TBlock.w = 32;
        TBlock.h = 32;

        LBlock.x = 0;
        LBlock.y = 0;
        LBlock.w = 32;
        LBlock.h = 32;

        BackwardsLBlock.x = 0;
        BackwardsLBlock.y = 0;
        BackwardsLBlock.w = 32;
        BackwardsLBlock.h = 32;

        StraightBlock.x = 0;
        StraightBlock.y = 0;
        StraightBlock.w = 32;
        StraightBlock.h = 32;

        SBlock.x = 0;
        SBlock.y = 0;
        SBlock.w = 32;
        SBlock.h = 32;

        BackwardsSBlock.x = 0;
        BackwardsSBlock.y = 0;
        BackwardsSBlock.w = 32;
        BackwardsSBlock.h = 32;
        */
	}

	// Remember, SQUARE_MEDIAN represents the distance from the square's center to //
	// its sides. SQUARE_MEDIAN*2 gives us the width and height of our squares.    //
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
				m_CenterY += SQUARE_MEDIAN * 2;
			} break;
		}
	}

	// Accessors //
	int GetCenterX() { return m_CenterX; }
	int GetCenterY() { return m_CenterY; }

	// Mutators //
	void SetCenterX(int x) { m_CenterX = x; }
	void SetCenterY(int y) { m_CenterY = y; }

	void SetColor(int r, int g, int b)
	{
        m_RedColor = 0;
        m_GreenColor = 0;
        m_BlueColor = 0;
	}
};
