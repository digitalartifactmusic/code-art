#include <iostream>
#include <vector>
#include <Windows.h>

using namespace std;

class Color
{
private:

	std::vector<unsigned> _color;

public:

	Color() = delete;

	Color(unsigned r, unsigned g, unsigned b) : _color{ r, g, b } {}

	unsigned getR() const { return _color[0]; }
	unsigned getG() const { return _color[1]; }
	unsigned getB() const { return _color[2]; }

	void setR(const unsigned r) { _color[0] = r; }
	void setG(const unsigned g) { _color[1] = g; }
	void setB(const unsigned b) { _color[2] = b; }

};

class Complex
{

public:

	long double _realPart;
	long double _complexPart;

public:

	Complex();
	Complex(long double realPart);
	Complex(long double realPart, long double complexPart);

	friend ostream& operator << (ostream& out, const Complex& c);       // Operator overloading.
	friend istream& operator >> (istream& in, Complex& c);

	friend Complex operator + (const Complex& c1, const Complex& c2);
	friend Complex operator - (const Complex& c1, const Complex& c2);
	friend Complex operator * (const Complex& c1, const Complex& c2);

	friend bool operator == (const Complex& c1, const Complex& c2);

};

Complex::Complex() : _realPart{ 0 }, _complexPart{ 0 } {}                    // Default constructor.

Complex::Complex(long double realPart) : _realPart{ realPart }, _complexPart{ 0 } {}  // Parameterized Constructors

Complex::Complex(long double realPart, long double complexPart) : _realPart{ realPart }, _complexPart{ complexPart } {}

ostream& operator << (ostream& out, const Complex& c)
{
	out << c._realPart << " + " << c._complexPart << "i";
	return out;
}
istream& operator >> (istream& in, Complex& c)
{
	in >> c._realPart >> c._complexPart;
	return in;
}
Complex operator + (const Complex& c1, const Complex& c2)
{
	return Complex{ (c1._realPart + c2._realPart), (c1._complexPart + c2._complexPart) };
}
Complex operator - (const Complex& c1, const Complex& c2)
{
	return Complex{ (c1._realPart - c2._realPart), (c1._complexPart - c2._complexPart) };
}
Complex operator * (const Complex& c1, const Complex& c2)
{
	return Complex{ ((c1._realPart * c2._realPart) - (c1._complexPart * c2._complexPart)), ((c1._realPart * c2._complexPart) +
	(c1._complexPart * c2._realPart)) };
}
bool operator == (const Complex& c1, const Complex& c2)
{
	return ((c1._realPart == c2._realPart) && (c1._complexPart == c2._complexPart));
}

static const unsigned HEIGHT = 2160;
static const unsigned WIDTH = 5040;
static const double RATIO = WIDTH / HEIGHT;

static const unsigned MAX_ITERATIONS = 10000;

static const bool DOWNSAMPLE = true;
static const bool JULIA = false;

static long double PLANE_HEIGHT = 4.0;
static long double PLANEDIV_HEIGHT = PLANE_HEIGHT / 2.0;

static long double PLANE_WIDTH = 4.0 * RATIO;
static long double PLANEDIV_WIDTH = PLANE_WIDTH / 2.0;

static long double ITERATE_HEIGHT = PLANE_HEIGHT / (long double)HEIGHT;
static long double ITERATE_WIDTH = PLANE_WIDTH / (long double)WIDTH;

static HWND _consoleHandle = GetConsoleWindow();
static HDC _deviceContext = GetDC(_consoleHandle);

static vector<vector<Complex>> SCREENSPACE;
static Complex COPY;

static const Complex JULIAPOINT = Complex{ -0.8 , -0.156 };

static vector<Color> COLORS = {};

static vector<Color>::iterator COLORITR = COLORS.begin();

static long double l1, l2, l3, l4;
static long double l;

static Complex k;

static unsigned i, j, x, y;

static unsigned r, g, b;

static unsigned t;

void colorInit()
{
	unsigned r = 0, g = 0, b = 0;
	for (unsigned i = 0; i < MAX_ITERATIONS; i++)
	{
		r = 0, g = 0, b = 0;
		if ((i / 255) % 2)
		{
			r = i % 256;
			b = r;
		}
		else
		{
			//for (unsigned j = 0; j < i; j++)
			//{
			//	r += 9;
			//	if (r > 255)
			//	{
			//		r = 0;
			//		b += 11;
			//		if (b > 255)
			//		{
			//			b = 0;
			//		}
			//	}
			//}

			r = i % 256;
			b = r;
		}

		COLORS.push_back(Color{ r, g, b });
	}
}

void initialize()
{
	long double y = -PLANEDIV_HEIGHT;
	for (unsigned i = 0; i < HEIGHT; i++)
	{
		SCREENSPACE.push_back(vector<Complex>{});
		long double x = -PLANEDIV_WIDTH;
		for (unsigned j = 0; j < WIDTH; j++)
		{
			SCREENSPACE[i].push_back(Complex{ x ,  y });
			x += ITERATE_WIDTH;
		}
		y += ITERATE_HEIGHT;
	}
}

void iterate(unsigned iterations)
{
	if (iterations > MAX_ITERATIONS)
	{
		cerr << "Too Many Iterations: " << iterations << " > " << MAX_ITERATIONS << endl;
		exit(1);
	}

	switch (DOWNSAMPLE)
	{
	case false:
		switch (JULIA)
		{
		case false:
			for (y = 0; y < HEIGHT; y++)
			{
				x = 0;
				for (Complex j : SCREENSPACE[y])
				{
					x++;
					l = 0;

					k = j;
					while (l++ < iterations)
					{
						j = (j * j) + k;
						if (((j._realPart * j._realPart) + (j._complexPart * j._complexPart)) >= 4)
						{
							COLORITR = COLORS.begin() + l;
							SetPixel(_deviceContext, x, y, RGB(COLORITR->getR(), COLORITR->getG(), COLORITR->getB()));
							break;
						}
					}
				}
			}
			break;
		case true:
			for (y = 0; y < HEIGHT; y++)
			{
				x = 0;
				for (Complex j : SCREENSPACE[y])
				{
					x++;
					l = 0;
					while (l++ < iterations)
					{
						j = (j * j) + JULIAPOINT;
						if (((j._realPart * j._realPart) + (j._complexPart * j._complexPart)) >= 4)
						{
							COLORITR = COLORS.begin() + l;
							SetPixel(_deviceContext, x, y, RGB(COLORITR->getR(), COLORITR->getG(), COLORITR->getB()));
							break;
						}
					}
				}
			}
			break;
		}
	case true:
		switch (JULIA)
		{
		case false:
			for (i = 0, y = 0; i < HEIGHT; i += 2, y++)
			{
				for (j = 0, x = 0; j < WIDTH; j += 2, x++)
				{
					k = SCREENSPACE[i][j];

					l = 0;
					COPY = k;
					while (l < iterations)
					{
						k = (k * k) + COPY;
						l++;
						if (((k._realPart * k._realPart) + (k._complexPart * k._complexPart)) >= 4)
							break;
					}
					l1 = l;

					k = SCREENSPACE[i][++j];

					l = 0;
					COPY = k;
					while (l < iterations)
					{
						k = (k * k) + COPY;
						l++;
						if (((k._realPart * k._realPart) + (k._complexPart * k._complexPart)) >= 4)
							break;
					}
					l2 = l;

					k = SCREENSPACE[++i][j];

					l = 0;
					COPY = k;
					while (l < iterations)
					{
						k = (k * k) + COPY;
						l++;
						if (((k._realPart * k._realPart) + (k._complexPart * k._complexPart)) >= 4)
							break;
					}
					l3 = l;

					k = SCREENSPACE[i][--j];

					l = 0;
					COPY = k;
					while (l < iterations)
					{
						k = (k * k) + COPY;
						l++;
						if (((k._realPart * k._realPart) + (k._complexPart * k._complexPart)) >= 4)
							break;
					}
					l4 = l;

					i--;

					r = 0, g = 0, b = 0;

					t = 0;

					if (l1 != iterations)
					{
						COLORITR = (COLORS.begin() + l1);
						r += COLORITR->getR();
						g += COLORITR->getG();
						b += COLORITR->getB();
						t++;
					}
					if (l2 != iterations)
					{
						COLORITR = (COLORS.begin() + l2);
						r += COLORITR->getR();
						g += COLORITR->getG();
						b += COLORITR->getB();
						t++;
					}
					if (l3 != iterations)
					{
						COLORITR = (COLORS.begin() + l3);
						r += COLORITR->getR();
						g += COLORITR->getG();
						b += COLORITR->getB();
						t++;
					}
					if (l4 != iterations)
					{
						COLORITR = (COLORS.begin() + l4);
						r += COLORITR->getR();
						g += COLORITR->getG();
						b += COLORITR->getB();
						t++;
					}

					if (t)
					{
						r /= t;
						g /= t;
						b /= t;

						SetPixel(_deviceContext, x, y, RGB(r, g, b));
					}
				}
			}
			break;
		case true:
			for (i = 0, y = 0; i < HEIGHT; i += 2, y++)
			{
				for (j = 0, x = 0; j < WIDTH; j += 2, x++)
				{
					k = SCREENSPACE[i][j];

					l = 0;
					while (l < iterations)
					{
						k = (k * k) + JULIAPOINT;
						l++;
						if (((k._realPart * k._realPart) + (k._complexPart * k._complexPart)) >= 4)
							break;
					}
					l1 = l;

					k = SCREENSPACE[i][++j];

					l = 0;
					while (l < iterations)
					{
						k = (k * k) + JULIAPOINT;
						l++;
						if (((k._realPart * k._realPart) + (k._complexPart * k._complexPart)) >= 4)
							break;
					}
					l2 = l;

					k = SCREENSPACE[++i][j];

					l = 0;
					while (l < iterations)
					{
						k = (k * k) + JULIAPOINT;
						l++;
						if (((k._realPart * k._realPart) + (k._complexPart * k._complexPart)) >= 4)
							break;
					}
					l3 = l;

					k = SCREENSPACE[i][--j];

					l = 0;
					while (l < iterations)
					{
						k = (k * k) + JULIAPOINT;
						l++;
						if (((k._realPart * k._realPart) + (k._complexPart * k._complexPart)) >= 4)
							break;
					}
					l4 = l;

					i--;

					r = 0, g = 0, b = 0;

					t = 0;

					if (l1 != iterations)
					{
						COLORITR = (COLORS.begin() + l1);
						r += COLORITR->getR();
						g += COLORITR->getG();
						b += COLORITR->getB();
						t++;
					}
					if (l2 != iterations)
					{
						COLORITR = (COLORS.begin() + l2);
						r += COLORITR->getR();
						g += COLORITR->getG();
						b += COLORITR->getB();
						t++;
					}
					if (l3 != iterations)
					{
						COLORITR = (COLORS.begin() + l3);
						r += COLORITR->getR();
						g += COLORITR->getG();
						b += COLORITR->getB();
						t++;
					}
					if (l4 != iterations)
					{
						COLORITR = (COLORS.begin() + l4);
						r += COLORITR->getR();
						g += COLORITR->getG();
						b += COLORITR->getB();
						t++;
					}

					if (t)
					{
						r /= t;
						g /= t;
						b /= t;

						SetPixel(_deviceContext, x, y, RGB(r, g, b));
					}
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
	for (vector<Complex>& i : SCREENSPACE)
	{
		long double x = -PLANEDIV_WIDTH;
		for (Complex& j : i)
		{
			j = Complex{ x + plusX , y + plusY };
			x += ITERATE_WIDTH;
		}
		y += ITERATE_HEIGHT;
	}

	Sleep(3000);

	system("CLS");
}

int main()
{
	colorInit();

	initialize();

	zoom(1.0, 0.0, 0.0);

	iterate(2000);

	zoom(40.0, 0.35, 0.15);

	iterate(2000);

	zoom(25.0, -1.2, 0.23);

	iterate(2000);

	zoom(50.0, -1.2, 0.23);

	iterate(2000);

	zoom(250.0, -1.18, 0.3);

	iterate(2000);

	zoom(1600.0, -1.185, 0.3);

	iterate(2000);

	zoom(10000.0, -1.185, 0.305);

	iterate(2000);

	zoom(25000.0, -1.1855, 0.3054);

	iterate(2000);

	zoom(200000.0, -1.1855, 0.3054);

	iterate(2000);

	zoom(2000.0, -1.18552, 0.30539);

	iterate(2000);

	zoom(20000.0, -1.18552, 0.30539);

	iterate(2000);

	zoom(200000.0, -1.18552, 0.30539);

	iterate(2000);

	zoom(2000000.0, -1.18552, 0.30539);

	iterate(2000);

	zoom(200.0, -1.25066, -0.02012);

	iterate(2000);

	zoom(2000.0, -1.25066, -0.02012);

	iterate(2000);

	zoom(20000.0, -1.25066, 0.02012);

	iterate(2000);

	zoom(200000.0, -1.25066, 0.02012);

	iterate(2000);

	ReleaseDC(_consoleHandle, _deviceContext);

	return 0;
}
