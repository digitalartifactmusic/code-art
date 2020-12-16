#pragma once

#include <vector>
#include <algorithm>

using namespace std;

class Color
{
private:

	std::vector<unsigned> _color;

	unsigned _region;

	unsigned _pos;
	unsigned _cap;
	unsigned _base;

public:

	Color() = delete;

	Color(unsigned region, unsigned r, unsigned g, unsigned b);

	Color(unsigned region, unsigned r, unsigned g, unsigned b, unsigned pos, unsigned cap, unsigned base);

	unsigned getR() const;
	unsigned getG() const;
	unsigned getB() const;

	unsigned getPos() const;
	
	unsigned getRegion() const;

	void setR(const unsigned r);
	void setG(const unsigned g);
	void setB(const unsigned b);

	void colorInit();

	void advanceSecondary();
	void advanceSecondary(unsigned amount);

};