#pragma once

class Point
{

private:

	unsigned _x, _y;

public:

	Point(unsigned x, unsigned y);

	unsigned getX() const;
	unsigned getY() const;

	friend bool operator == (const Point& p1, const Point& p2);

	void downX();
	void downY();
	void upX();
	void upY();

};