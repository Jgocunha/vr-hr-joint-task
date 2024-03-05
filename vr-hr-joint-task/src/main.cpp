// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


// This .cpp file is an example of how you can use the library to create your own DNF simulation.

#include <iostream>
#include <string>

#include <simulation/simulation.h>
#include <application/application.h>
#include <exceptions/exception.h>
#include <tools/logger.h>

#include "coppeliasim_handler.h"
#include "dnfcomposer_handler.h"

int main(int argc, char* argv[])
{
	constexpr int waitTime = 500;

	CoppeliasimHandler coppeliasimHandler;
	DNFComposerHandler dnfcomposerHandler{{"hr-vr-experiment", 10}};

	coppeliasimHandler.init();
	dnfcomposerHandler.init();

	while(!coppeliasimHandler.isConnected())
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for connection with CoppeliaSim...\n");
		Sleep(waitTime);
	}
	coppeliasimHandler.setSignal(SignalSignatures::START_SIM, 1);

	bool hasSimStarted = coppeliasimHandler.getSignals().simStarted;
	while(!hasSimStarted)
	{
		coppeliasimHandler.setSignal(SignalSignatures::START_SIM, 1);
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for CoppeliaSim to start...\n");
		hasSimStarted = coppeliasimHandler.getSignals().simStarted;
		Sleep(waitTime);
	}
	coppeliasimHandler.setSignal(SignalSignatures::START_SIM, 0);

	bool haveObjectBeenCreated = coppeliasimHandler.getSignals().objectsCreated;
	while (!haveObjectBeenCreated)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be created...\n");
		haveObjectBeenCreated = coppeliasimHandler.getSignals().objectsCreated;
		Sleep(waitTime);
	}

	std::tuple<int, int, int> objectPositions = { coppeliasimHandler.getSignals().object1, coppeliasimHandler.getSignals().object2, coppeliasimHandler.getSignals().object3 };
	while (std::get<0>(objectPositions) == 0 || std::get<1>(objectPositions) == 0 || std::get<2>(objectPositions) == 0)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be positioned...\n");
		objectPositions = { coppeliasimHandler.getSignals().object1, coppeliasimHandler.getSignals().object2, coppeliasimHandler.getSignals().object3 };
		Sleep(waitTime);
	}

	Sleep(4000);
	coppeliasimHandler.setSignal(SignalSignatures::TARGET_OBJECT, 2);
	Sleep(3000);
	coppeliasimHandler.setSignal(SignalSignatures::TARGET_OBJECT, 3);
	Sleep(2000);

	for(int i = 1; i <= 3; i++)
	{
		objectPositions = { coppeliasimHandler.getSignals().object1, coppeliasimHandler.getSignals().object2, coppeliasimHandler.getSignals().object3 };
		// randomly select an object from the available objects to be the target
		const int targetObject = i;

		bool hasObjectBeenGrasped = coppeliasimHandler.getSignals().objectGrasped;
		while (!hasObjectBeenGrasped)
		{
			coppeliasimHandler.setSignal(SignalSignatures::TARGET_OBJECT, targetObject);
			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be grasped...\n");
			hasObjectBeenGrasped = coppeliasimHandler.getSignals().objectGrasped;
			Sleep(waitTime);
		}
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_GRASPED, 0);

		bool hasObjectBeenPlaced = coppeliasimHandler.getSignals().objectPlaced;
		while (!hasObjectBeenPlaced)
		{
			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be placed...\n");
			hasObjectBeenPlaced = coppeliasimHandler.getSignals().objectPlaced;
			Sleep(waitTime);
		}
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_PLACED, 0);

		log(dnf_composer::tools::logger::LogLevel::INFO, "Object " + std::to_string(i) + " has been placed.\n");
		// Make sure signals are reset
		Sleep(waitTime);
	}

	coppeliasimHandler.close();
	dnfcomposerHandler.close();

	return 0;
}
