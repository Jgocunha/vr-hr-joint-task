#pragma once

#include "dnf_architecture.h"
#include "dnf_composer_handler.h"
#include "coppeliasim_handler.h"
#include "event_logger.h"
#include "logger.h"

struct ExperimentParameters
{
	DnfArchitectureType dnf;
	double deltaT;
	uint8_t numTrials;

	int  participantId = 0;
	char condition     = 'N'; // N-neutral, K-known, I-inference
	char feature       = 'B'; // B-blue, Y-yellow, L-long, S-short

	ExperimentParameters(DnfArchitectureType dnf, double deltaT, int numTrials)
		: dnf(dnf), deltaT(deltaT), numTrials(numTrials)
	{}
};

struct LogMsgs
{
    int lastTargetObject = -1;
	int prevReplanningCount = 0;
	int prevTargetBeforeReplan = 0;	
	int newTargetAfterReplan = 0;
	bool waitingForPostReplanTarget = false;
	bool prevSimStarted = false;
    bool prevRobotGraspObj1 = false;
    bool prevRobotGraspObj2 = false;
    bool prevRobotGraspObj3 = false;
	bool prevRobotGraspObj4 = false;
	bool prevRobotGraspObj5 = false;
	bool prevRobotGraspObj6 = false;
	bool prevRobotGraspObj7 = false;
	bool prevRobotGraspObj8 = false;

    bool prevHumanGraspObj1 = false;
    bool prevHumanGraspObj2 = false;
    bool prevHumanGraspObj3 = false;
	bool prevHumanGraspObj4 = false;
	bool prevHumanGraspObj5 = false;
	bool prevHumanGraspObj6 = false;
	bool prevHumanGraspObj7 = false;
	bool prevHumanGraspObj8 = false;

    bool prevRobotPlaceObj1 = false;
    bool prevRobotPlaceObj2 = false;
    bool prevRobotPlaceObj3 = false;
	bool prevRobotPlaceObj4 = false;
	bool prevRobotPlaceObj5 = false;
	bool prevRobotPlaceObj6 = false;
	bool prevRobotPlaceObj7 = false;
	bool prevRobotPlaceObj8 = false;
	

    bool prevHumanPlaceObj1 = false;
    bool prevHumanPlaceObj2 = false;
    bool prevHumanPlaceObj3 = false;
	bool prevHumanPlaceObj4 = false;
	bool prevHumanPlaceObj5 = false;
	bool prevHumanPlaceObj6 = false;
	bool prevHumanPlaceObj7 = false;
	bool prevHumanPlaceObj8 = false;
	bool prevSimFinished = false;

    void clear()
	{
        lastTargetObject = -1;
		prevSimStarted = false;
        prevRobotGraspObj1 = false;
        prevRobotGraspObj2 = false;
        prevRobotGraspObj3 = false;
		prevRobotGraspObj4 = false;
		prevRobotGraspObj5 = false;
		prevRobotGraspObj6 = false;
		prevRobotGraspObj7 = false;
		prevRobotGraspObj8 = false;
        prevHumanGraspObj1 = false;
        prevHumanGraspObj2 = false;
        prevHumanGraspObj3 = false;
		prevHumanGraspObj4 = false;
		prevHumanGraspObj5 = false;
		prevHumanGraspObj6 = false;
		prevHumanGraspObj7 = false;
		prevHumanGraspObj8 = false;
        prevRobotPlaceObj1 = false;
        prevRobotPlaceObj2 = false;
        prevRobotPlaceObj3 = false;
		prevRobotPlaceObj4 = false;
		prevRobotPlaceObj5 = false;
		prevRobotPlaceObj6 = false;
		prevRobotPlaceObj7 = false;
		prevRobotPlaceObj8 = false;
        prevHumanPlaceObj1 = false;
        prevHumanPlaceObj2 = false;
        prevHumanPlaceObj3 = false;
		prevHumanPlaceObj4 = false;
		prevHumanPlaceObj5 = false;
		prevHumanPlaceObj6 = false;
		prevHumanPlaceObj7 = false;
		prevHumanPlaceObj8 = false;
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
	int numTrials;
	int trialCounter;
	int participantId;
	char condition;
	char feature;
public:
	Experiment(const ExperimentParameters& parameters);
	~Experiment();

	void init();
	void run();
	void end();
private:
	void handleSignalsBetweenDnfAndCoppeliasim();

	void waitForConnectionWithCoppeliasim() const;
	void waitForSimulationToStart();

	void sendHandPositionToDnf();
	void sendAvailableObjectsToDnf() const;
	void sendTargetObjectToRobot();
	void interpretAndLogSystemState();

	void keepAliveWhileTaskIsRunning();
	bool areObjectsPresent() const;
	bool areAllObjectsPresent() const;
};

// global helpers
inline int askParticipantId()
{
	int id;
	while (true)
	{
		std::cout << "What is the participant id? (enter only a valid integer): ";
		if (std::cin >> id && id > 0)
		{
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return id;
		}
		std::cout << "Invalid participant id. Please enter a positive integer.\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}

inline char askCondition()
{
	while (true)
	{
		std::cout << "What is the condition? (Nn - neutral; Kk - known; Ii - inference): ";
		char c;
		if (std::cin >> c)
		{
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
			if (c == 'N' || c == 'K' || c == 'I')
				return c;
		}
		std::cout << "Invalid condition. Please enter N, K, or I.\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}

inline char askFeature()
{
	while (true)
	{
		std::cout << "What is the feature? (Bb - blue, Yy - yellow, Ll - long, Ss - short): ";
		char f;
		if (std::cin >> f)
		{
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			f = static_cast<char>(std::toupper(static_cast<unsigned char>(f)));
			if (f == 'B' || f == 'Y' || f == 'L' || f == 'S')
				return f;
		}
		std::cout << "Invalid feature. Please enter B, Y, L, or S.\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}
