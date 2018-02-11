//Written by Matthew Markowitz
#include <SDL.h>
#include <math.h>
#include <vector>
#include <fstream>

using namespace std;

const int SCREEN_HEIGHT = 1080;
const int SCREEN_WIDTH = 1920;
enum
{
	TYPE_LOW_BOUNCY,
	TYPE_MEDIUM_BOUNCY,
	TYPE_HIGH_BOUNCY,
	TYPE_FINISH_SQUARE,
	TYPE_BEGINNING_AREA,
};

class obstacle
{
private:
	int type;
	SDL_Rect position;
public:
	obstacle(int x, int y, int w, int h, int mode)
	{
		position.x = x;
		position.y = y;
		position.w = w;
		position.h = h;

		type = mode;
	}
	int getType()
	{
		return type;
	}
	SDL_Rect getPosition()
	{
		return position;
	}
};

int main (int argc, char* args[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface * window = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,32,SDL_SWSURFACE | SDL_FULLSCREEN);

	SDL_Event mainEvent;

	bool quit = false;

	int initialX = 0;
	int initialY = 0;
	
	int finalX = 0;
	int finalY = 0;

	//what obstacles will be placed.
	int mode = TYPE_BEGINNING_AREA;

	vector<obstacle> obstacleHolder;

	bool buttondown = false;

	while (!quit)
	{
		SDL_PollEvent(&mainEvent);
		if (mainEvent.type == SDL_QUIT || mainEvent.key.keysym.sym == SDLK_ESCAPE)
		{
			quit = true;
		}
		else if (mainEvent.type == SDL_KEYDOWN)
		{
			switch (mainEvent.key.keysym.sym)
			{
				case SDLK_1:
					mode = TYPE_BEGINNING_AREA;
					break;
				case SDLK_2:
					mode = TYPE_FINISH_SQUARE;
					break;
				case SDLK_3:
					mode = TYPE_LOW_BOUNCY;
					break;
				case SDLK_4:
					mode = TYPE_MEDIUM_BOUNCY;
					break;
				case SDLK_5:
					mode = TYPE_HIGH_BOUNCY;
					break;
			}
		}
		else if (mainEvent.type == SDL_MOUSEBUTTONDOWN)
		{
			buttondown = true;
			initialX = mainEvent.motion.x;
			initialY = mainEvent.motion.y;

			finalX = initialX;
			finalY = initialY;
		}

		else if (mainEvent.type == SDL_MOUSEMOTION && buttondown)
		{
			finalX = mainEvent.motion.x;
			finalY = mainEvent.motion.y;
		}
		else if (mainEvent.type == SDL_MOUSEBUTTONUP)
		{
			buttondown = false;
			if (mode == TYPE_BEGINNING_AREA)
			{
				obstacleHolder.insert(obstacleHolder.begin(),obstacle(initialX < finalX ? initialX : finalX, initialY < finalY ? initialY : finalY, abs(finalX - initialX), abs(finalY - initialY), mode));
			}
			else
			{
				obstacleHolder.push_back(obstacle(initialX < finalX ? initialX : finalX, initialY < finalY ? initialY : finalY, abs(finalX - initialX), abs(finalY - initialY), mode));
			}
			mainEvent.type = NULL;
			finalX = initialX;
			finalY = initialY;
		}
		else if (mainEvent.key.keysym.sym == SDLK_SPACE)
		{
			//export the level
			ofstream exportFile("Level.lev");
			for (int i = 0; i < obstacleHolder.size(); i++)
			{
				exportFile<<obstacleHolder[i].getPosition().x<<" "<<obstacleHolder[i].getPosition().y<<" "<<obstacleHolder[i].getPosition().w<<" "<<obstacleHolder[i].getPosition().h<<" "<<obstacleHolder[i].getType()<<"\n";
			}
			exportFile.close();
		}

		SDL_FillRect(window,&(window->clip_rect),SDL_MapRGB(window->format, 255,255,255));

		for (int i = 0; i < obstacleHolder.size(); i++)
		{
			SDL_Rect offset = obstacleHolder[i].getPosition();
			switch (obstacleHolder[i].getType())
			{
			case TYPE_LOW_BOUNCY:
				SDL_FillRect(window,&offset, SDL_MapRGB(window->format, 0, 183, 4));
				break;
			case TYPE_MEDIUM_BOUNCY:
				SDL_FillRect(window,&offset, SDL_MapRGB(window->format, 197, 215, 0));
				break;
			case TYPE_HIGH_BOUNCY:
				SDL_FillRect(window,&offset, SDL_MapRGB(window->format, 212, 0, 0));
				break;
			case TYPE_FINISH_SQUARE:
				SDL_FillRect(window,&offset,SDL_MapRGB(window->format, 142, 96, 75));
				break;
			case TYPE_BEGINNING_AREA:
				SDL_FillRect(window,&offset, SDL_MapRGB(window->format, 229, 229, 229));
				break;
			}
		}

		SDL_Rect instanceRect;

		instanceRect.x = initialX < finalX ? initialX : finalX;
		instanceRect.y = initialY < finalY ? initialY : finalY;
		instanceRect.w = abs(finalX - initialX);
		instanceRect.h = abs(finalY - initialY);

		switch (mode)
		{
		case TYPE_LOW_BOUNCY:
			SDL_FillRect(window,&instanceRect, SDL_MapRGB(window->format, 0, 183, 4));
			break;
		case TYPE_MEDIUM_BOUNCY:
			SDL_FillRect(window,&instanceRect, SDL_MapRGB(window->format, 197, 215, 0));
			break;
		case TYPE_HIGH_BOUNCY:
			SDL_FillRect(window,&instanceRect, SDL_MapRGB(window->format, 212, 0, 0));
			break;
		case TYPE_FINISH_SQUARE:
			SDL_FillRect(window,&instanceRect,SDL_MapRGB(window->format, 142, 96, 75));
			break;
		case TYPE_BEGINNING_AREA:
			SDL_FillRect(window,&instanceRect, SDL_MapRGB(window->format, 229, 229, 229));
			break;
		}
		SDL_Flip(window);
	}
	SDL_Quit();
	return 0;
}