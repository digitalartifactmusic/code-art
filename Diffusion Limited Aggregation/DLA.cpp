#include "DLA.h"

DLA::DLA(unsigned numMovers, unsigned numSeeds) : _numMovers{ numMovers }, _numSeeds{ numSeeds }
{
	for (unsigned i = 0; i < GRID_H; i++)
	{
		_empty.push_back(vector<Point>());
		_emptyX.push_back(vector<unsigned>());
		_emptyY.push_back(i);
		for (unsigned j = 0; j < GRID_W; j++)
		{
			_empty[i].push_back(Point{ j , i });
			_emptyX[i].push_back(j);
		}
	}
	_aggregated.resize(GRID_H, vector<Color>(GRID_W, Color{ 0 , 0 , 0 , 0 }));
	_grid.resize(GRID_H, vector<bool>(GRID_W, 0));
	_movers.reserve(GRID_W* GRID_H);
	makeSeed(_numSeeds, 0);
	makeMover(_numMovers);
}

void DLA::animateClassic()
{
	for (unsigned k = 0; k < 100; k++)
	{
		for (unsigned i = 0; i < _anim.size(); i++)
		{
			for (unsigned j = 0; j < _anim[i].size(); j++)
			{
				_y = _anim[i][j].getY();
				_x = _anim[i][j].getX();
				_targetColor = _aggregated[_y].begin() + _x;
				_targetColor->advanceSecondary(144);
				SetPixel(_deviceContext, _x, _y, RGB(_targetColor->getR(), _targetColor->getG(), _targetColor->getB()));
			}
		}
	}
}

void DLA::makeMover(const unsigned num)
{
	if (_empty.size())
	{
		for (unsigned i = 0; i < num; i++)
		{
			_y = _next() % _empty.size();
			_x = _next() % _empty[_y].size();

			_movers.push_back(Point{ _empty[_y][_x].getX() , _empty[_y][_x].getY() });
		}
	}
}

void DLA::makeSeed(const unsigned num, const bool grav)
{
	for (unsigned i = 0; i < num; i++)
	{
		_y = _next()% _empty.size();
		_x = _next()% _empty[_y].size();

		if (grav)
			_y = GRID_H - 1;

		_grid[_y][_x] = 1;
		_aggregated[_y][_x] = Color{ i , 160 , 60 , 220 };
		SetPixel(_deviceContext, _x, _y, RGB(_aggregated[_y][_x].getR(), _aggregated[_y][_x].getG(), _aggregated[_y][_x].getB()));

		erase(_x, _y);

		_anim.push_back(vector<Point>());
		_anim[i].push_back( Point{ _x , _y } );
	}


}

void DLA::erase(const unsigned x, const unsigned y)
{
	_xMove = _emptyX[y][x];
	_yMove = _emptyY[y];

	_empty[_yMove].erase(_empty[_yMove].begin() + _xMove);

	for (unsigned i = x; i < GRID_W; i++)
		_emptyX[y][i]--;


	if (!_empty[_yMove].size())
	{
		_empty.erase(_empty.begin() + _yMove);

		for (unsigned i = y; i < GRID_H; i++)
			_emptyY[i]--;
	}
}

DLA::~DLA()
{
	ReleaseDC(_consoleHandle, _deviceContext);
}

void DLA::iterateClassicFill()	// Not adding any movers. Not checking mover space aggregation. Not updating number of movers each iteration. Not preparing erased vector for random access.
{
	makeMover(ADDMOVERS);
	_index = _movers.begin();
	_numMovers = _movers.size();

	for (unsigned i = 0; i < _numMovers; i++)
	{
		switch (i % 4)		// This is a rough approximation. Generating random numbers is hard The movers are randomly distributed accross the board though, so it mimicks a random walk fairly well.
		{
		case 0:
		{
			_x = _index->getX();

			if (_x == TOPINDEX_W)
			{
				_index++;
				break;
			}

			_y = _index->getY();

			if (_grid[_y][_x])
			{
				_index = _movers.erase(_index);
				_numMovers--;

				break;
			}

			if (_grid[_y][_x + 1])
			{
				_grid[_y][_x] = 1;
				_index = _movers.erase(_index);
				_numMovers--;

				_targetColor = _aggregated[_y].begin() + _x;
				_anim[_targetColor->getRegion()].push_back(Point{ _x , _y });
				*_targetColor = *(_targetColor + 1);
				_targetColor->advanceSecondary();
				erase(_x, _y);
				SetPixel(_deviceContext, _x, _y, RGB(_targetColor->getR(), _targetColor->getG(), _targetColor->getB()));

				break;
			}

			_index->upX();
			_index++;

			break;
		}

		case 1:
		{
			_x = _index->getX();

			if (!_x)
			{
				_index++;
				break;
			}

			_y = _index->getY();

			if (_grid[_y][_x])
			{
				_index = _movers.erase(_index);
				_numMovers--;

				break;
			}

			if (_grid[_y][_x - 1])
			{
				_grid[_y][_x] = 1;
				_movers.erase(_index);
				_numMovers--;

				_targetColor = _aggregated[_y].begin() + _x;
				_anim[_targetColor->getRegion()].push_back(Point{ _x , _y });
				*_targetColor = *(_targetColor - 1);
				_targetColor->advanceSecondary();
				erase(_x, _y);
				SetPixel(_deviceContext, _x, _y, RGB(_targetColor->getR(), _targetColor->getG(), _targetColor->getB()));

				break;
			}

			_index->downX();
			_index++;

			break;
		}
		case 2:
		{
			_y = _index->getY();

			if (_y == TOPINDEX_H)
			{
				_index++;
				break;
			}

			_x = _index->getX();

			if (_grid[_y][_x])
			{
				_index = _movers.erase(_index);
				_numMovers--;

				break;
			}

			if (_grid[_y + 1][_x])
			{
				_grid[_y][_x] = 1;
				_movers.erase(_index);
				_numMovers--;

				_targetColor = _aggregated[_y].begin() + _x;
				_anim[_targetColor->getRegion()].push_back(Point{ _x , _y });
				*_targetColor = _aggregated[_y + 1][_x];
				_targetColor->advanceSecondary();
				erase(_x, _y);
				SetPixel(_deviceContext, _x, _y, RGB(_targetColor->getR(), _targetColor->getG(), _targetColor->getB()));

				break;
			}

			_index->upY();
			_index++;

			break;
		}
		case 3:
		{
			_y = _index->getY();

			if (!_y)
			{
				_index++;
				break;
			}

			_x = _index->getX();

			if (_grid[_y][_x])
			{
				_index = _movers.erase(_index);
				_numMovers--;

				break;
			}

			if (_grid[_y - 1][_x])
			{
				_grid[_y][_x] = 1;
				_movers.erase(_index);
				_numMovers--;

				_targetColor = _aggregated[_y].begin() + _x;
				_anim[_targetColor->getRegion()].push_back(Point{ _x , _y });
				*_targetColor = _aggregated[_y - 1][_x];
				_targetColor->advanceSecondary();
				erase(_x, _y);
				SetPixel(_deviceContext, _x, _y, RGB(_targetColor->getR(), _targetColor->getG(), _targetColor->getB()));

				break;
			}

			_index->downY();
			_index++;

			break;
		}
		default:
			exit(1);
			break;
		}
	}
}