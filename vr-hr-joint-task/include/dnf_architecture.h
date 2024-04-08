#pragma once

#include <elements/element_factory.h>

enum class RobotArchitecture
{
	HAND_MOTION,
	ACTION_LIKELIHOOD,
};

std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitectureHandMotion(const std::string& id, const double& deltaT);

std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitectureActionLikelihood(const std::string& id, const double& deltaT);