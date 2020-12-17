#include <iostream>
#include <Windows.h>
#include <time.h>
#include <vector>

#include <cmath>

using namespace std;

const unsigned SCREENSIDE = 500;

double PLANE = 2.0;
double PLANEDIV = PLANE / 2;
double HEIGHT_ITERATE = PLANE / (double)SCREENSIDE, WIDTH_ITERATE = PLANE / (double)SCREENSIDE;

HWND _consoleHandle = GetConsoleWindow();

HDC _deviceContext = GetDC(_consoleHandle);

class Complex
{

public:

    double _realPart;
    double _complexPart;

public:

    Complex();
    Complex(double realPart);
    Complex(double realPart, double complexPart);

	double radius()
	{
		return sqrt(pow(_realPart, 2.0) + pow(_complexPart, 2.0));
	}

    friend ostream& operator << (ostream& out, const Complex& c);       // Operator overloading.
    friend istream& operator >> (istream& in, Complex& c);

    friend Complex operator + (const Complex& c1, const Complex& c2);
    friend Complex operator - (const Complex& c1, const Complex& c2);
    friend Complex operator * (const Complex& c1, const Complex& c2);

    friend bool operator == (const Complex& c1, const Complex& c2);

};

const Complex i(0, 1);                                                  // Defined constant i which is equivalent to sqrt(-1)

Complex::Complex() : _realPart{ 0 }, _complexPart{ 0 } {}                    // Default constructor.

Complex::Complex(double realPart) : _realPart{ realPart }, _complexPart{ 0 } {}  // Parameterized Constructors

Complex::Complex(double realPart, double complexPart) : _realPart{ realPart }, _complexPart{ complexPart } {}

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

void initialize()
{
	unsigned i = 0;
	for (double y = -PLANEDIV; y <= PLANEDIV; y += HEIGHT_ITERATE)
	{
		SCREENSPACE.push_back(vector<Complex>{});
		for (double x = -PLANEDIV; x <= PLANEDIV; x += WIDTH_ITERATE)
		{
			SCREENSPACE[i].push_back(Complex{ x ,  y });
		}
		i++;
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
			Complex temp = j;
			while ((((j._realPart * j._realPart) + (j._complexPart * j._complexPart)) <= 4.0) && (k < iterations))
			{
				j = (j * j) + temp;
				k++;
			}
			if (j.radius() > 2)
			{
				SetPixel(_deviceContext, l, i, RGB(255, 0, 0));
			}
		}
	}
}

void zoom(double magnification, double plusX = 0.0, double plusY = 0.0)
{
	PLANEDIV = PLANEDIV / magnification;
	PLANE = PLANEDIV * 2;

	HEIGHT_ITERATE = PLANE / (double)SCREENSIDE;
	WIDTH_ITERATE = PLANE / (double)SCREENSIDE;

	double y = PLANEDIV * -1.0;
	for (vector<Complex>& i : SCREENSPACE)
	{
		double x = PLANEDIV * -1.0;
		for (Complex& j : i)
		{
			j = Complex{ x + plusX , y + plusY };
			x += WIDTH_ITERATE;
		}
		y += HEIGHT_ITERATE;
	}

	system("CLS");
}

int main()
{
	initialize();

	iterate(200);

	zoom(100.0, 0.0, 0.64);

	iterate(10000);

	ReleaseDC(_consoleHandle, _deviceContext);
}
