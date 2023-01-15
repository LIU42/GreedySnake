#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <SDL.h>
#include <list>

using namespace std;

enum Direct { UP, LEFT, RIGHT, DOWN };

struct Block : SDL_Rect
{
	bool operator == (Block);
};

struct Head : Block
{
	Direct direct;
	Direct directLast;
};

typedef Block Body;
typedef Block Food;

typedef list <Body> BodyList;
typedef list <Food> FoodList;

class Snake
{
	public:
		static const int INIT_LENGTH = 10;

	private:
		Head head;
		BodyList body;
		Block temp;
		Block front;
		bool isAlive;

	public:
		void init(int, int);
		void turnTo(Direct);
		void move();
		void eat();

	public:
		BodyList::iterator getBodyBegin();
		BodyList::iterator getBodyEnd();

	public:
		int getHeadX();
		int getHeadY();
		int getLength();
		bool getIsAlive();
		bool getIsCrashed(int, int);
};
#endif