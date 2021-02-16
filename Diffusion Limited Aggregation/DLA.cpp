#include <SFML/Graphics.hpp>

#include <iostream>
#include <time.h>
#include <vector>
#include <algorithm>

#include <list>

//#include <thread>

static unsigned NUM_SEEDS = 15, NUM_MOVERS = 300000, ADDMOVERS = 10, GRID_H = 1000, TOPINDEX_H = GRID_H - 1, GRID_W = 1000, TOPINDEX_W = GRID_W - 1;

using namespace std;

//thread T1, T2, T3;

static vector<unsigned> RANDOM(NUM_MOVERS * 2);

static vector<unsigned>::iterator RANDOM_ITR = RANDOM.begin();

static const unsigned SIZE = GRID_W * GRID_H;
sf::VertexArray VERTEXARRAY(sf::Points, SIZE);

const uint64_t SEED = time(0);

uint64_t _seed{ SEED }; /* The state can be seeded with any value. */
uint64_t _next()
{
	uint64_t z = (_seed += UINT64_C(0x9E3779B97F4A7C15));	// <- Not my code.
	z = (z ^ (z >> 30))* UINT64_C(0xBF58476D1CE4E5B9);
	z = (z ^ (z >> 27))* UINT64_C(0x94D049BB133111EB);
	return z ^ (z >> 31);
}

sf::Color COLORS[100000];
sf::Color* COLORSBEGIN = COLORS;
sf::Color* COLORSITR = COLORS;

class DLA
{

private:

	unsigned _x, _y, _xMove, _yMove;

	list<pair<unsigned, unsigned>> _movers;
	list<pair<unsigned, unsigned>>::iterator _index;

	vector<vector<bool>> _grid;

	vector<vector<sf::Color*>> _aggregated;

	vector<pair<unsigned, unsigned>> _anim;

	sf::Color* _targetColor;

	vector<vector<pair<unsigned, unsigned>>> _empty;
	vector<vector<unsigned>> _emptyX;		// Maps to _empty, keeps indices static during erasures.
	vector<unsigned> _emptyY;				// x vector allows x deletes and y vector allows y deletes.

	size_t _numMovers, _numSeeds;

public:

	DLA() = delete;

	DLA(unsigned numMovers, unsigned numSeeds);

	void iterateClassic();

	void draw(unsigned x, unsigned y)
	{
		_grid[y][x] = 1;
		*_aggregated[y][x] = { 160 , 60 , 230 };
	}

	unsigned getMovers() { return _movers.size(); };

	void animateClassic();

	void makeSeed(const unsigned num, const bool grav);
	void makeMover(const unsigned num);

	void erase(const unsigned x, const unsigned y);

};

DLA::DLA(unsigned numMovers, unsigned numSeeds) : _numMovers{ numMovers }, _numSeeds{ numSeeds }
{
	for (unsigned i = 0; i < GRID_H; i++)
	{
		_empty.push_back(vector<pair<unsigned, unsigned>>());
		_emptyX.push_back(vector<unsigned>());
		_emptyY.push_back(i);
		for (unsigned j = 0; j < GRID_W; j++)
		{
			_empty[i].push_back({ j , i });
			_emptyX[i].push_back(j);
		}
	}
	_aggregated.resize(GRID_H, vector<sf::Color*>(GRID_W, COLORSBEGIN));
	_grid.resize(GRID_H, vector<bool>(GRID_W, 0));
	makeSeed(_numSeeds, 0);
	makeMover(_numMovers);

	COLORSITR = COLORSBEGIN;
	sf::Uint8 r = 160, g = 60, b = 230, pos = 2;

	//for (unsigned i = 1; i < 10000 + 1; i++)
	//{
	//	r = 0, g = 0, b = 0;
	//	if ((i / 255) % 2)
	//	{
	//		r = 255 - (i % 256);
	//		b = 255;
	//	}
	//	else
	//	{
	//		for (unsigned j = 0; j < i; j++)
	//		{
	//			r += 9;
	//			if (r > 255)
	//			{
	//				r = 0;
	//				b += 11;
	//				if (b > 255)
	//				{
	//					b = 0;
	//				}
	//			}
	//		}
	//		//r = (i % 256);
	//		//b = r;
	//	}

	//	COLORS[i] = sf::Color{ r, g, b };
	//}

	for (unsigned i = 0; i < 100000; i++)
	{
		switch (pos)
		{
		case 0:

			if (g != 255)
				g++;
			else if (r != 0)
				r--;
			else
				pos++;

			break;
		case 1:

			if (b != 255)
				b++;
			else if (g != 0)
				g--;
			else
				pos++;

			break;
		case 2:

			if (r != 255)
				r++;
			else if (b != 0)
				b--;
			else
				pos = 0;

			break;
		}
		*COLORSITR = sf::Color{ r, g, b };
		COLORSITR++;
	}

	COLORSITR = COLORSBEGIN;
}

void DLA::animateClassic()
{
	for (auto& i : _anim)
		VERTEXARRAY[(i.second * GRID_H) + i.first].color = *(++_aggregated[i.second][i.first]);
}

void DLA::makeMover(const unsigned num)
{
	if (_empty.size())
		for (unsigned i = 0; i < num; i++)
			_movers.push_back(_empty[_next() % _empty.size()][_x = _next() % _empty[_y].size()]);
}

void DLA::makeSeed(const unsigned num, const bool grav)
{
	for (unsigned i = 0; i < num; i++)
	{
		_y = _next() % _empty.size();
		_x = _next() % _empty[_y].size();

		if (grav)
			_y = GRID_H - 1;

		_grid[_y][_x] = 1;
		*_aggregated[_y][_x] = sf::Color{ 160 , 60 , 230 };
		VERTEXARRAY[(_y * GRID_H) + _x].color = sf::Color{ _aggregated[_y][_x]->r, _aggregated[_y][_x]->g, _aggregated[_y][_x]->b };

		erase(_x, _y);

		_anim.push_back(pair<unsigned, unsigned>{ _x, _y });
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

void DLA::iterateClassic()
{
	_index = _movers.begin();
	_numMovers = _movers.size();

	for (unsigned i = 0; i < _numMovers; i++)
	{
		_x = _index->first;
		_y = _index->second;
		switch (*(++RANDOM_ITR))
		{
		case 0:
		{

			if (_x == TOPINDEX_W)
			{
				if (_grid[_y][0])
				{
					_grid[_y][_x] = 1;
					_index = _movers.erase(_index);
					--_numMovers;

					_aggregated[_y][_x] = _aggregated[_y][0] + 1;
					VERTEXARRAY[(_y * GRID_H) + _x].color = *_aggregated[_y][_x];
					_anim.push_back({ _x, _y });
					break;
				}
				else
				{
					_index->first = 0;
				}
			}
			else
			{
				if (_grid[_y][_x + 1])
				{
					_grid[_y][_x] = 1;
					_index = _movers.erase(_index);
					--_numMovers;

					_aggregated[_y][_x] = _aggregated[_y][_x + 1] + 1;
					VERTEXARRAY[(_y * GRID_H) + _x].color = *_aggregated[_y][_x];
					_anim.push_back({ _x, _y });
					break;
				}
				else
				{
					++(_index->first);
				}
			}

			++_index;

			break;
		}
		case 1:
		{
			if (_x == 0)
			{
				if (_grid[_y][TOPINDEX_W])
				{
					_grid[_y][_x] = 1;
					_index = _movers.erase(_index);
					--_numMovers;

					_aggregated[_y][_x] = _aggregated[_y][TOPINDEX_W] + 1;
					VERTEXARRAY[(_y * GRID_H) + _x].color = *_aggregated[_y][_x];
					_anim.push_back({ _x, _y });
					break;
				}
				else
				{
					_index->first = TOPINDEX_W;
				}
			}
			else
			{
				if (_grid[_y][_x - 1])
				{
					_grid[_y][_x] = 1;
					_index = _movers.erase(_index);
					--_numMovers;

					_aggregated[_y][_x] = _aggregated[_y][_x - 1] + 1;
					VERTEXARRAY[(_y * GRID_H) + _x].color = *_aggregated[_y][_x];
					_anim.push_back({ _x, _y });
					break;
				}
				else
				{
					--(_index->first);
				}
			}

			++_index;

			break;
		}
		case 2:
		{
			if (_y == TOPINDEX_H)
			{
				if (_grid[0][_x])
				{
					_grid[_y][_x] = 1;
					_index = _movers.erase(_index);
					--_numMovers;

					_aggregated[_y][_x] = _aggregated[0][_x] + 1;
					VERTEXARRAY[(_y * GRID_H) + _x].color = *_aggregated[_y][_x];
					_anim.push_back({ _x, _y });
					break;
				}
				else
				{
					_index->second = 0;
				}
			}
			else
			{
				if (_grid[_y + 1][_x])
				{
					_grid[_y][_x] = 1;
					_index = _movers.erase(_index);
					_numMovers--;

					_aggregated[_y][_x] = _aggregated[_y + 1][_x] + 1;
					VERTEXARRAY[(_y * GRID_H) + _x].color = *_aggregated[_y][_x];
					_anim.push_back({ _x, _y });
					break;
				}
				else
				{
					++(_index->second);
				}
			}

			++_index;

			break;
		}
		case 3:
		{
			if (_y == 0)
			{
				if (_grid[TOPINDEX_H][_x])
				{
					_grid[_y][_x] = 1;
					_index = _movers.erase(_index);
					--_numMovers;

					_aggregated[_y][_x] = _aggregated[TOPINDEX_H][_x] + 1;
					VERTEXARRAY[(_y * GRID_H) + _x].color = *_aggregated[_y][_x];
					_anim.push_back({ _x, _y });
					break;
				}
				else
				{
					_index->second = TOPINDEX_H;
				}
			}
			else
			{
				if (_grid[_y - 1][_x])
				{
					_grid[_y][_x] = 1;
					_index = _movers.erase(_index);
					--_numMovers;

					_aggregated[_y][_x] = _aggregated[_y - 1][_x] + 1;
					VERTEXARRAY[(_y * GRID_H) + _x].color = *_aggregated[_y][_x];
					_anim.push_back({ _x, _y });
					break;
				}
				else
				{
					--(_index->second);
				}
			}

			++_index;

			break;
		}
		default:
		{
			break;
		}
		}
	}
}

static DLA D1 = DLA{ NUM_MOVERS, NUM_SEEDS };
static bool RESET = 0;

int main()
{
	cout << "Seed: " << SEED << endl;
	for (unsigned i = 0; i < RANDOM.size(); i++)
	{
		RANDOM[i] = (_next() % 4);
	}
	for (unsigned i = 0; i < GRID_H; i++)
	{
		for (unsigned j = 0; j < GRID_W; j++)
		{
			VERTEXARRAY[(i * GRID_H) + j] = sf::Vertex{ sf::Vector2f{ (float)j , (float)i }, sf::Color{ 0 , 0 , 0 } };
		}
	}

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	static sf::RenderWindow window(sf::VideoMode{ GRID_W , GRID_H }, "DLA", sf::Style::Default, settings);

	sf::Event event;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				window.close();
				break;
			}
			case sf::Event::KeyPressed:
			{
				sf::Vector2i position = sf::Mouse::getPosition(window);
				VERTEXARRAY[(position.y * GRID_H) + position.x].color = sf::Color{ 160 , 60 , 230 };
				D1.draw(position.x, position.y);
				break;
			}
			default:
				break;
			}
		}

		if (D1.getMovers() < 100)
		{
			D1.animateClassic();
		}
		else
		{
			RANDOM_ITR = RANDOM.begin() - 1;
			D1.iterateClassic();
		}


		window.draw(VERTEXARRAY);
		window.display();
	}

	return 0;
}
