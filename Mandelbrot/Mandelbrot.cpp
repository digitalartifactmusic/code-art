#include <iostream>
#include <Windows.h>
#include <time.h>
#include <vector>

using namespace std;

const unsigned SCREENSIDE = 1000;

long double PLANE = 4.0;
long double PLANEDIV = PLANE / 2.0;
long double ITERATE = PLANE / (long double)SCREENSIDE;

HWND _consoleHandle = GetConsoleWindow();
HDC _deviceContext = GetDC(_consoleHandle);

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

const Complex i(0, 1);                                                  // Defined constant i which is equivalent to sqrt(-1)

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

static vector<vector<Complex>> SCREENSPACE;
static Complex COPY;

void initialize()
{
	PLANE = 4.0;
	PLANEDIV = PLANE / 2;

	ITERATE = PLANE / (long double)SCREENSIDE;

	long double y = -PLANEDIV;
	for (unsigned i = 0; i < SCREENSIDE; i++)
	{
		SCREENSPACE.push_back(vector<Complex>{});
		long double x = -PLANEDIV;
		for (unsigned j = 0; j < SCREENSIDE; j++)
		{
			SCREENSPACE[i].push_back(Complex{ x ,  y });
			x += ITERATE;
		}
		y += ITERATE;
	}
}

void iterate(unsigned iterations)
{
	for (unsigned i = 0; i < SCREENSPACE.size(); i++)
	{
		unsigned l = 0;
		for (Complex j : SCREENSPACE[i])
		{
			l++;
			unsigned k = 0;
			COPY = j;
			while (k < iterations)
			{
				j = (j * j) + COPY;
				k++;
				if (((j._realPart * j._realPart) + (j._complexPart * j._complexPart)) >= 4)
				{
					SetPixel(_deviceContext, l, i, RGB(255 - (k * 5), 0, 0 + (k * 5)));
					break;
				}
			}
		}
	}
}

void zoom(long double magnification, long double plusX, long double plusY)
{
	PLANEDIV = 4.0 / magnification;
	PLANE = PLANEDIV * 2;

	ITERATE = PLANE / (long double)SCREENSIDE;

	long double y = -PLANEDIV;
	for (vector<Complex>& i : SCREENSPACE)
	{
		long double x = -PLANEDIV;
		for (Complex& j : i)
		{
			j = Complex{ x + plusX , y + plusY };
			x += ITERATE;
		}
		y += ITERATE;
	}

	system("CLS");
}

int main()
{
	initialize();

	zoom(6, -1.2, 0.0);

	iterate(2000);

	zoom(40.0, 0.35, 0.15);

	iterate(2000);

	zoom(25.0, -1.2, 0.23);

	iterate(2000);

	ReleaseDC(_consoleHandle, _deviceContext);

	return 0;
}
