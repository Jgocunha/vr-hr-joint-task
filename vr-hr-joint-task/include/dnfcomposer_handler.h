#pragma once
#include <simulation/simulation.h>
#include <thread>
#include <application/application.h>
#include <user_interface/plot_window.h>

#include "experiment_window.h"
#include "dnf_architecture.h"
#include "misc.h"


struct SimulationParameters
{
	std::string identifier;
	double deltaT;
	bool uiActive;

	SimulationParameters(std::string identifier = "simulation", double deltaT = 10, bool uiActive = true)
		: identifier(std::move(identifier)), deltaT(deltaT), uiActive(uiActive)
	{}
};

class DNFComposerHandler
{
private:
	std::shared_ptr<dnf_composer::Simulation> simulation;
	std::shared_ptr<dnf_composer::Application> application;
	std::shared_ptr<ExperimentWindow> experimentWindow;
	std::thread dnfcomposerThread;
public:
	DNFComposerHandler(const SimulationParameters& simParams);
	~DNFComposerHandler();

	void init();
	void run() const;
	void close();
	void setHandStimulus(const double& hand_y, const double& hand_proximity) const;
	int getTargetObject() const;
	void addTargetObject(int objectIndex) const;
	void removeTargetObject(int objectIndex) const;
private:
	void setupUserInterface() const;
	static double transformXToCircular(const double& x);
};
