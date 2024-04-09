#include "experiment.h"

Experiment::Experiment(const ExperimentParameters& parameters)
	: dnfComposerHandler(parameters.dnf, parameters.deltaT)
	, coppeliasimHandler()
	, handPose({},{})
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
		interpretAndLogSystemState();
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
	log(dnf_composer::tools::logger::LogLevel::INFO, "Connected with CoppeliaSim.\n");
	EventLogger::log(LogLevel::CONTROL, "Connected with CoppeliaSim.");
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
	log(dnf_composer::tools::logger::LogLevel::INFO, "Simulation has started.\n");
}

void Experiment::sendHandPositionToDnf()
{
	handPose = coppeliasimHandler.getHandPose();
	dnfComposerHandler.setHandStimulus({ handPose.position.x,
		handPose.position.y,
		handPose.position.z},
		signals.object1, 
		signals.object2, 
		signals.object3);
}

void Experiment::sendAvailableObjectsToDnf() const
{
	dnfComposerHandler.setAvailableObjectsInTheWorkspace(signals.object1, signals.object2, signals.object3);
}

void Experiment::sendTargetObjectToRobot()
{
	signals.targetObject = dnfComposerHandler.getTargetObject();
}

void Experiment::interpretAndLogSystemState()
{
	const std::string log = "Hand pose: x = "
		+ std::to_string(handPose.position.x) +
		", y = " + std::to_string(handPose.position.y) +
		", z = " + std::to_string(handPose.position.z) +
		", alpha = " + std::to_string(handPose.orientation.alpha) +
		", beta = " + std::to_string(handPose.orientation.beta) +
		", gamma = " + std::to_string(handPose.orientation.gamma);
	EventLogger::logHumanHandPose(log);

	if(signals.simStarted && logMsgs.prevSimStarted == false)
	{
		EventLogger::log(LogLevel::CONTROL, "Simulation has started.");
		logMsgs.prevSimStarted = true;
	}
	logMsgs.prevSimStarted = signals.simStarted;

	// Check if the robot is approaching a new object.
	if (signals.robotApproaching && signals.targetObject != logMsgs.lastTargetObject) {
		if (signals.targetObject != 0)
			EventLogger::log(LogLevel::ROBOT, "Robot is approaching object " + std::to_string(signals.targetObject) + ".");
		logMsgs.lastTargetObject = signals.targetObject;
	}

	// Grasping events for robot, logged every time it passes from 0 to 1.
	if (signals.robotGraspObj1 && logMsgs.prevRobotGraspObj1 == 0) {
		EventLogger::log(LogLevel::ROBOT, "Robot is grasping object 1.");
	}
	logMsgs.prevRobotGraspObj1 = signals.robotGraspObj1;

	if (signals.robotGraspObj2 && logMsgs.prevRobotGraspObj2 == 0) {
		EventLogger::log(LogLevel::ROBOT, "Robot is grasping object 2.");
	}
	logMsgs.prevRobotGraspObj2 = signals.robotGraspObj2;

	if (signals.robotGraspObj3 && logMsgs.prevRobotGraspObj3 == 0) {
		EventLogger::log(LogLevel::ROBOT, "Robot is grasping object 3.");
	}
	logMsgs.prevRobotGraspObj3 = signals.robotGraspObj3;

	// Grasping events for human, logged every time it passes from 0 to 1.
	if (signals.humanGraspObj1 && logMsgs.prevHumanGraspObj1 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is grasping object 1.");
	}
	logMsgs.prevHumanGraspObj1 = signals.humanGraspObj1;

	if (signals.humanGraspObj2 && logMsgs.prevHumanGraspObj2 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is grasping object 2.");
	}
	logMsgs.prevHumanGraspObj2 = signals.humanGraspObj2;

	if (signals.humanGraspObj3 && logMsgs.prevHumanGraspObj3 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is grasping object 3.");
	}
	logMsgs.prevHumanGraspObj3 = signals.humanGraspObj3;

	// Placement events for robot, logged every time it passes from 0 to 1.
	if (signals.robotPlaceObj1 && logMsgs.prevRobotPlaceObj1 == 0) {
		EventLogger::log(LogLevel::ROBOT, "Robot is placing object 1.");
	}
	logMsgs.prevRobotPlaceObj1 = signals.robotPlaceObj1;

	if (signals.robotPlaceObj2 && logMsgs.prevRobotPlaceObj2 == 0) {
		EventLogger::log(LogLevel::ROBOT, "Robot is placing object 2.");
	}
	logMsgs.prevRobotPlaceObj2 = signals.robotPlaceObj2;

	if (signals.robotPlaceObj3 && logMsgs.prevRobotPlaceObj3 == 0) {
		EventLogger::log(LogLevel::ROBOT, "Robot is placing object 3.");
	}
	logMsgs.prevRobotPlaceObj3 = signals.robotPlaceObj3;

	// Placement events for human, logged every time it passes from 0 to 1.
	if (signals.humanPlaceObj1 && logMsgs.prevHumanPlaceObj1 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is placing object 1.");
	}
	logMsgs.prevHumanPlaceObj1 = signals.humanPlaceObj1;

	if (signals.humanPlaceObj2 && logMsgs.prevHumanPlaceObj2 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is placing object 2.");
	}
	logMsgs.prevHumanPlaceObj2 = signals.humanPlaceObj2;

	if (signals.humanPlaceObj3 && logMsgs.prevHumanPlaceObj3 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is placing object 3.");
	}
	logMsgs.prevHumanPlaceObj3 = signals.humanPlaceObj3;
}

void Experiment::keepAliveWhileTaskIsRunning() const
{
	while (true)
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

bool Experiment::areAllObjectsPresent() const
{
	return signals.object1 != 0 && signals.object2 != 0 && signals.object3 != 0;
}
