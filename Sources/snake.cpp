#include "snake.h"

bool Block::operator == (Block block)
{
	return this->x == block.x && this->y == block.y;
}

void Snake::init(int tableRows, int tableCols)
{
	head.x = tableRows / 2;
	head.y = tableCols / 2;
	head.direct = RIGHT;
	head.directLast = RIGHT;
	isAlive = true;
	body.clear();

	for (int i = 1; i < INIT_LENGTH; i++)
	{
		temp.x = head.x - i;
		temp.y = head.y;
		body.push_back(temp);
	}
}

void Snake::turnTo(Direct direct)
{
	if (direct + head.directLast != 3)
	{
		head.direct = direct;
	}
}

void Snake::move()
{
	if (isAlive)
	{
		temp = head;

		switch (head.direct)
		{
			case UP: head.y -= 1; break;
			case DOWN: head.y += 1; break;
			case LEFT: head.x -= 1; break;
			case RIGHT: head.x += 1; break;
		}
		body.insert(body.begin(), temp);
		body.pop_back();
		head.directLast = head.direct;
	}
}

void Snake::eat()
{
	body.push_back(temp);
}

int Snake::getHeadX()
{
	return head.x;
}

int Snake::getHeadY()
{
	return head.y;
}

int Snake::getLength()
{
	return (int)body.size() + 1;
}

bool Snake::getIsAlive()
{
	return isAlive;
}

bool Snake::getIsCrashed(int tableRows, int tableCols)
{
	front = head;

	switch (head.direct)
	{
		case UP: front.y -= 1; break;
		case DOWN: front.y += 1; break;
		case LEFT: front.x -= 1; break;
		case RIGHT: front.x += 1; break;
	}
	if (front.x < 0 || front.x >= tableRows || front.y < 0 || front.y >= tableCols)
	{
		isAlive = false;
		return true;
	}
	for (auto it = body.begin(); it != body.end(); ++it)
	{
		if (it->x == front.x && it->y == front.y)
		{
			isAlive = false;
			return true;
		}
	}
	return false;
}

list <Body>::iterator Snake::getBodyBegin()
{
	return body.begin();
}

list <Body>::iterator Snake::getBodyEnd()
{
	return body.end();
}