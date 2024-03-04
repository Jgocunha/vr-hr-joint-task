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
	// After defining the simulation, we can create the application.
	//const auto simulation = std::make_shared<dnf_composer::Simulation>("sim", 5, 0, 0);
	// You can run the application without the user interface by setting the second parameter to false.
	//constexpr bool activateUserInterface = true;
	//const dnf_composer::Application app{ simulation, activateUserInterface };

	// After creating the application, we can add the windows we want to display.
	//app.activateUserInterfaceWindow(dnf_composer::user_interface::LOG_WINDOW);

	constexpr int waitTime = 500;

	CoppeliasimHandler coppeliasimHandler;
	DNFComposerHandler dnfcomposerHandler{{"hr-vr-experiment", 10}};

	coppeliasimHandler.init();
	dnfcomposerHandler.init();

	while(!coppeliasimHandler.isConnected())
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for connection with CoppeliaSim...\n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		Sleep(waitTime);
	}
	coppeliasimHandler.setSignal(SignalSignatures::START_SIM, 1);

	bool hasSimStarted = coppeliasimHandler.getSignals().simStarted;
	while(!hasSimStarted)
	{
		coppeliasimHandler.setSignal(SignalSignatures::START_SIM, 1);
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for CoppeliaSim to start...\n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		hasSimStarted = coppeliasimHandler.getSignals().simStarted;
		Sleep(waitTime);
	}
	coppeliasimHandler.setSignal(SignalSignatures::START_SIM, 0);

	bool haveObjectBeenCreated = coppeliasimHandler.getSignals().objectsCreated;
	while (!haveObjectBeenCreated)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be created...\n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		haveObjectBeenCreated = coppeliasimHandler.getSignals().objectsCreated;
		Sleep(waitTime);
	}

	std::tuple<int, int, int> objectPositions = { coppeliasimHandler.getSignals().object1, coppeliasimHandler.getSignals().object2, coppeliasimHandler.getSignals().object3 };
	while (std::get<0>(objectPositions) == 0 || std::get<1>(objectPositions) == 0 || std::get<2>(objectPositions) == 0)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be positioned...\n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		objectPositions = { coppeliasimHandler.getSignals().object1, coppeliasimHandler.getSignals().object2, coppeliasimHandler.getSignals().object3 };
		Sleep(waitTime);
	}

	for(int i = 1; i <= 3; i++)
	{
		objectPositions = { coppeliasimHandler.getSignals().object1, coppeliasimHandler.getSignals().object2, coppeliasimHandler.getSignals().object3 };
		// randomly select an object from the available objects to be the target
		const int targetObject = i;

		bool hasObjectBeenGrasped = coppeliasimHandler.getSignals().objectGrasped;
		while (!hasObjectBeenGrasped)
		{
			coppeliasimHandler.setSignal(SignalSignatures::TARGET_OBJECT, targetObject);
			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be grasped...\n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
			hasObjectBeenGrasped = coppeliasimHandler.getSignals().objectGrasped;
			Sleep(waitTime);
		}
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_GRASPED, 0);

		bool hasObjectBeenPlaced = coppeliasimHandler.getSignals().objectPlaced;
		while (!hasObjectBeenPlaced)
		{
			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be placed...\n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
			hasObjectBeenPlaced = coppeliasimHandler.getSignals().objectPlaced;
			Sleep(waitTime);
		}
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_PLACED, 0);

		log(dnf_composer::tools::logger::LogLevel::INFO, "Object " + std::to_string(i) + " has been placed.\n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		// Make sure signals are reset
		Sleep(waitTime);
	}


	return 0;
	



	/*try
	{
		app.init();

		bool userRequestClose = false;
		while (!userRequestClose)
		{
			app.step();
			userRequestClose = app.getCloseUI();
		}
		app.close();
	 	return 0;
	 }
	 catch (const dnf_composer::Exception &ex)
	 {
	 	const std::string errorMessage = "Exception: " + std::string(ex.what()) + " ErrorCode: " + std::to_string(static_cast<int>(ex.getErrorCode())) + ". \n";
	 	log(dnf_composer::tools::logger::LogLevel::FATAL, errorMessage, dnf_composer::tools::logger::LogOutputMode::CONSOLE);
	 	return static_cast<int>(ex.getErrorCode());
	 }
	 catch (const std::exception &ex)
	 {
	 	log(dnf_composer::tools::logger::LogLevel::FATAL, "Exception caught: " + std::string(ex.what()) + ". \n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
	 	return 1;
	 }
	 catch (...)
	 {
	 	log(dnf_composer::tools::logger::LogLevel::FATAL, "Unknown exception occurred. \n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
	 	return 1;
	 }*/
}
