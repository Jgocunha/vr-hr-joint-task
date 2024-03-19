#include "experiment.h"


Experiment::Experiment(std::string name, int commsFreq, double deltaT)
		: coppeliasimHandler(), dnfcomposerHandler({ std::move(name), deltaT }), commsFrequency(commsFreq)
{ }

Experiment::~Experiment()
{
	close();
}

void Experiment::init()
{
	coppeliasimHandler.init();
	dnfcomposerHandler.init();
}

void Experiment::run()
{
	main();
}

void Experiment::close()
{
	coppeliasimHandler.close();
	dnfcomposerHandler.close();
}

void Experiment::main()
{
	waitForConnection();
	waitForSimulationStart();
	waitForObjectsToBeCreated();
	pickAndPlaceObjects();
}

void Experiment::waitForConnection() const
{
	while (!coppeliasimHandler.isConnected())
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for connection with CoppeliaSim...\n");
		Sleep(commsFrequency);
	}
}

void Experiment::waitForSimulationStart()
{
	coppeliasimHandler.setSignal(SignalSignatures::START_SIM, 1);
	bool hasSimStarted = coppeliasimHandler.getSignals().simStarted;
	while (!hasSimStarted)
	{
		coppeliasimHandler.setSignal(SignalSignatures::START_SIM, 1);
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for CoppeliaSim to start...\n");
		hasSimStarted = coppeliasimHandler.getSignals().simStarted;
		Sleep(commsFrequency);
	}
	coppeliasimHandler.setSignal(SignalSignatures::START_SIM, 0);
}

void Experiment::waitForObjectsToBeCreated() const
{
	bool haveObjectBeenCreated = coppeliasimHandler.getSignals().objectsCreated;
	while (!haveObjectBeenCreated)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be created...\n");
		haveObjectBeenCreated = coppeliasimHandler.getSignals().objectsCreated;
		Sleep(commsFrequency);
	}

	std::tuple<int, int, int> objectPositions = { coppeliasimHandler.getSignals().object1, coppeliasimHandler.getSignals().object2, coppeliasimHandler.getSignals().object3 };
	while (std::get<0>(objectPositions) == 0 || std::get<1>(objectPositions) == 0 || std::get<2>(objectPositions) == 0)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be positioned...\n");
		objectPositions = { coppeliasimHandler.getSignals().object1, coppeliasimHandler.getSignals().object2, coppeliasimHandler.getSignals().object3 };
		Sleep(commsFrequency);
	}
}

void Experiment::pickAndPlaceObjects()
{

	for (int i = 1; i <= 3; i++)
	{
		bool hasObjectBeenGrasped = coppeliasimHandler.getSignals().objectGrasped;
		int targetObject = dnfcomposerHandler.getTargetObject();
		while (!hasObjectBeenGrasped)
		{
			targetObject = dnfcomposerHandler.getTargetObject();
			coppeliasimHandler.setSignal(SignalSignatures::TARGET_OBJECT, targetObject);

			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be grasped...\n");
			hasObjectBeenGrasped = coppeliasimHandler.getSignals().objectGrasped;
			Sleep(commsFrequency);
		}
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_GRASPED, 0);

		Sleep(commsFrequency);

		// Update the remaining objects here
		if (targetObject == 1)
			coppeliasimHandler.setSignal(SignalSignatures::OBJECT1_EXISTS, 0);
		else if (targetObject == 2)
			coppeliasimHandler.setSignal(SignalSignatures::OBJECT2_EXISTS, 0);
		else if (targetObject == 3)
			coppeliasimHandler.setSignal(SignalSignatures::OBJECT3_EXISTS, 0);
		dnfcomposerHandler.removeTargetObject(targetObject);


		// Also update the target object here
		targetObject = dnfcomposerHandler.getTargetObject();
		coppeliasimHandler.setSignal(SignalSignatures::TARGET_OBJECT, targetObject);


		bool hasObjectBeenPlaced = coppeliasimHandler.getSignals().objectPlaced;
		while (!hasObjectBeenPlaced)
		{
			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be placed...\n");
			hasObjectBeenPlaced = coppeliasimHandler.getSignals().objectPlaced;
			Sleep(commsFrequency);
		}
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_PLACED, 0);

		log(dnf_composer::tools::logger::LogLevel::INFO, "Object " + std::to_string(i) + " has been placed.\n");
		// Make sure signals are reset
		Sleep(commsFrequency);
	}
}
