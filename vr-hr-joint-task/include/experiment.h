#pragma once

#include "dnf_architecture.h"
#include "dnf_composer_handler.h"
#include "coppeliasim_handler.h"
#include "event_logger.h"

struct ExperimentParameters
{
	DnfArchitectureType dnf;
	double deltaT;

	ExperimentParameters(DnfArchitectureType dnf, double deltaT)
	: dnf(dnf), deltaT(deltaT)
	{}
};

struct LogMsgs
{
    int lastTargetObject = -1;
	bool prevSimStarted = false;
    bool prevRobotGraspObj1 = false;
    bool prevRobotGraspObj2 = false;
    bool prevRobotGraspObj3 = false;
    bool prevHumanGraspObj1 = false;
    bool prevHumanGraspObj2 = false;
    bool prevHumanGraspObj3 = false;
    bool prevRobotPlaceObj1 = false;
    bool prevRobotPlaceObj2 = false;
    bool prevRobotPlaceObj3 = false;
    bool prevHumanPlaceObj1 = false;
    bool prevHumanPlaceObj2 = false;
    bool prevHumanPlaceObj3 = false;
	bool prevSimFinished = false;

    void clear()
	{
        lastTargetObject = -1;
		prevSimStarted = false;
        prevRobotGraspObj1 = false;
        prevRobotGraspObj2 = false;
        prevRobotGraspObj3 = false;
        prevHumanGraspObj1 = false;
        prevHumanGraspObj2 = false;
        prevHumanGraspObj3 = false;
        prevRobotPlaceObj1 = false;
        prevRobotPlaceObj2 = false;
        prevRobotPlaceObj3 = false;
        prevHumanPlaceObj1 = false;
        prevHumanPlaceObj2 = false;
        prevHumanPlaceObj3 = false;
		prevSimFinished = false;
    }
};

class Experiment
{
private:
	DnfComposerHandler dnfComposerHandler;
	CoppeliasimHandler coppeliasimHandler;
	std::thread experimentThread;
	IncomingSignals inSignals;
	OutgoingSignals outSignals;
	Pose handPose;
	LogMsgs logMsgs;
public:
	Experiment(const ExperimentParameters& parameters);
	~Experiment();

	void init();
	void run();
	void end();
private:
	void handleSignalsBetweenDnfAndCoppeliasim();

	void waitForConnectionWithCoppeliasim();
	void waitForSimulationToStart();

	void sendHandPositionToDnf();
	void sendAvailableObjectsToDnf() const;
	void sendTargetObjectToRobot();
	void interpretAndLogSystemState();

	void keepAliveWhileTaskIsRunning() const;
	bool areObjectsPresent() const;
	bool areAllObjectsPresent() const;
};