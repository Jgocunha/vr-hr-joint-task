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
	signalsThread.join();
	handPositionThread.join();
	coppeliasimHandler.close();
	dnfcomposerHandler.close();
}

void Experiment::main()
{
	waitForConnection();
	waitForSimulationStart();
	waitForObjectsToBeCreated();
	signalsThread = std::thread(&Experiment::updateSignals, this);
	handPositionThread = std::thread(&Experiment::updateHandPosition, this);
	//pickAndPlaceObjects();
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
	//coppeliasimHandler.setSignal(SignalSignatures::START_SIM, 0);
}

void Experiment::waitForObjectsToBeCreated() const
{
	/*bool haveObjectBeenCreated = coppeliasimHandler.getSignals().objectsCreated;
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
	}*/

	log(dnf_composer::tools::logger::LogLevel::INFO, "Pick and place will now start...\n");
}

void Experiment::pickAndPlaceObjects()
{

	do {

		//do {
		//	//log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be grasped...\n");
		//	Sleep(commsFrequency);
		//} while (!coppeliasimHandler.getSignals().objectGrasped);
		//coppeliasimHandler.setSignal(SignalSignatures::OBJECT_GRASPED, 0);

		//Sleep(commsFrequency);

		//do {
		//	//log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be placed...\n");
		//	Sleep(commsFrequency);
		//} while (!coppeliasimHandler.getSignals().objectPlaced);
		//coppeliasimHandler.setSignal(SignalSignatures::OBJECT_PLACED, 0);

		////log(dnf_composer::tools::logger::LogLevel::INFO, "An object has been placed by the robot.\n");
		////Sleep(commsFrequency);
		Sleep(commsFrequency);

	} while (areObjectsPresent());

	Sleep(commsFrequency);
	taskFinished = true;
}


void Experiment::updateHandPosition()
{
	do
	{
		bool validValues = true;
		double hand_proximity = coppeliasimHandler.getSignals().hand_proximity;
		double hand_y = coppeliasimHandler.getSignals().hand_y;
		double filtered_hand_proximity = hand_proximity;
		double filtered_hand_y = hand_y;

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
			dnfcomposerHandler.setHandStimulus(filtered_hand_y, filtered_hand_proximity);

		//Sleep(1);
	} while (!taskFinished);
}

void Experiment::updateAvailableObjects()
{
	const int sampleSize = 5; // Number of reads to determine the majority

	// Object 1
	if (coppeliasimHandler.hasSignalMajorityValue(SignalSignatures::OBJECT1_EXISTS, 0, sampleSize))
	{
		dnfcomposerHandler.removeTargetObject(1);
	}
	else if (coppeliasimHandler.hasSignalMajorityValue(SignalSignatures::OBJECT1_EXISTS, 1, sampleSize))
	{
		dnfcomposerHandler.addTargetObject(1);
	}

	// Object 2
	if (coppeliasimHandler.hasSignalMajorityValue(SignalSignatures::OBJECT2_EXISTS, 0, sampleSize))
	{
		dnfcomposerHandler.removeTargetObject(2);
	}
	else if (coppeliasimHandler.hasSignalMajorityValue(SignalSignatures::OBJECT2_EXISTS, 1, sampleSize))
	{
		dnfcomposerHandler.addTargetObject(2);
	}

	// Object 3
	if (coppeliasimHandler.hasSignalMajorityValue(SignalSignatures::OBJECT3_EXISTS, 0, sampleSize))
	{
		dnfcomposerHandler.removeTargetObject(3);
	}
	else if (coppeliasimHandler.hasSignalMajorityValue(SignalSignatures::OBJECT3_EXISTS, 1, sampleSize))
	{
		dnfcomposerHandler.addTargetObject(3);
	}
}

void Experiment::updateTargetObject()
{
	coppeliasimHandler.setSignal(SignalSignatures::TARGET_OBJECT, dnfcomposerHandler.getTargetObject());
}

void Experiment::updateSignals()
{
	do
	{
		updateAvailableObjects();
		updateTargetObject();
		//Sleep(10);
	} while (!taskFinished);
}

bool Experiment::areObjectsPresent() const
{
	auto signals = coppeliasimHandler.getSignals();
	bool isPresent = signals.object1 != 0 || signals.object2 != 0 || signals.object3 != 0;
	return isPresent;
}

