#pragma once

#include <elements/element_factory.h>

enum class DnfArchitectureType
{
	NEUTRAL,
	NO_ANTICIPATION,
	BASELINE,
	KNOWN_BLUE,
	KNOWN_YELLOW,
	KNOWN_LONG,
	KNOWN_SHORT
};

std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureNeutral(const std::string& id, const double& deltaT);

std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureKnownBlue(const std::string& id, const double& deltaT);
	std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureKnownYellow(const std::string& id, const double& deltaT);
	std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureKnownLong(const std::string& id, const double& deltaT);
	std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureKnownShort(const std::string& id, const double& deltaT);

std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureNoAnticipation(const std::string& id, const double& deltaT);
