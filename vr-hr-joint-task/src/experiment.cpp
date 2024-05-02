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
		inSignals = coppeliasimHandler.getSignals();
		sendHandPositionToDnf();
		sendAvailableObjectsToDnf();
		sendTargetObjectToRobot();
		interpretAndLogSystemState();
		coppeliasimHandler.setSignals(outSignals);
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
	bool hasSimStarted = inSignals.simStarted;
	while (!hasSimStarted)
	{
		outSignals.startSim = true;
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for Simulation to start...\n");
		hasSimStarted = inSignals.simStarted;
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
		inSignals.object1,
		inSignals.object2,
		inSignals.object3);
}

void Experiment::sendAvailableObjectsToDnf() const
{
	dnfComposerHandler.setAvailableObjectsInTheWorkspace(inSignals.object1, inSignals.object2, inSignals.object3);
}

void Experiment::sendTargetObjectToRobot()
{
	outSignals.targetObject = dnfComposerHandler.getTargetObject();
}

void Experiment::interpretAndLogSystemState()
{
	static bool afterGraspingForcePlacing = false;
	static bool afterPlacingForceTargeting = false;
	static int placeCount = 0;
	static int newTargetCount = 0;

	const std::string log = "Hand pose: x = "
		+ std::to_string(handPose.position.x) +
		", y = " + std::to_string(handPose.position.y) +
		", z = " + std::to_string(handPose.position.z) +
		", alpha = " + std::to_string(handPose.orientation.alpha) +
		", beta = " + std::to_string(handPose.orientation.beta) +
		", gamma = " + std::to_string(handPose.orientation.gamma);
	EventLogger::logHumanHandPose(log);

	if (inSignals.canRestart && logMsgs.prevSimFinished == false && placeCount >= 3)
	{
		EventLogger::log(LogLevel::CONTROL, "Task has finished.");
		EventLogger::log(LogLevel::CONTROL, "Re-planning count: " + std::to_string(newTargetCount));
		EventLogger::log(LogLevel::CONTROL, "Collision count: " + std::to_string(inSignals.collisionCounter));
		EventLogger::log(LogLevel::CONTROL, "Human idle time: " + std::to_string(inSignals.humanIdleTime));
		EventLogger::log(LogLevel::CONTROL, "Robot idle time: " + std::to_string(inSignals.robotIdleTime));
		logMsgs.prevSimFinished = true;
		afterGraspingForcePlacing = false;
		afterPlacingForceTargeting = false;
		logMsgs.lastTargetObject = -1;
		placeCount = 0;
		newTargetCount = 0;
	}

	if(inSignals.simStarted && logMsgs.prevSimStarted == false)
	{
		EventLogger::log(LogLevel::CONTROL, "Task has started.");
		logMsgs.prevSimStarted = true;
		logMsgs.prevSimFinished = false;
		afterGraspingForcePlacing = false;
		afterPlacingForceTargeting = false;
		logMsgs.lastTargetObject = -1;
		placeCount = 0;
		newTargetCount = 0;
	}
	logMsgs.prevSimStarted = inSignals.simStarted;

	// Grasping events for robot, logged every time it passes from 0 to 1.
	if (inSignals.robotGraspObj1 && logMsgs.prevRobotGraspObj1 == 0 && !afterPlacingForceTargeting) {
		EventLogger::log(LogLevel::ROBOT, "Robot is grasping object 1.");
		afterGraspingForcePlacing = true;
	}
	logMsgs.prevRobotGraspObj1 = inSignals.robotGraspObj1;

	if (inSignals.robotGraspObj2 && logMsgs.prevRobotGraspObj2 == 0 && !afterPlacingForceTargeting) {
		EventLogger::log(LogLevel::ROBOT, "Robot is grasping object 2.");
		afterGraspingForcePlacing = true;
	}
	logMsgs.prevRobotGraspObj2 = inSignals.robotGraspObj2;

	if (inSignals.robotGraspObj3 && logMsgs.prevRobotGraspObj3 == 0 && !afterPlacingForceTargeting) {
		EventLogger::log(LogLevel::ROBOT, "Robot is grasping object 3.");
		afterGraspingForcePlacing = true;
	}
	logMsgs.prevRobotGraspObj3 = inSignals.robotGraspObj3;

	// Grasping events for human, logged every time it passes from 0 to 1.
	if (inSignals.humanGraspObj1 && logMsgs.prevHumanGraspObj1 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is grasping object 1.");
	}
	logMsgs.prevHumanGraspObj1 = inSignals.humanGraspObj1;

	if (inSignals.humanGraspObj2 && logMsgs.prevHumanGraspObj2 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is grasping object 2.");
	}
	logMsgs.prevHumanGraspObj2 = inSignals.humanGraspObj2;

	if (inSignals.humanGraspObj3 && logMsgs.prevHumanGraspObj3 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is grasping object 3.");
	}
	logMsgs.prevHumanGraspObj3 = inSignals.humanGraspObj3;

	// Placement events for robot, logged every time it passes from 0 to 1.
	if (inSignals.robotPlaceObj1 && logMsgs.prevRobotPlaceObj1 == 0) {
		EventLogger::log(LogLevel::ROBOT, "Robot is placing object 1.");
		afterGraspingForcePlacing = false;
		afterPlacingForceTargeting = true;
		placeCount++;
	}
	logMsgs.prevRobotPlaceObj1 = inSignals.robotPlaceObj1;

	if (inSignals.robotPlaceObj2 && logMsgs.prevRobotPlaceObj2 == 0) {
		EventLogger::log(LogLevel::ROBOT, "Robot is placing object 2.");
		afterGraspingForcePlacing = false;
		afterPlacingForceTargeting = true;
		placeCount++;
	}
	logMsgs.prevRobotPlaceObj2 = inSignals.robotPlaceObj2;

	if (inSignals.robotPlaceObj3 && logMsgs.prevRobotPlaceObj3 == 0) {
		EventLogger::log(LogLevel::ROBOT, "Robot is placing object 3.");
		afterGraspingForcePlacing = false;
		afterPlacingForceTargeting = true;
		placeCount++;
	}
	logMsgs.prevRobotPlaceObj3 = inSignals.robotPlaceObj3;

	// Placement events for human, logged every time it passes from 0 to 1.
	if (inSignals.humanPlaceObj1 && logMsgs.prevHumanPlaceObj1 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is placing object 1.");
		placeCount++;
	}
	logMsgs.prevHumanPlaceObj1 = inSignals.humanPlaceObj1;

	if (inSignals.humanPlaceObj2 && logMsgs.prevHumanPlaceObj2 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is placing object 2.");
		placeCount++;
	}
	logMsgs.prevHumanPlaceObj2 = inSignals.humanPlaceObj2;

	if (inSignals.humanPlaceObj3 && logMsgs.prevHumanPlaceObj3 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is placing object 3.");
		placeCount++;
	}
	logMsgs.prevHumanPlaceObj3 = inSignals.humanPlaceObj3;

	// Check if the robot is approaching a new object.
	//if (inSignals.robotApproaching && /*!inSignals.robotGrasping && */outSignals.targetObject != logMsgs.lastTargetObject) {
	if (inSignals.robotApproaching && !afterGraspingForcePlacing && outSignals.targetObject != logMsgs.lastTargetObject) {
		if (outSignals.targetObject != 0)
		{
			EventLogger::log(LogLevel::ROBOT, "Robot will target object " + std::to_string(outSignals.targetObject) + ".");
			newTargetCount++;
		}
		logMsgs.lastTargetObject = outSignals.targetObject;
		afterPlacingForceTargeting = false;
	}
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
	const bool isPresent = inSignals.object1 != 0 || inSignals.object2 != 0 || inSignals.object3 != 0;
	return isPresent;
}

bool Experiment::areAllObjectsPresent() const
{
	return inSignals.object1 != 0 && inSignals.object2 != 0 && inSignals.object3 != 0;
}
