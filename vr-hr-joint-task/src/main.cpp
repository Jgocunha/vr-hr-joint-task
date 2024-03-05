// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


// This .cpp file is an example of how you can use the library to create your own DNF simulation.

#include "experiment.h"

int main(int argc, char* argv[])
{
	constexpr int commsFrequency = 500;

	Experiment experiment("experiment", commsFrequency, 10);
	experiment.init();

	experiment.run();

	experiment.close();

	return 0;
}
