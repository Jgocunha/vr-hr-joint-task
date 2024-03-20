#include "experiment.h"


Experiment::Experiment(std::string name, int commsFreq, double deltaT)
		: coppeliasimHandler(), dnfcomposerHandler({ std::move(name), deltaT }), commsFrequency(commsFreq)
{
	finished = false;
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
}

void Experiment::main()
{
	waitForConnection();
	waitForSimulationStart();
	waitForObjectsToBeCreated();
	signalsThread = std::thread(&Experiment::updateSignals, this);
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
		while (!hasObjectBeenGrasped)
		{
			log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for object to be grasped...\n");
			hasObjectBeenGrasped = coppeliasimHandler.getSignals().objectGrasped;
			Sleep(commsFrequency);
		}
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT_GRASPED, 0);

		Sleep(commsFrequency);


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
	finished = true;
}


void Experiment::updateHandPosition() const
{
	coppeliasim_cpp::Position handPosition = coppeliasimHandler.getSimulationData().handPosition;
	dnfcomposerHandler.setHandStimulus({handPosition.x, handPosition.y, handPosition.z});
}

void Experiment::updateAvailableObjects()
{
	const coppeliasim_cpp::Position object1Position = coppeliasimHandler.getSimulationData().object1Position;
	const coppeliasim_cpp::Position object2Position = coppeliasimHandler.getSimulationData().object2Position;
	const coppeliasim_cpp::Position object3Position = coppeliasimHandler.getSimulationData().object3Position;

	// this needs to die quickly ugghhh

	if (!isObjectInWorkspace(object1Position))
	{
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT1_EXISTS, 0);
		dnfcomposerHandler.removeTargetObject(1);
	}
	else
	{
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT1_EXISTS, 1);
		dnfcomposerHandler.addTargetObject(1);
	}

	if (!isObjectInWorkspace(object2Position))
	{
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT2_EXISTS, 0);
		dnfcomposerHandler.removeTargetObject(2);
	}
	else
	{
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT2_EXISTS, 1);
		dnfcomposerHandler.addTargetObject(2);
	}

	if (!isObjectInWorkspace(object3Position))
	{
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT3_EXISTS, 0);
		dnfcomposerHandler.removeTargetObject(3);
	}
	else
	{
		coppeliasimHandler.setSignal(SignalSignatures::OBJECT3_EXISTS, 1);
		dnfcomposerHandler.addTargetObject(3);
	}
}

void Experiment::updateTargetObject()
{
	const int targetObject = dnfcomposerHandler.getTargetObject();
	coppeliasimHandler.setSignal(SignalSignatures::TARGET_OBJECT, targetObject);
}


void Experiment::updateSignals()
{
	while (!finished)
	{
		updateHandPosition();
		updateAvailableObjects();
		updateTargetObject();
		Sleep(1);
	}
}

bool Experiment::isObjectInWorkspace(const coppeliasim_cpp::Position& position)
{
	log(dnf_composer::tools::logger::LogLevel::DEBUG, "Checking if object is in workspace...\n");
	log(dnf_composer::tools::logger::LogLevel::DEBUG, "Object position: " + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + '\n');

	if(position.z < 0.65)
	{
		log(dnf_composer::tools::logger::LogLevel::FATAL, "Object is not in workspace.\n");
		return true;
	}

	// Define workspace bounds
	constexpr double min_x = 0.55; // Minimum x-coordinate
	constexpr double max_x = 0.65;  // Maximum x-coordinate
	constexpr double min_y = -0.4; // Minimum y-coordinate
	constexpr double max_y = 0.1;  // Maximum y-coordinate
	constexpr double min_z = 0.65;  // Minimum z-coordinate
	constexpr double max_z = 0.75;  // Maximum z-coordinate

	return position.x >= min_x && position.x <= max_x &&
		position.y >= min_y && position.y <= max_y &&
		position.z >= min_z && position.z <= max_z;
}
