//Written by Matthew Markowitz
using namespace std;
#include <SDL.h>
#include <math.h>
#include <vector>
#include <fstream>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int BOX_DIMENSION = 64;

enum
{
	TYPE_LOW_BOUNCY,
	TYPE_MEDIUM_BOUNCY,
	TYPE_HIGH_BOUNCY,
	TYPE_FINISH_SQUARE,
	TYPE_BEGINNING_AREA,
};

SDL_Surface * window;

class timer
{
private:
	int starttime;
public:
	timer()
	{
		starttime = 0;
	}

	void start()
	{
		starttime = SDL_GetTicks();
	}

	void stop()
	{
		starttime = 0;
	}

	int getTime()
	{
		if (starttime != 0)
		{
			return SDL_GetTicks() - starttime;
		}
		else
		{
			return 0;
		}
	}
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

void apply_rect (int x, int y)
{
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	offset.w = BOX_DIMENSION;
	offset.h = BOX_DIMENSION;
	SDL_FillRect(window, &offset, SDL_MapRGB(window->format, 24, 12, 53));
}
int main (int argc, char* args[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	
	window = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,32, SDL_SWSURFACE | SDL_FULLSCREEN);
	SDL_WM_SetCaption("Physics Game", NULL);

	//whether the user hit escape
	bool quit = false;
	//whether the user has fired out of bounds
	bool outOfBounds = false;
	//whether the user has fired once
	bool fired = false;

	const int acceleration = -1000;
	
	//used as coordinates of vector of velocity
	int initialXPoint = 0;
	int initialYPoint = 0;
	int terminalXPoint = 0;
	int terminalYPoint = 0;

	int xLocation = -BOX_DIMENSION;
	int yLocation = -BOX_DIMENSION;
	int height = 0;

	double XVelocity = 0;
	double initialYVelocity = 0;

	SDL_Event mainEvent;

	timer physicstimerX;
	timer physicstimerY;

	//holds all of the obstacles
	vector<obstacle> obstacleHolder;
	//loading phase goes here
	ifstream importer("Levels/Level.lev");
	int ix = 0, iy = 0, iw = 0, ih = 0, im = 0;
	while(importer>>ix>>iy>>iw>>ih>>im)
	{
		obstacleHolder.push_back(obstacle(ix,iy,iw,ih,im));
	}
	importer.close();

	while (!quit)
	{
		//handle input
		SDL_PollEvent(&mainEvent);
		if (mainEvent.type == SDL_QUIT)
		{
			quit = true;
		}
		else if (mainEvent.type == SDL_KEYDOWN)
		{
			//if the user resets with backspace
			if (mainEvent.key.keysym.sym == SDLK_BACKSPACE)
			{
				outOfBounds = false;
				fired = false;
				initialXPoint = 0;
				initialYPoint = 0;
				terminalXPoint = 0;
				terminalYPoint = 0;

				xLocation = -BOX_DIMENSION;
				yLocation = -BOX_DIMENSION;
				height = 0;

				XVelocity = 0;
				initialYVelocity = 0;

				physicstimerX.stop();
				physicstimerY.stop();
			}

			else if (mainEvent.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
			}
		}
		else if (mainEvent.type == SDL_MOUSEBUTTONDOWN && !fired)
		{
			mainEvent.type = NULL;
			//if the user is clicking in the beginning obstacle
			if ((obstacleHolder[0].getPosition().x < mainEvent.motion.x && obstacleHolder[0].getPosition().x + obstacleHolder[0].getPosition().w > mainEvent.motion.x) && (((obstacleHolder[0].getPosition().y) < mainEvent.motion.y) && obstacleHolder[0].getPosition().y + obstacleHolder[0].getPosition().h > mainEvent.motion.y))
			{
				outOfBounds = false;
				initialXPoint = mainEvent.motion.x;
				initialYPoint = mainEvent.motion.y;
				xLocation = initialXPoint - (BOX_DIMENSION / 2);
				yLocation = initialYPoint - (BOX_DIMENSION / 2);
			}
			else
			{
				outOfBounds = true;
			}
		}
		else if (mainEvent.type == SDL_MOUSEBUTTONUP && !fired && !outOfBounds)
		{
			mainEvent.type = NULL;

			terminalXPoint = mainEvent.motion.x;
			terminalYPoint = mainEvent.motion.y;
			XVelocity = (terminalXPoint - initialXPoint) * 5;
			//the initialYPoint is being subtracted from terminal point because of inverted y coordinates
			initialYVelocity = (initialYPoint - terminalYPoint) * 5;

			fired = true;
			
			physicstimerX.start();
			physicstimerY.start();
		}
		//logic - check collision
		//starts at one to avoid beginning area collission
		for(unsigned int i = 1; i < obstacleHolder.size(); i++)
		{
			//the xposition of cube is:
			//xLocation + XVelocity * (physicstimerX.getTime() / 1000.);

			//the yposition of cube is:
			//yLocation - height;

			int cx = xLocation + XVelocity * (physicstimerX.getTime() / 1000.);
			int cy = yLocation - height;

			double bounciness = 0.;

			//if the boxes have collided
			if (cx + BOX_DIMENSION > obstacleHolder[i].getPosition().x && cx < obstacleHolder[i].getPosition().x + obstacleHolder[i].getPosition().w && cy + BOX_DIMENSION > obstacleHolder[i].getPosition().y && cy < obstacleHolder[i].getPosition().y + obstacleHolder[i].getPosition().h)
			{
				switch (obstacleHolder[i].getType())
				{
			
				case TYPE_LOW_BOUNCY:
					bounciness = -0.4;
					break;
				case TYPE_MEDIUM_BOUNCY:
					bounciness = -0.6;
					break;
				case TYPE_HIGH_BOUNCY:
					bounciness = -0.8;
					break;
				case TYPE_FINISH_SQUARE:
					bounciness = 0;
					break;
				}
				//XVelocity part is if the box slides of an obstacle
				if (obstacleHolder[i].getPosition().x - (cx + BOX_DIMENSION) > -32 && (XVelocity > 0))
				{
					xLocation = cx;
					XVelocity *= bounciness;
					physicstimerX.start();
				}
				else if (obstacleHolder[i].getPosition().x + obstacleHolder[i].getPosition().w - cx < 32 && (XVelocity < 0))
				{
					xLocation = cx + 1;
					XVelocity *= bounciness;
					physicstimerX.start();
				}

				else if (obstacleHolder[i].getPosition().y  - (cy + BOX_DIMENSION) > -32)
				{
					initialYVelocity = bounciness * (initialYVelocity + acceleration * (physicstimerY.getTime() / 1000.));
					//slow down if it hit the finish square
					if (bounciness == 0)
					{
						xLocation += XVelocity * (physicstimerX.getTime() / 1000.);
						XVelocity *= 0.8;
						physicstimerX.start();
					}
					yLocation = cy - 1;
					physicstimerY.start();
				}
				//dont bounce if it hits the bottom
				else
				{
					initialYVelocity = -(initialYVelocity + acceleration * (physicstimerY.getTime() / 1000.));
					yLocation = cy + 1;
					physicstimerY.start();
				}
			}
			

		}
		//render
		SDL_FillRect( window, &window->clip_rect, SDL_MapRGB( window->format, 255, 255, 255 ) );

		//stop it if it gets really slow
		if (abs(XVelocity) < 0.01)
		{
			xLocation += XVelocity * (physicstimerX.getTime() / 1000.);
			XVelocity = 0;
		}

		height = (int) (initialYVelocity * (physicstimerY.getTime() / 1000.) + .5 * acceleration * powf(physicstimerY.getTime()/1000.,2));

		for (unsigned int i = 0; i < obstacleHolder.size(); i++)
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



		apply_rect(xLocation + XVelocity * (physicstimerX.getTime() / 1000.), yLocation - height);

		SDL_Flip(window);
	}
	SDL_Quit();
	return 0;
}