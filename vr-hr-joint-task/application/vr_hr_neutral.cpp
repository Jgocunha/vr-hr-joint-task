// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "experiment.h"

int main(int argc, char* argv[])
{

	try
	{
		// 1) Ask user for participant info
		const int  participantId = askParticipantId();
		const char condition     = askCondition();
		const char feature       = askFeature();

		// 2) Experiment configuration
		constexpr double deltaT = 50;
		constexpr DnfArchitectureType architecture =
			DnfArchitectureType::NEUTRAL;
		constexpr uint8_t numTrials = 5;

		// 3) Fill parameters, including participant info
		ExperimentParameters params{architecture, deltaT, numTrials };
		params.participantId = participantId;
		params.condition     = condition;
		params.feature       = feature;

		// 4) Run experiment
		Experiment experiment(params);

		experiment.init();
		experiment.run();
		experiment.end();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception" << std::endl;
	}

	return 0;
}
