#include "experiment.h"

Experiment::Experiment(const ExperimentParameters& parameters)
	: dnfComposerHandler(parameters.dnf, parameters.deltaT)
	, coppeliasimHandler()
{

}

Experiment::~Experiment()
{
	end();
}

void Experiment::init()
{
	dnfComposerHandler.init();
	coppeliasimHandler.init();
	EventLogger::initialize();
}


void Experiment::run()
{
	waitForConnectionWithCoppeliasim();
	experimentThread = std::thread(&Experiment::handleSignalsBetweenDnfAndCoppeliasim, this);
	waitForSimulationToStart();
}

void Experiment::end()
{
	dnfComposerHandler.end();
	coppeliasimHandler.end();
	experimentThread.join();
	EventLogger::finalize();
}

void Experiment::handleSignalsBetweenDnfAndCoppeliasim()
{
	while (coppeliasimHandler.isConnected())
	{
		signals = coppeliasimHandler.getSignals();
		sendHandPositionToDnf();
		sendAvailableObjectsToDnf();
		sendTargetObjectToRobot();
		coppeliasimHandler.setSignals(signals.startSim, signals.targetObject);
	}
}

void Experiment::waitForConnectionWithCoppeliasim()
{
	while (!coppeliasimHandler.isConnected())
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for connection with CoppeliaSim...\n");
		Sleep(500);
	}
	log(dnf_composer::tools::logger::LogLevel::INFO, "Connected with CoppeliaSim\n");
}

void Experiment::waitForSimulationToStart()
{
	bool hasSimStarted = signals.simStarted;
	while (!hasSimStarted)
	{
		signals.startSim = true;
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for Simulation to start...\n");
		hasSimStarted = signals.simStarted;
		Sleep(500);
	}
	log(dnf_composer::tools::logger::LogLevel::INFO, "Simulation has started\n");
}

void Experiment::sendHandPositionToDnf() const
{
	const Pose hand = coppeliasimHandler.getHandPose();
	dnfComposerHandler.setHandStimulus({hand.position.x, 
		hand.position.y,
		hand.position.z},
		signals.object1, 
		signals.object2, 
		signals.object3);
	const std::string log = "Hand pose: x = "
	+ std::to_string(hand.position.x) +
		", y = " + std::to_string(hand.position.y) +
		", z = " + std::to_string(hand.position.z) +
		", alpha = " + std::to_string(hand.orientation.alpha) +
		", beta = " + std::to_string(hand.orientation.beta) +
		", gamma = " + std::to_string(hand.orientation.gamma);
	EventLogger::logHumanHandPose(log);
}

void Experiment::sendAvailableObjectsToDnf() const
{
	dnfComposerHandler.setAvailableObjectsInTheWorkspace(signals.object1, signals.object2, signals.object3);
}

void Experiment::sendTargetObjectToRobot()
{
	signals.targetObject = dnfComposerHandler.getTargetObject();
}

void Experiment::keepAliveWhileTaskIsRunning() const
{
	while (areObjectsPresent())
	{
		Sleep(1000);
	}
	// For now let's keep alive for a few seconds after the task is done.
	Sleep(10000);
}

bool Experiment::areObjectsPresent() const
{
	const bool isPresent = signals.object1 != 0 || signals.object2 != 0 || signals.object3 != 0;
	return isPresent;
}