#pragma once

#include "Point.h"
#include "Color.h"
#include <Windows.h>
#include <time.h>

static const unsigned NUM_SEEDS = 15, NUM_MOVERS = 500000, ITERATIONS = 2000000, ADDMOVERS = 1, GRID_H = 1000, TOPINDEX_H = GRID_H - 1, GRID_W = 2000, TOPINDEX_W = GRID_W - 1;

class DLA
{

private:

	unsigned pos, _x, _y, _xMove, _yMove, _t;

	vector<Point> _movers;
	vector<Point>::iterator _index;

	vector<vector<bool>> _grid;

	vector<vector<Color>> _aggregated;

	vector<vector<Point>> _anim; //

	vector<Color>::iterator _targetColor;

	vector<vector<Point>> _empty;
	vector<vector<unsigned>> _emptyX;		// Maps to _empty, keeps indices static during erasures.
	vector<unsigned> _emptyY;				// x vector allows x deletes and y vector allows y deletes.

	size_t _numMovers, _numSeeds;

	HWND _consoleHandle = GetConsoleWindow();

	HDC _deviceContext = GetDC(_consoleHandle);

	uint64_t _seed{ (uint64_t)time(0) }; /* The state can be seeded with any value. */
	uint64_t _next()
	{
		uint64_t z = (_seed += UINT64_C(0x9E3779B97F4A7C15));	// <- Not my code.
		z = (z ^ (z >> 30))* UINT64_C(0xBF58476D1CE4E5B9);
		z = (z ^ (z >> 27))* UINT64_C(0x94D049BB133111EB);
		return z ^ (z >> 31);
	}

public:

	DLA() = delete;

	DLA(unsigned numMovers, unsigned numSeeds);

	void iterateClassicFill();
	
	void animateClassic();

	void makeSeed(const unsigned num, const bool grav);
	void makeMover(const unsigned num);

	void erase(const unsigned x, const unsigned y);

	~DLA();
};