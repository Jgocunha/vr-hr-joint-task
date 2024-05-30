#include "experiment.h"

Experiment::Experiment(const ExperimentParameters& parameters)
	: dnfComposerHandler(parameters.dnf, parameters.deltaT)
	, coppeliasimHandler()
	, inSignals()
	, outSignals()
	, handPose({},{})
	, logMsgs()
	, numTrials(parameters.numTrials)
	, trialCounter(0)
{
	switch (parameters.dnf)
	{
	case DnfArchitectureType::HAND_MOTION:
	case DnfArchitectureType::ACTION_LIKELIHOOD:
		outSignals.archType = 1;
	break;
	case DnfArchitectureType::NO_ANTICIPATION:
		case DnfArchitectureType::BASELINE:
		outSignals.archType = 0;
	break;
	}
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
	keepAliveWhileTaskIsRunning();
}

void Experiment::end()
{
	using namespace vr_hr_joint_task::tools;
	logger::log(logger::LogLevel::INFO, "Ending experiment...");
	dnfComposerHandler.end();
	coppeliasimHandler.end();
	if (experimentThread.joinable())
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
		if (dnfComposerHandler.getDnfType() != DnfArchitectureType::BASELINE)
			sendTargetObjectToRobot();
		interpretAndLogSystemState();
		coppeliasimHandler.setSignals(outSignals);
	}
}

void Experiment::waitForConnectionWithCoppeliasim()
{
	using namespace vr_hr_joint_task::tools;
	while (!coppeliasimHandler.isConnected())
	{
		logger::log(logger::LogLevel::INFO, "Waiting for connection with CoppeliaSim...");
		Sleep(500);
	}
	logger::log(logger::LogLevel::INFO, "Connected with CoppeliaSim.");
	EventLogger::log(LogLevel::CONTROL, "Connected with CoppeliaSim.");
}

void Experiment::waitForSimulationToStart()
{
	using namespace vr_hr_joint_task::tools;
	bool hasSimStarted = inSignals.simStarted;
	while (!hasSimStarted)
	{
		outSignals.startSim = true;
		logger::log(logger::LogLevel::INFO, "Waiting for Simulation to start...");
		hasSimStarted = inSignals.simStarted;
		Sleep(500);
	}
	logger::log(logger::LogLevel::INFO, "Simulation has started.");
}

void Experiment::sendHandPositionToDnf()
{
	handPose = coppeliasimHandler.getHandPose();
	dnfComposerHandler.setHandStimulus({
		handPose.position.x,
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
	static bool afterPlacingForceTargeting = true;
	static bool afterTargetingForceGrasping = false;
	static bool afterGraspingForcePlacing = false;
	static int placeCount = 0;
	static int newTargetCount = 0;

	if (!(handPose.position.x == 0.0 && handPose.position.y == 0.0 && handPose.position.z == 0.0))
	{
		const std::string log = "Hand pose: x = "
			+ std::to_string(handPose.position.x) +
			", y = " + std::to_string(handPose.position.y) +
			", z = " + std::to_string(handPose.position.z) +
			", alpha = " + std::to_string(handPose.orientation.alpha) +
			", beta = " + std::to_string(handPose.orientation.beta) +
			", gamma = " + std::to_string(handPose.orientation.gamma);
		EventLogger::logHumanHandPose(log);
	}

	if (/*inSignals.canRestart && logMsgs.prevSimFinished == false &&*/ placeCount >= 3)
	{
		using namespace vr_hr_joint_task::tools;
		EventLogger::log(LogLevel::CONTROL, "Task has finished.");
		logger::log(logger::LogLevel::INFO, "Task has finished.");
		EventLogger::logHumanHandPose("Task has finished.");
		EventLogger::log(LogLevel::CONTROL, "Re-planning count: " + std::to_string(inSignals.replanningCount));
		logger::log(logger::LogLevel::INFO, "Re-planning count: " + std::to_string(inSignals.replanningCount));
		EventLogger::log(LogLevel::CONTROL, "Collision count: " + std::to_string(inSignals.collisionCounter));
		logger::log(logger::LogLevel::INFO, "Collision count: " + std::to_string(inSignals.collisionCounter));
		EventLogger::log(LogLevel::CONTROL, "Human idle time: " + std::to_string(inSignals.humanIdleTime));
		logger::log(logger::LogLevel::INFO, "Human idle time: " + std::to_string(inSignals.humanIdleTime));
		EventLogger::log(LogLevel::CONTROL, "Robot idle time: " + std::to_string(inSignals.robotIdleTime));
		logger::log(logger::LogLevel::INFO, "Robot idle time: " + std::to_string(inSignals.robotIdleTime));
		EventLogger::log(LogLevel::CONTROL, "Common grasp: " + std::to_string(inSignals.commonGrasp));
		logger::log(logger::LogLevel::INFO, "Common grasp: " + std::to_string(inSignals.commonGrasp));
		logMsgs.prevSimFinished = true;
		afterPlacingForceTargeting = true;
		afterTargetingForceGrasping = false;
		afterGraspingForcePlacing = false;
		logMsgs.lastTargetObject = 0;
		afterGraspingForcePlacing = false; placeCount = 0;
		newTargetCount = 0;
		trialCounter++;
		logger::log(logger::LogLevel::INFO, "Trial " + std::to_string(trialCounter) + " out of " + std::to_string(numTrials));
	}

	if(inSignals.simStarted && logMsgs.prevSimStarted == false)
	{
		using namespace vr_hr_joint_task::tools;
		logger::log(logger::LogLevel::INFO, "Task has started.");
		EventLogger::log(LogLevel::CONTROL, "Task has started.");
		EventLogger::logHumanHandPose("Task has started.");
		logMsgs.prevSimStarted = true;
		logMsgs.prevSimFinished = false;
		afterPlacingForceTargeting = true;
		afterTargetingForceGrasping = false;		
		logMsgs.lastTargetObject = 0;
		afterGraspingForcePlacing = false;		
		placeCount = 0;
		newTargetCount = 0;
	}
	logMsgs.prevSimStarted = inSignals.simStarted;

	// Check if the robot is approaching a new object.
	//if (inSignals.robotApproaching && /*!inSignals.robotGrasping && */outSignals.targetObject != logMsgs.lastTargetObject) {
	if (/*inSignals.robotApproaching &&*/afterPlacingForceTargeting && outSignals.targetObject != logMsgs.lastTargetObject) {
		if (outSignals.targetObject != 0)
		{
			EventLogger::log(LogLevel::ROBOT, "Robot will target object " + std::to_string(outSignals.targetObject) + ".");
			using namespace vr_hr_joint_task::tools;
			logger::log(logger::LogLevel::INFO, "Robot will target object " + std::to_string(outSignals.targetObject) + ".");
			newTargetCount++;
			afterPlacingForceTargeting = false;
			afterTargetingForceGrasping = true;
			logMsgs.lastTargetObject = outSignals.targetObject;
		}
	}

	// Grasping events for robot, logged every time it passes from 0 to 1.
	if (inSignals.robotGraspObj1 && logMsgs.prevRobotGraspObj1 == 0 && afterTargetingForceGrasping/**/) {
		EventLogger::log(LogLevel::ROBOT, "Robot is grasping object 1.");
		using namespace vr_hr_joint_task::tools;
		logger::log(logger::LogLevel::INFO, "Robot is grasping object 1.");
		afterTargetingForceGrasping = false;
		afterGraspingForcePlacing = true;
	}
	logMsgs.prevRobotGraspObj1 = inSignals.robotGraspObj1;

	if (inSignals.robotGraspObj2 && logMsgs.prevRobotGraspObj2 == 0 && afterTargetingForceGrasping/**/) {
		EventLogger::log(LogLevel::ROBOT, "Robot is grasping object 2.");
		using namespace vr_hr_joint_task::tools;
		logger::log(logger::LogLevel::INFO, "Robot is grasping object 2.");
		afterTargetingForceGrasping = false;
		afterGraspingForcePlacing = true;
	}
	logMsgs.prevRobotGraspObj2 = inSignals.robotGraspObj2;

	if (inSignals.robotGraspObj3 && logMsgs.prevRobotGraspObj3 == 0&& afterTargetingForceGrasping /**/) {
		EventLogger::log(LogLevel::ROBOT, "Robot is grasping object 3.");
		using namespace vr_hr_joint_task::tools;
		logger::log(logger::LogLevel::INFO, "Robot is grasping object 3.");
		afterTargetingForceGrasping = false;
		afterGraspingForcePlacing = true;
	}
	logMsgs.prevRobotGraspObj3 = inSignals.robotGraspObj3;

	// Grasping events for human, logged every time it passes from 0 to 1.
	if (inSignals.humanGraspObj1 && logMsgs.prevHumanGraspObj1 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is grasping object 1.");
		using namespace vr_hr_joint_task::tools;
		logger::log(logger::LogLevel::INFO, "Human is grasping object 1.");
		EventLogger::logHumanHandPose("Human is grasping object 1.");
	}
	logMsgs.prevHumanGraspObj1 = inSignals.humanGraspObj1;

	if (inSignals.humanGraspObj2 && logMsgs.prevHumanGraspObj2 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is grasping object 2.");
		using namespace vr_hr_joint_task::tools;
		logger::log(logger::LogLevel::INFO, "Human is grasping object 2.");
		EventLogger::logHumanHandPose("Human is grasping object 2.");
	}
	logMsgs.prevHumanGraspObj2 = inSignals.humanGraspObj2;

	if (inSignals.humanGraspObj3 && logMsgs.prevHumanGraspObj3 == 0) {
		EventLogger::log(LogLevel::HUMAN, "Human is grasping object 3.");
		using namespace vr_hr_joint_task::tools;
		logger::log(logger::LogLevel::INFO, "Human is grasping object 3.");
		EventLogger::logHumanHandPose("Human is grasping object 3.");
	}
	logMsgs.prevHumanGraspObj3 = inSignals.humanGraspObj3;

	// Placement events for robot, logged every time it passes from 0 to 1.
	if (inSignals.robotPlaceObj1 && logMsgs.prevRobotPlaceObj1 == 0 /*&& afterGraspingForcePlacing*/) {
		using namespace vr_hr_joint_task::tools;
		EventLogger::log(LogLevel::ROBOT, "Robot is placing object 1.");
		logger::log(logger::LogLevel::INFO, "Robot is placing object 1.");
		afterGraspingForcePlacing = false;
		afterPlacingForceTargeting = true;
		placeCount++;
		logger::log(logger::LogLevel::INFO, "Place count: " + std::to_string(placeCount));
	}
	logMsgs.prevRobotPlaceObj1 = inSignals.robotPlaceObj1;

	if (inSignals.robotPlaceObj2 && logMsgs.prevRobotPlaceObj2 == 0 /*&& afterGraspingForcePlacing*/) {
		using namespace vr_hr_joint_task::tools;
		EventLogger::log(LogLevel::ROBOT, "Robot is placing object 2.");
		logger::log(logger::LogLevel::INFO, "Robot is placing object 2.");
		afterGraspingForcePlacing = false;
		afterPlacingForceTargeting = true;
		placeCount++;
		logger::log(logger::LogLevel::INFO, "Place count: " + std::to_string(placeCount));
	}
	logMsgs.prevRobotPlaceObj2 = inSignals.robotPlaceObj2;

	if (inSignals.robotPlaceObj3 && logMsgs.prevRobotPlaceObj3 == 0 /*&& afterGraspingForcePlacing*/) {
		using namespace vr_hr_joint_task::tools;
		EventLogger::log(LogLevel::ROBOT, "Robot is placing object 3.");
		logger::log(logger::LogLevel::INFO, "Robot is placing object 3.");
		afterGraspingForcePlacing = false;
		afterPlacingForceTargeting = true;
		placeCount++;
		logger::log(logger::LogLevel::INFO, "Place count: " + std::to_string(placeCount));
	}
	logMsgs.prevRobotPlaceObj3 = inSignals.robotPlaceObj3;

	// Placement events for human, logged every time it passes from 0 to 1.
	if (inSignals.humanPlaceObj1 && logMsgs.prevHumanPlaceObj1 == 0) {
		using namespace vr_hr_joint_task::tools;
		EventLogger::log(LogLevel::HUMAN, "Human is placing object 1.");
		EventLogger::logHumanHandPose("Human is placing object 1.");
		logger::log(logger::LogLevel::INFO, "Human is placing object 1.");
		placeCount++;
		logger::log(logger::LogLevel::INFO, "Place count: " + std::to_string(placeCount));
	}
	logMsgs.prevHumanPlaceObj1 = inSignals.humanPlaceObj1;

	if (inSignals.humanPlaceObj2 && logMsgs.prevHumanPlaceObj2 == 0) {
		using namespace vr_hr_joint_task::tools;
		EventLogger::log(LogLevel::HUMAN, "Human is placing object 2.");
		EventLogger::logHumanHandPose("Human is placing object 2.");
		logger::log(logger::LogLevel::INFO, "Human is placing object 2.");
		placeCount++;
		logger::log(logger::LogLevel::INFO, "Place count: " + std::to_string(placeCount));
	}
	logMsgs.prevHumanPlaceObj2 = inSignals.humanPlaceObj2;

	if (inSignals.humanPlaceObj3 && logMsgs.prevHumanPlaceObj3 == 0) {
		using namespace vr_hr_joint_task::tools;
		EventLogger::log(LogLevel::HUMAN, "Human is placing object 3.");
		EventLogger::logHumanHandPose("Human is placing object 3.");
		logger::log(logger::LogLevel::INFO, "Human is placing object 3.");
		placeCount++;
		logger::log(logger::LogLevel::INFO, "Place count: " + std::to_string(placeCount));
	}
	logMsgs.prevHumanPlaceObj3 = inSignals.humanPlaceObj3;
}

void Experiment::keepAliveWhileTaskIsRunning()
{
	while (trialCounter < numTrials)
	{
		Sleep(10);
	}
	// For now let's keep alive for a few seconds after the task is done.
	killEverything = true;
	end();
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
