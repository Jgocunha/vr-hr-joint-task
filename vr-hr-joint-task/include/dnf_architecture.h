#pragma once

#include <elements/element_factory.h>

enum class DnfArchitectureType
{
	NEUTRAL,
	NO_ANTICIPATION,
	BASELINE,
	KNOWN
};

std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureNeutral(const std::string& id, const double& deltaT);

std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureKnown(const std::string& id, const double& deltaT);

std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureNoAnticipation(const std::string& id, const double& deltaT);
