
#include "dnf_architecture.h"

std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitecture(const std::string& id, const double& deltaT)
{
	using namespace dnf_composer;
	auto simulation = std::make_shared<Simulation>(id, deltaT, 0, 0);

	element::ElementFactory factory;
	element::ElementSpatialDimensionParameters dim_params{ 90, 1.0 };

	// Action observation layer
	element::GaussStimulusParameters hand_position_gsp = { 6, 0, 0 };
	const auto hand_position_stimulus = factory.createElement(element::GAUSS_STIMULUS, { "hand position stimulus", dim_params }, { hand_position_gsp });
	simulation->addElement(hand_position_stimulus);

	const element::SigmoidFunction aol_af = { 0.0, 10.0 };
	element::NeuralFieldParameters aol_params = { 20, -5, aol_af };
	const auto aol = factory.createElement(element::NEURAL_FIELD, { "aol", dim_params }, { aol_params });
	simulation->addElement(aol);

	element::GaussKernelParameters aol_aol_k_params = { 3, 4.5 };
	const auto aol_aol_k = factory.createElement(element::GAUSS_KERNEL, { "aol -> aol", dim_params }, { aol_aol_k_params });
	simulation->addElement(aol_aol_k);

	const element::NormalNoiseParameters aol_nn_params = { 0.001 };
	const auto aol_nn = factory.createElement(element::NORMAL_NOISE, { "normal noise aol", dim_params }, aol_nn_params);
	simulation->addElement(aol_nn);

	simulation->createInteraction("aol", "output", "aol -> aol");
	simulation->createInteraction("aol -> aol", "output", "aol");
	simulation->createInteraction("normal noise aol", "output", "aol");
	simulation->createInteraction("hand position stimulus", "output", "aol");

	// Action simulation layer
	const element::SigmoidFunction asl_af = { 0.0, 10.0 };
	element::NeuralFieldParameters asl_params = { 20, -5, asl_af };
	const auto asl = factory.createElement(element::NEURAL_FIELD, { "asl", dim_params }, { asl_params });
	simulation->addElement(asl);

	element::LateralInteractionsParameters asl_asl_k_params = { 2, 19, 10, 30, -1.67 };
	const auto asl_asl_k = factory.createElement(element::LATERAL_INTERACTIONS, { "asl -> asl", dim_params }, { asl_asl_k_params });
	simulation->addElement(asl_asl_k);

	element::GaussKernelParameters aol_asl_k_params = { 3, -4.5 };
	const auto aol_asl_k = factory.createElement(element::GAUSS_KERNEL, { "aol -> asl", dim_params }, { aol_asl_k_params });
	simulation->addElement(aol_asl_k);

	const element::NormalNoiseParameters asl_nn_params = { 0.001 };
	const auto asl_nn = factory.createElement(element::NORMAL_NOISE, { "normal noise asl", dim_params }, asl_nn_params);
	simulation->addElement(asl_nn);

	simulation->createInteraction("asl", "output", "asl -> asl");
	simulation->createInteraction("asl -> asl", "output", "asl");
	simulation->createInteraction("normal noise asl", "output", "asl");

	simulation->createInteraction("aol", "output", "aol -> asl");
	simulation->createInteraction("aol -> asl", "output", "asl");

	// Object memory layer
	element::GaussStimulusParameters oml_gsp = { 3, 5, 30 };
	const auto oml_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS, { "object stimulus 1", dim_params }, { oml_gsp });
	simulation->addElement(oml_stimulus_1);

	oml_gsp = { 3, 5, 60 };
	const auto oml_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS, { "object stimulus 2", dim_params }, { oml_gsp });
	simulation->addElement(oml_stimulus_2);

	oml_gsp = { 3, 5, 0 };
	const auto oml_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS, { "object stimulus 3", dim_params }, { oml_gsp });
	simulation->addElement(oml_stimulus_3);

	element::SigmoidFunction oml_af = { 0.0, 10.0 };
	element::NeuralFieldParameters oml_params = { 20, -5, oml_af };
	const auto oml = factory.createElement(element::NEURAL_FIELD, { "oml", dim_params }, { oml_params });
	simulation->addElement(oml);

	element::GaussKernelParameters oml_oml_k_params = { 3, 5 };
	const auto oml_oml_k = factory.createElement(element::GAUSS_KERNEL, { "oml -> oml", dim_params }, { oml_oml_k_params });
	simulation->addElement(oml_oml_k);

	element::GaussKernelParameters oml_asl_k_params = { 3, 10 };
	const auto oml_asl_k = factory.createElement(element::GAUSS_KERNEL, { "oml -> asl", dim_params }, { oml_asl_k_params });
	simulation->addElement(oml_asl_k);

	element::NormalNoiseParameters oml_nn_params = { 0.001 };
	const auto oml_nn = factory.createElement(element::NORMAL_NOISE, { "normal noise oml", dim_params }, oml_nn_params);
	simulation->addElement(oml_nn);

	simulation->createInteraction("oml", "output", "oml -> asl");
	simulation->createInteraction("oml -> asl", "output", "asl");
	simulation->createInteraction("oml", "output", "oml -> oml");
	simulation->createInteraction("oml -> oml", "output", "oml");
	simulation->createInteraction("normal noise oml", "output", "oml");
	simulation->createInteraction("object stimulus 1", "output", "oml");
	simulation->createInteraction("object stimulus 2", "output", "oml");
	simulation->createInteraction("object stimulus 3", "output", "oml");

	// Action execution layer
	element::SigmoidFunction ael_af = { 0.0, 10.0 };
	element::NeuralFieldParameters ael_params = { 20, -5, ael_af };
	const auto ael = factory.createElement(element::NEURAL_FIELD, { "ael", dim_params }, { ael_params });
	simulation->addElement(ael);

	element::GaussKernelParameters asl_ael_k_params = { 3, 15 };
	const auto asl_ael_k = factory.createElement(element::GAUSS_KERNEL, { "asl -> ael", dim_params }, { asl_ael_k_params });
	simulation->addElement(asl_ael_k);

	element::LateralInteractionsParameters ael_ael_k_params = { 4, 6, 11, 12, -0.8 };
	const auto ael_ael_k = factory.createElement(element::LATERAL_INTERACTIONS, { "ael -> ael", dim_params }, { ael_ael_k_params });
	simulation->addElement(ael_ael_k);

	element::NormalNoiseParameters ael_nn_params = { 0.001 };
	const auto ael_nn = factory.createElement(element::NORMAL_NOISE, { "normal noise ael", dim_params }, ael_nn_params);
	simulation->addElement(ael_nn);

	simulation->createInteraction("ael", "output", "ael -> ael");
	simulation->createInteraction("ael -> ael", "output", "ael");
	simulation->createInteraction("normal noise ael", "output", "ael");
	simulation->createInteraction("asl", "output", "asl -> ael");
	simulation->createInteraction("asl -> ael", "output", "ael");

	return simulation;
}
