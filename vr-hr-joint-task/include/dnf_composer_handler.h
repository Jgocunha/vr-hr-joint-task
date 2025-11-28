#pragma once

#include <memory>
#include <thread>

#include <application/application.h>
#include <simulation/simulation.h>
#include "dnf_composer/user_interface/plots_window.h"
#include "dnf_composer/user_interface/node_graph_window.h"
#include "dnf_composer/user_interface/element_window.h"
#include "dnf_architecture.h"
#include "misc.h"

class DnfComposerHandler
{
private:
	DnfArchitectureType dnf;
	std::shared_ptr<dnf_composer::Simulation> simulation;
	std::shared_ptr<dnf_composer::Application> application;
	std::shared_ptr<dnf_composer::Visualization> visualization;
	std::thread simulationThread;
public:
	DnfComposerHandler(DnfArchitectureType dnf, double deltaT);
	~DnfComposerHandler();

	void init();
	void run();
	void end();

	void setHandStimulus(const Position& position, 
		bool object1,
		bool object2,
		bool object3) const;
	int getTargetObject() const;
	void setAvailableObjectsInTheWorkspace(bool object1, bool object2, bool object3,bool object4, bool object5, bool object6,  bool object7, bool object8) const;
	DnfArchitectureType getDnfType() const { return dnf; }
private:
	void setHandStimulusDependingOnHumanActionLikelihood(const Position& position, 
		bool object1, 
		bool object2, 
		bool object3) const;
	void setHandStimulusDependingOnHumanHandPosition(const Position& position) const;
	static double calculateHandDistanceToObjects(const Position& position);
	static double calculateHandProximityToObjects(double distance);
	static double normalizeHandPosition(double handPositionY);
	void setupUserInterface() const;
};