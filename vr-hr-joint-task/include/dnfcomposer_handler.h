#pragma once
#include <simulation/simulation.h>
#include <thread>
#include <application/application.h>

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
	std::thread dnfcomposerThread;
public:
	DNFComposerHandler(const SimulationParameters& simParams);
	~DNFComposerHandler() = default;

	void init();
	void run();
	void close();
private:
	void setupUserInterface() const;
};
