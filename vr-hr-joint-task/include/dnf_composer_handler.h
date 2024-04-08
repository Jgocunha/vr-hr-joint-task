#pragma once

#include <memory>
#include <thread>

#include <application/application.h>
#include <simulation/simulation.h>
#include <user_interface/plot_window.h>

#include "dnf_architecture.h"
#include "misc.h"

class DnfComposerHandler
{
private:
	DnfArchitectureType dnf;
	std::shared_ptr<dnf_composer::Simulation> simulation;
	std::shared_ptr<dnf_composer::Application> application;
	std::thread simulationThread;
public:
	DnfComposerHandler(DnfArchitectureType dnf, double deltaT);
	~DnfComposerHandler();

	void init();
	void run() const;
	void end();

	void setHandStimulus(const Position& position) const;
	int getTargetObject() const;
	void setAvailableObjectsInTheWorkspace(bool object1, bool object2, bool object3) const;
private:
	static double calculateHandDistanceToObjects(double handPositionX, double handPositionZ);
	static double calculateHandProximityToObjects(double distance);
	static double normalizeHandPosition(double handPositionY);
	void setupUserInterface() const;
};
