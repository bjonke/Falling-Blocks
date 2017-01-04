//////////////////////////////////////////////////////////////////////////////////
// cSquare.h
// - represents an individual square in Falling Blocks
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Enums.h"
#include "Defines.h"
#include <SDL2/SDL.h> // Used for SDL_Color

class cSquare
{
private:
	// Location of the center of the square //
	int m_CenterX;
	int m_CenterY;

	// Type of block. Needed to locate the correct square in our bitmap //
	BlockType m_BlockType;

    SDL_Color m_RGB;

public:
	// Default constructor, your compiler will probably require this //
	cSquare()
	{
        std::cout << "Constructing a square..." << std::endl;
	}

	// Main constructor takes location and type of block, //
	// and pointer to our bitmap surface. //
	cSquare(int x, int y, SDL_Surface* bitmap, BlockType type, SDL_Color rgb) : m_CenterX(x), m_CenterY(y),
		m_BlockType(type), m_RGB(rgb)
	{
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
                source = { RED_SQUARE_X, RED_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
			} break;
		case T_BLOCK:
			{
                source = { PURPLE_SQUARE_X, PURPLE_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
			} break;
		case L_BLOCK:
			{
                source = { GREY_SQUARE_X, GREY_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
			} break;
		case BACKWARDS_L_BLOCK:
			{
                source = { BLUE_SQUARE_X, BLUE_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
			} break;
		case STRAIGHT_BLOCK:
			{
                source = { GREEN_SQUARE_X, GREEN_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
			} break;
		case S_BLOCK:
			{
                source = { BLACK_SQUARE_X, BLACK_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
			} break;
		case BACKWARDS_S_BLOCK:
			{
                source = { YELLOW_SQUARE_X, YELLOW_SQUARE_Y, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };
			} break;
		}

		// Draw at square's current location. Remember that m_X and m_Y store the center of the square. //
		//SDL_Rect destination = { m_CenterX - SQUARE_MEDIAN, m_CenterY - SQUARE_MEDIAN, SQUARE_MEDIAN * 2, SQUARE_MEDIAN * 2 };

        //SDL_SetRenderDrawColor(MainRenderer, m_RedColor, m_GreenColor, m_BlueColor, 255);
        SDL_SetRenderDrawColor(MainRenderer, m_RGB.r, m_RGB.g, m_RGB.b, 255);
        SDL_Rect rectangle;

        rectangle.x = m_CenterX - SQUARE_MEDIAN;
        rectangle.y = m_CenterY - SQUARE_MEDIAN;
        rectangle.w = SQUARE_MEDIAN * 2;
        rectangle.h = SQUARE_MEDIAN * 2;

        SDL_RenderFillRect(MainRenderer, &rectangle);
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
};
