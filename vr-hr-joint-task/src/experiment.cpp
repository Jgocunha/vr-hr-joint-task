#include "experiment.h"


Experiment::Experiment(std::string name, int commsFreq, double deltaT)
		: coppeliasimHandler(), dnfcomposerHandler({ std::move(name), deltaT }), commsFrequency(commsFreq)
{
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
	waitForObjectsToBeCreated();
	keepAliveWhileTaskIsRunning();
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

void Experiment::waitForObjectsToBeCreated() const
{
	bool areObjectsPresentInScene = areObjectsPresent();
	while (!areObjectsPresentInScene)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be created...\n");
		areObjectsPresentInScene = areObjectsPresent();
		Sleep(commsFrequency);
	}

	log(dnf_composer::tools::logger::LogLevel::INFO, "Pick and place will now start...\n");
}

void Experiment::keepAliveWhileTaskIsRunning() const
{
	while(areObjectsPresent())
	{
		Sleep(commsFrequency);
	}
	// For now let's keep alive
	// for a few seconds after the task is done.
	Sleep(10000);
}

void Experiment::updateHandPosition() const
{
	dnfcomposerHandler.setHandStimulus(coppeliasimSignals.hand_y, coppeliasimSignals.hand_proximity);
}

void Experiment::updateAvailableObjects() const
{
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
	dnfcomposerSignals.targetObject = dnfcomposerHandler.getTargetObject();
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
	const bool isPresent = coppeliasimSignals.object1 != 0 || coppeliasimSignals.object2 != 0 || coppeliasimSignals.object3 != 0;
	return isPresent;
}