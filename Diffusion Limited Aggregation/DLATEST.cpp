// Noah Grassmeyer
// Diffusion Limited Aggregation

#include "DLA.h"
#include <iostream>

int main()
{
	bool go;

	do {
		DLA d1{ NUM_MOVERS, NUM_SEEDS };
		for (int i = 0; i < ITERATIONS; i++)
			d1.iterateClassicFill();
		d1.animateClassic();
		cout << "Enter 1 to go again. Enter anything else to exit: ";
		cin >> go;
		break;
	} while (go);

	return 0;
}
