#pragma once

#include <elements/element_factory.h>

std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitecture(const std::string& id, const double& deltaT);