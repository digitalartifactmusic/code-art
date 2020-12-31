#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
#include <complex>

#include <thread>

static const unsigned MAX_THREADS = std::thread::hardware_concurrency();;
std::vector<std::thread> THREADS(MAX_THREADS);

static const bool JULIA = false;

static const unsigned MAX_ITERATIONS = 1000;

static const unsigned HEIGHT = 1000;
static const unsigned WIDTH = 1000;
static const double RATIO = WIDTH / HEIGHT;

static const long double HEIGHTDIV = (long double)HEIGHT / 2.0;
static const long double WIDTHDIV = (long double)WIDTH / 2.0;

static long double PLANEDIV_HEIGHT = 2.0;
static long double PLANEDIV_WIDTH = 2.0 * RATIO;

static long double ITERATE_HEIGHT = PLANEDIV_HEIGHT / HEIGHTDIV;
static long double ITERATE_WIDTH = PLANEDIV_WIDTH / WIDTHDIV;

static std::vector<sf::Color> COLORS = {};

void colorInit()
{
	sf::Uint8 r = 0, g = 0, b = 0;
	for (unsigned i = 0; i < MAX_ITERATIONS; i++)
	{
		r = 0, g = 0, b = 0;
		if ((i / 255) % 2)
		{
			r = 255 - (i % 256);
			b = 255;
		}
		else
		{
			for (unsigned j = 0; j < i; j++)
			{
				r += 9;
				if (r > 255)
				{
					r = 0;
					b += 11;
					if (b > 255)
					{
						b = 0;
					}
				}
			}
		}

		COLORS.push_back(sf::Color{ r, g, b });
	}
}

class Point
{
public:
	std::complex<long double> comp;
	unsigned pos;
	float x, y;

	Point() = delete;

	Point(std::complex<long double> imag, unsigned pos, float x, float y) : comp{ imag }, pos{ pos }, x{ x }, y{ y } {}

};
static std::vector<Point> SCREENSPACE;

static const std::complex<long double> JULIAPOINT = std::complex<long double>{ -0.8 , -0.156 };

void initialize()
{
	unsigned k = 0;
	long double y = -PLANEDIV_HEIGHT;
	for (unsigned i = 0; i < HEIGHT; i++)
	{
		long double x = -PLANEDIV_WIDTH;
		for (unsigned j = 0; j < WIDTH; j++)
		{
			SCREENSPACE.push_back(Point{ std::complex<long double>{ x ,  y }, k, (float)j, (float)i });
			k++;
			x += ITERATE_WIDTH;
		}
		y += ITERATE_HEIGHT;
	}
}

void iterate(sf::VertexArray& vertexarray, const unsigned thread)
{
	switch (JULIA)
	{
		case false:
		{
			unsigned i = thread;
			while (1)
			{
				Point& point = SCREENSPACE[i += MAX_THREADS];
					std::complex<long double> k = point.comp, t = k;
					for (unsigned l = 0; l < MAX_ITERATIONS; l++)
					{
						k = (k * k) + t;
						if (((k.real() * k.real()) + (k.imag() * k.imag())) >= 4)
						{
							std::vector<sf::Color>::iterator COLORITR = COLORS.begin() + l;
							vertexarray[i].position = sf::Vector2f{ point.x , point.y };
							vertexarray[i].color = *COLORITR;
							break;
						}
					}

				if (i >= SCREENSPACE.size())
				{
					break;
				}
			}
			break;
		}
		case true:
		{
			unsigned i = thread;
			while (1)
			{
				Point& point = SCREENSPACE[i += MAX_THREADS];
					std::complex<long double> k = point.comp;
					for (unsigned l = 0; l < MAX_ITERATIONS; l++)
					{
						k = (k * k) + JULIAPOINT;
						if (((k.real() * k.real()) + (k.imag() * k.imag())) >= 4)
						{
							std::vector<sf::Color>::iterator COLORITR = COLORS.begin() + l;
							vertexarray[i].position = sf::Vector2f{ point.x , point.y };
							vertexarray[i].color = *COLORITR;
							break;
						}
					}

				if (i >= SCREENSPACE.size())
				{
					break;
				}
			}
			break;
		}
	}
}

void zoom(long double magnification, const long double& coordX, const long double& coordY)
{
	std::thread t1([&]
	{
		ITERATE_HEIGHT *= magnification;
	});
	std::thread t2([&]
	{
		ITERATE_WIDTH *= magnification;
	});
	std::thread t3([&]
	{
		PLANEDIV_HEIGHT *= magnification;
	});
	std::thread t4([&]
	{
		PLANEDIV_WIDTH *= magnification;
	});

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	THREADS.clear();
	for (unsigned i = 0; i < MAX_THREADS; i++)
	{
		THREADS.emplace_back(std::thread([i, &coordX, &coordY] 
		{
			unsigned j = i;
			while (1)
			{
				Point& point = SCREENSPACE[j += MAX_THREADS];

				point.comp = std::complex<long double>{ -PLANEDIV_WIDTH + ((unsigned)point.x * ITERATE_WIDTH) + coordX,  -PLANEDIV_HEIGHT + ((unsigned)point.y * ITERATE_HEIGHT) + coordY};

				if (j >= SCREENSPACE.size())
					break;
			}
		}));
	}
	for (auto& th : THREADS)
		th.join();
}

int main()
{
	colorInit();

	initialize();

	long double currentZoom = 1.0;

	sf::RenderWindow window(sf::VideoMode{ WIDTH , HEIGHT }, "Mandelbrot");

	const unsigned vertexArrSize = WIDTH * HEIGHT;

	sf::VertexArray vertexArray(sf::Points, vertexArrSize);

	long double plusX = 0.267235642726, plusY = 0.003347589624;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		THREADS.clear();
		for (unsigned i = 0; i < MAX_THREADS; i++)
		{
			THREADS.emplace_back(std::thread([i, &vertexArray, &vertexArrSize]
			{
				unsigned j = i;
				while (1)
				{
					vertexArray[j += MAX_THREADS].color = sf::Color::Black;

					if (j >= vertexArrSize)
						break;
				}
			}));
		}
		for (auto& th : THREADS)
			th.join();

		zoom(0.9, plusX, plusY);

		THREADS.clear();
		for (unsigned i = 0; i < MAX_THREADS; i++)
			THREADS.emplace_back(std::thread(iterate, std::ref(vertexArray), i));
		for (auto& th : THREADS)
			th.join();

		window.draw(vertexArray);
		window.display();
	}

	return 0;
}
