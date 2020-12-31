#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
#include <complex>

#include <thread>

static const bool JULIA = true;

static const unsigned MAX_ITERATIONS = 1000;

static const unsigned HEIGHT = 1000;
static const unsigned WIDTH = 1000;
static const double RATIO = WIDTH / HEIGHT;

static long double PLANE_HEIGHT = 4.0;
static long double PLANEDIV_HEIGHT = PLANE_HEIGHT / 2.0;

static long double PLANE_WIDTH = 4.0 * RATIO;
static long double PLANEDIV_WIDTH = PLANE_WIDTH / 2.0;

static long double ITERATE_HEIGHT = PLANE_HEIGHT / (long double)HEIGHT;
static long double ITERATE_WIDTH = PLANE_WIDTH / (long double)WIDTH;

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
	long double y = -PLANEDIV_HEIGHT;
	for (unsigned i = 0; i < HEIGHT; i++)
	{
		long double x = -PLANEDIV_WIDTH;
		for (unsigned j = 0; j < WIDTH; j++)
		{
			SCREENSPACE.push_back(Point{ std::complex<long double>{ x ,  y }, ((i * HEIGHT) + j), (float)j, (float)i });
			x += ITERATE_WIDTH;
		}
		y += ITERATE_HEIGHT;
	}
}

void iterate(sf::VertexArray& vertexarray, const unsigned thread, const unsigned threads)
{
	switch (JULIA)
	{
		case false:
		{
			unsigned i = thread;
			while (1)
			{
				Point& point = SCREENSPACE[i += threads];
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
		}
		case true:
		{
			unsigned i = thread;
			while (1)
			{
				Point& point = SCREENSPACE[i += threads];
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

void zoom(long double magnification, long double plusX, long double plusY)
{

	PLANE_HEIGHT = 4.0 / magnification;
	PLANEDIV_HEIGHT = PLANE_HEIGHT / 2;

	PLANE_WIDTH = (4.0 * RATIO) / magnification;
	PLANEDIV_WIDTH = PLANE_WIDTH / 2;

	ITERATE_HEIGHT = PLANE_HEIGHT / (long double)HEIGHT;
	ITERATE_WIDTH = PLANE_WIDTH / (long double)WIDTH;

	long double y = -PLANEDIV_HEIGHT;
	for (unsigned i = 0; i < HEIGHT; i++)
	{
		long double x = -PLANEDIV_WIDTH;
		for (unsigned j = 0; j < WIDTH; j++)
		{
			SCREENSPACE[((i * HEIGHT) + j)] = Point{ std::complex<long double>{ x + plusX,  y + plusY }, ((i * HEIGHT) + j), (float)j, (float)i };
			x += ITERATE_WIDTH;
		}
		y += ITERATE_HEIGHT;
	}
}

int main()
{
	colorInit();

	initialize();

	long double currentZoom = 1.0;

	sf::RenderWindow window(sf::VideoMode{ WIDTH , HEIGHT }, "Mandelbrot");

	sf::VertexArray vertexArray(sf::Points, WIDTH * HEIGHT);

	long double plusX = 0.0, plusY = 0.0;

	int maxZoom = 200;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (currentZoom < maxZoom)
		{
			for (int i = 0; i < WIDTH * HEIGHT; i++)
				vertexArray[i].color = sf::Color::Black;

			zoom(currentZoom *= 1.1, plusX, plusY);

			unsigned max = std::thread::hardware_concurrency();;

			std::vector<std::thread> threads;
			for (unsigned i = 1; i <= max; i++)
				threads.emplace_back(std::thread(iterate, std::ref(vertexArray), i, max));
			for (auto& th : threads)
				th.join();

			window.draw(vertexArray);
			window.display();
		}
	}

	return 0;
}
