#pragma once

#include "dnf_architecture.h"
#include "dnf_composer_handler.h"
#include "coppeliasim_handler.h"

struct ExperimentParameters
{
	DnfArchitectureType dnf;
	double deltaT;

	ExperimentParameters(DnfArchitectureType dnf, double deltaT)
	: dnf(dnf), deltaT(deltaT)
	{}
};

class Experiment
{
private:
	DnfComposerHandler dnfComposerHandler;
	CoppeliasimHandler coppeliasimHandler;
	std::thread experimentThread;
	Signals signals;
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

	void sendHandPositionToDnf() const;
	void sendAvailableObjectsToDnf() const;
	void sendTargetObjectToRobot();

	void keepAliveWhileTaskIsRunning() const;
	bool areObjectsPresent() const;
};