#include "Point.h"

Point::Point(unsigned x, unsigned y) : _x{ x }, _y{ y } {}

unsigned Point::getX() const { return _x; };
unsigned Point::getY() const { return _y; };

bool operator == (const Point& p1, const Point& p2) { return ((p1._x == p2._x) && (p1._y == p2._y)); };

void Point::downX() { _x--; }
void Point::downY() { _y--; }
void Point::upX() { _x++; }
void Point::upY() { _y++; }