#include "experiment.h"


Experiment::Experiment(std::string name, int commsFreq, double deltaT)
		: coppeliasimHandler(), dnfcomposerHandler({ std::move(name), deltaT }), commsFrequency(commsFreq)
{}

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
	signalsThread = std::thread(&Experiment::updateSignals, this);
	main();
}

void Experiment::close()
{
	signalsThread.join();
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
	/*bool haveObjectBeenCreated = coppeliasimHandler.getSignals().objectsCreated;
	while (!haveObjectBeenCreated)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be created...\n");
		haveObjectBeenCreated = coppeliasimHandler.getSignals().objectsCreated;
		Sleep(commsFrequency);
	}*/

	std::tuple<int, int, int> objectPositions = { coppeliasimHandler.getSignals().object1, coppeliasimHandler.getSignals().object2, coppeliasimHandler.getSignals().object3 };
	while (std::get<0>(objectPositions) == 0 || std::get<1>(objectPositions) == 0 || std::get<2>(objectPositions) == 0)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be positioned...\n");
		objectPositions = { coppeliasimHandler.getSignals().object1, coppeliasimHandler.getSignals().object2, coppeliasimHandler.getSignals().object3 };
		Sleep(commsFrequency);
	}

	log(dnf_composer::tools::logger::LogLevel::INFO, "Pick and place will now start...\n");
}

void Experiment::pickAndPlaceObjects()
{

	while(areObjectsPresent())
	{
		bool hasObjectBeenGrasped = false;
		while (!hasObjectBeenGrasped)
		{
			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be grasped...\n");
			hasObjectBeenGrasped = coppeliasimHandler.getSignals().objectGrasped;
			Sleep(commsFrequency);
		}
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_GRASPED, 0);

		Sleep(commsFrequency);

		bool hasObjectBeenPlaced = false;
		while (!hasObjectBeenPlaced)
		{
			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be placed...\n");
			hasObjectBeenPlaced = coppeliasimHandler.getSignals().objectPlaced;
			Sleep(commsFrequency);
		}
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_PLACED, 0);
		//updateAvailableObjects();
		//Sleep(commsFrequency);
		//updateAvailableObjects();
		//Sleep(commsFrequency);
		//updateAvailableObjects();
		//Sleep(commsFrequency);
		//updateAvailableObjects();
		//Sleep(commsFrequency);
		//updateAvailableObjects();

		log(dnf_composer::tools::logger::LogLevel::INFO, "Object has been placed.\n");
		// Make sure signals are reset
		Sleep(commsFrequency);
	}
}


void Experiment::updateHandPosition() const
{
	static coppeliasim_cpp::Position lastHandPosition = coppeliasimHandler.getSimulationData().handPosition;
	// Get the current hand position from CoppeliaSim
	coppeliasim_cpp::Position handPosition = coppeliasimHandler.getSimulationData().handPosition;

	// Define a threshold for position change
	constexpr double positionChangeThreshold = 0.9; // You can adjust this threshold based on your application's requirements

	// Calculate the difference between the current hand position and the previous hand position across all dimensions
	double positionDifference = std::sqrt(std::pow(handPosition.x - lastHandPosition.x, 2) +
		std::pow(handPosition.y - lastHandPosition.y, 2) +
		std::pow(handPosition.z - lastHandPosition.z, 2));

	// Check if the position difference exceeds the threshold
	if (positionDifference > positionChangeThreshold)
	{
		// If the difference is too large, discard the new hand position and use the previous one
		dnfcomposerHandler.setHandStimulus({ lastHandPosition.x, lastHandPosition.y, lastHandPosition.z });
	}
	else
	{
		// If the difference is within the threshold, use the new hand position
		dnfcomposerHandler.setHandStimulus({ handPosition.x, handPosition.y, handPosition.z });

		// Update the last hand position for future comparison
		lastHandPosition = handPosition;
	}
}

//void Experiment::updateHandPosition() const
//{
//	static coppeliasim_cpp::Position lastHandPosition = coppeliasimHandler.getSimulationData().handPosition;
//
//	// Define a threshold for position change
//	constexpr double positionChangeThreshold = 0.9; // You can adjust this threshold based on your application's requirements
//
//	// Get the current hand position from CoppeliaSim
//	coppeliasim_cpp::Position handPosition = coppeliasimHandler.getSimulationData().handPosition;
//
//	// Calculate the difference between the current hand position and the previous hand position across all dimensions
//	double positionDifference = std::sqrt(std::pow(handPosition.x - lastHandPosition.x, 2) +
//		std::pow(handPosition.y - lastHandPosition.y, 2) +
//		std::pow(handPosition.z - lastHandPosition.z, 2));
//
//	// Check if the position difference exceeds the threshold
//	if (positionDifference > positionChangeThreshold)
//	{
//		// If the difference is too large, reset the last hand position to the current hand position
//		lastHandPosition = handPosition;
//	}
//	else
//	{
//		// If the difference is within the threshold, use the new hand position
//		dnfcomposerHandler.setHandStimulus({ handPosition.x, handPosition.y, handPosition.z });
//	}
//}

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


//void Experiment::updateAvailableObjects()
//{
//	// not stable and not clean
//	// but kinda working
//	if (!coppeliasimHandler.getSignals().object1)
//	{
//		dnfcomposerHandler.removeTargetObject(1);
//	}
//	else
//	{
//		dnfcomposerHandler.addTargetObject(1);
//	}
//
//	if (!coppeliasimHandler.getSignals().object2)
//	{
//		dnfcomposerHandler.removeTargetObject(2);
//	}
//	else
//	{
//		dnfcomposerHandler.addTargetObject(2);
//	}
//
//	if (!coppeliasimHandler.getSignals().object3)
//	{
//		dnfcomposerHandler.removeTargetObject(3);
//	}
//	else
//	{
//		dnfcomposerHandler.addTargetObject(3);
//	}
//}

void Experiment::updateTargetObject()
{
	coppeliasimHandler.setSignal(SignalSignatures::TARGET_OBJECT, dnfcomposerHandler.getTargetObject());
}

void Experiment::updateSignals()
{
	while (areObjectsPresent())
	{
		updateHandPosition();
		updateAvailableObjects();
		updateTargetObject();
		Sleep(10);
	}
}

bool Experiment::areObjectsPresent() const
{
	auto signals = coppeliasimHandler.getSignals();
	bool isPresent = signals.object1 != 0 || signals.object2 != 0 || signals.object3 != 0;
	return isPresent;
}

