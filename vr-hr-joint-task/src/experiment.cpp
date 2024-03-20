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

		do {
			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be grasped...\n");
			Sleep(commsFrequency);
		} while (!coppeliasimHandler.getSignals().objectGrasped);
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_GRASPED, 0);

		Sleep(commsFrequency);

		do {
			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be placed...\n");
			Sleep(commsFrequency);
		} while (!coppeliasimHandler.getSignals().objectPlaced);
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_PLACED, 0);

		log(dnf_composer::tools::logger::LogLevel::INFO, "An object has been placed by the robot.\n");
		Sleep(commsFrequency);

	} while (areObjectsPresent());

	Sleep(commsFrequency);
	taskFinished = true;
}


//void Experiment::updateHandPosition() const
//{
//	const double hand_proximity = coppeliasimHandler.getSignals().hand_proximity;
//	const double hand_y = coppeliasimHandler.getSignals().hand_y;
//
//	// Do some interpretation here.
//	// values of hand proximity can go as high as 40abs
//	// values from hand_y are normalized but can be negative and over 100abs
//	// consider only positive values up until 100abs
//	dnfcomposerHandler.setHandStimulus(hand_y, hand_proximity);
//}


void Experiment::updateHandPosition() const 
{
	static double lastHandProximityEMA = 0;
	static double lastHandYEMA = 0;
	double alpha = 0.1; // Smoothing factor. Adjust as needed for smoother or more responsive filtering

	auto signals = coppeliasimHandler.getSignals();

	// Initial handling to limit the raw values
	double rawHandProximity = std::min(signals.hand_proximity, 10.0f);
	double rawHandY = std::max(0.0f, std::min(signals.hand_y, 100.0f));

	// Apply exponential moving average (EMA) filter
	double handProximityEMA = (alpha * rawHandProximity) + ((1 - alpha) * lastHandProximityEMA);
	double handYEMA = (alpha * rawHandY) + ((1 - alpha) * lastHandYEMA);

	// Update the last EMA values for the next call
	lastHandProximityEMA = handProximityEMA;
	lastHandYEMA = handYEMA;

	// Now use the smoothed values for further processing
	dnfcomposerHandler.setHandStimulus(handYEMA, handProximityEMA);
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
		updateHandPosition();
		updateAvailableObjects();
		updateTargetObject();
		Sleep(10);
	} while (!taskFinished);
}

bool Experiment::areObjectsPresent() const
{
	auto signals = coppeliasimHandler.getSignals();
	bool isPresent = signals.object1 != 0 || signals.object2 != 0 || signals.object3 != 0;
	return isPresent;
}

