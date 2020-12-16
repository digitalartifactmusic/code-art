#include "Color.h"

Color::Color(unsigned region, unsigned r, unsigned g, unsigned b) : _region{ region }, _color { r, g, b } { colorInit(); }

Color::Color(unsigned region, unsigned r, unsigned g, unsigned b, unsigned pos, unsigned cap, unsigned base) : _region{ region }, _color{ r , g , b }, _cap{ cap }, _base{ base } { _pos = pos; }

unsigned Color::getR() const { return _color[0]; }
unsigned Color::getG() const { return _color[1]; }
unsigned Color::getB() const { return _color[2]; }

unsigned Color::getPos() const { return _pos; }

unsigned Color::getRegion() const { return _region;  }

void Color::setR(const unsigned r) { _color[0] = r; }
void Color::setG(const unsigned g) { _color[1] = g; }
void Color::setB(const unsigned b) { _color[2] = b; }

void Color::colorInit()
{
	_base = *min_element(_color.begin(), _color.end());

	_pos = max_element(_color.begin(), _color.end()) - _color.begin();
	_cap = _color[_pos];
}

void Color::advanceSecondary()
{
	switch (_pos)
	{
	case 0:

		if (_color[1] != _cap)
			_color[1]++;
		else if (_color[0] != _base)
			_color[0]--;
		else
			_pos++;

		break;
	case 1:

		if (_color[2] != _cap)
			_color[2]++;
		else if (_color[1] != _base)
			_color[1]--;
		else
			_pos++;

		break;
	case 2:

		if (_color[0] != _cap)
			_color[0]++;
		else if (_color[2] != _base)
			_color[2]--;
		else
			_pos = 0;

		break;
	}
}

void Color::advanceSecondary(unsigned amount)
{
	for (unsigned i = 0; i < amount; i++)
	{
		switch (_pos)
		{
		case 0:

			if (_color[1] != _cap)
				_color[1]++;
			else if (_color[0] != _base)
				_color[0]--;
			else
				_pos++;

			break;
		case 1:

			if (_color[2] != _cap)
				_color[2]++;
			else if (_color[1] != _base)
				_color[1]--;
			else
				_pos++;

			break;
		case 2:

			if (_color[0] != _cap)
				_color[0]++;
			else if (_color[2] != _base)
				_color[2]--;
			else
				_pos = 0;

			break;
		}
	}
}
