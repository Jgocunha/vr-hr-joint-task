// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


// This .cpp file is an example of how you can use the library to create your own DNF simulation.

#include "experiment.h"

int main(int argc, char* argv[])
{
	constexpr int commsFrequency = 500;
	constexpr double deltaT = 10;
	const std::string experimentName = "experiment";
	constexpr RobotArchitecture architecture = RobotArchitecture::HAND_MOTION;

	Experiment experiment(experimentName, architecture, commsFrequency, deltaT);
	experiment.init();
	experiment.run();
	experiment.close();

	return 0;
}
