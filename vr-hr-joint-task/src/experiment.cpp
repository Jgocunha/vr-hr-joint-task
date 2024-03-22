#include "experiment.h"


Experiment::Experiment(std::string name, int commsFreq, double deltaT)
		: coppeliasimHandler(), dnfcomposerHandler({ std::move(name), deltaT }), commsFrequency(commsFreq)
{
	taskFinished = false;
}

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
	signalsThread.join();
}

void Experiment::main()
{
	waitForConnection();
	Sleep(commsFrequency);
	signalsThread = std::thread(&Experiment::updateSignals, this);
	waitForSimulationStart();
	//waitForObjectsToBeCreated();
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
	dnfcomposerSignals.startSim = true;
	bool hasSimStarted = coppeliasimSignals.simStarted;
	while (!hasSimStarted)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for CoppeliaSim to start...\n");
		hasSimStarted = coppeliasimSignals.simStarted;
		Sleep(commsFrequency);
	}
}

//void Experiment::waitForObjectsToBeCreated() const
//{
//	bool haveObjectBeenCreated = signals.objectsCreated;
//	while (!haveObjectBeenCreated)
//	{
//		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be created...\n");
//		haveObjectBeenCreated = signals.objectsCreated;
//		Sleep(commsFrequency);
//	}
//
//	std::tuple<int, int, int> objectPositions = { signals.object1, signals.object2, signals.object3 };
//	while (std::get<0>(objectPositions) == 0 || std::get<1>(objectPositions) == 0 || std::get<2>(objectPositions) == 0)
//	{
//		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be positioned...\n");
//		objectPositions = { signals.object1, signals.object2, signals.object3 };
//		Sleep(commsFrequency);
//	}
//
//	log(dnf_composer::tools::logger::LogLevel::INFO, "Pick and place will now start...\n");
//}

void Experiment::updateHandPosition()
{
	//do
	//{
		bool validValues = true;
		double filtered_hand_proximity = coppeliasimSignals.hand_proximity;
		double filtered_hand_y = coppeliasimSignals.hand_y;

		// Filter out bad readings
		if (filtered_hand_y < 0.1 || filtered_hand_y > 90)
			validValues = false;
		if (filtered_hand_proximity < 0.1 || filtered_hand_proximity > 20)
			validValues = false;

		// Filter out of range values
		if (filtered_hand_y == -1)
		{
			filtered_hand_proximity = 0;
			filtered_hand_y = 0;
			validValues = true;
		}

		if (validValues)
		{
			dnfcomposerHandler.setHandStimulus(filtered_hand_y, filtered_hand_proximity);
			//const std::string msg = "Filtered hand_y: " + std::to_string(filtered_hand_y) + " Filtered hand_proximity: " + std::to_string(filtered_hand_proximity) + '\n';
			//log(dnf_composer::tools::logger::LogLevel::INFO, msg.c_str(), dnf_composer::tools::logger::LogOutputMode::GUI);
		}

		//Sleep(1);
	//} while (!taskFinished);
}

void Experiment::updateAvailableObjects()
{
	const int sampleSize = 5; // Number of reads to determine the majority

	// Object 1
	if (!coppeliasimSignals.object1)
		dnfcomposerHandler.removeTargetObject(1);
	else
		dnfcomposerHandler.addTargetObject(1);

	// Object 2
	if (!coppeliasimSignals.object2)
		dnfcomposerHandler.removeTargetObject(2);
	else
		dnfcomposerHandler.addTargetObject(2);

	// Object 3
	if (!coppeliasimSignals.object3)
		dnfcomposerHandler.removeTargetObject(3);
	else 
		dnfcomposerHandler.addTargetObject(3);
}

void Experiment::updateTargetObject()
{
	//coppeliasimHandler.setSignal(SignalSignatures::TARGET_OBJECT, dnfcomposerHandler.getTargetObject());
	dnfcomposerSignals.targetObject = dnfcomposerHandler.getTargetObject();
	//coppeliasimHandler.setSignals(dnfcomposerSignals);
}

void Experiment::updateSignals()
{
	do
	{
		coppeliasimSignals = coppeliasimHandler.getSignals();
		updateHandPosition();
		updateAvailableObjects();
		updateTargetObject();
		coppeliasimHandler.setSignals(dnfcomposerSignals);
		Sleep(10);
	} while (true);
}

bool Experiment::areObjectsPresent() const
{
	bool isPresent = coppeliasimSignals.object1 != 0 || coppeliasimSignals.object2 != 0 || coppeliasimSignals.object3 != 0;
	return isPresent;
}

void Experiment::updateObjectsInWorkspace()
{
	do
	{
		updateAvailableObjects();
		updateTargetObject();
		Sleep(1);
	} while (!taskFinished);
}