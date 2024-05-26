#pragma once

#include <elements/element_factory.h>

enum class DnfArchitectureType
{
	HAND_MOTION,
	ACTION_LIKELIHOOD,
	NO_ANTICIPATION
};

std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureHandMotion(const std::string& id, const double& deltaT);

std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureActionLikelihood(const std::string& id, const double& deltaT);

std::shared_ptr<dnf_composer::Simulation>
	getDynamicNeuralFieldArchitectureNoAnticipation(const std::string& id, const double& deltaT);
