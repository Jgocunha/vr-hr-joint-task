
#include "dnf_architecture.h"


std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitectureNeutral(const std::string& id, const double& deltaT)

{
	using namespace dnf_composer;
	auto simulation = std::make_shared<Simulation>(id, deltaT, 0, 0);

	element::ElementFactory factory;
	element::ElementDimensions dim_params{ 240, 0.5 };
	constexpr bool circularity = true;
	constexpr bool normalization = false;
	constexpr double tau = 100;
	constexpr double resting_level = -10;
	constexpr double x_shift = 0;
	constexpr double steepness = 5;
	constexpr double stimulus_sigma = 4;
	constexpr double stimulus_amplitude = 15;
	constexpr double noise_amplitude = 0.05;;

	//neural fields
const element::SigmoidFunction aol_af = { x_shift, steepness };
element::NeuralFieldParameters aol_params = { tau, resting_level, aol_af };
const auto aol = factory.createElement(element::NEURAL_FIELD,
	{ "aol", dim_params }, { aol_params });
simulation->addElement(aol);

const element::SigmoidFunction sosf_af = { x_shift, steepness };
element::NeuralFieldParameters sosf_params = { tau, resting_level, sosf_af };
const auto sosf = factory.createElement(element::NEURAL_FIELD,
	{ "sosf", dim_params }, { sosf_params });
simulation->addElement(sosf);

const element::SigmoidFunction loif_af = { x_shift, steepness };
element::NeuralFieldParameters loif_params = { tau, resting_level, loif_af };
const auto loif = factory.createElement(element::NEURAL_FIELD,
	{ "loif", dim_params }, { loif_params });
simulation->addElement(loif);

element::SigmoidFunction sof_af = { x_shift, steepness };
element::NeuralFieldParameters sof_params = { tau, resting_level, sof_af };
const auto sof = factory.createElement(element::NEURAL_FIELD,
	{ "sof", dim_params }, { sof_params });
simulation->addElement(sof);

element::SigmoidFunction lof_af = { x_shift, steepness };
element::NeuralFieldParameters lof_params = { tau, resting_level, lof_af };
const auto lof = factory.createElement(element::NEURAL_FIELD,
	{ "lof", dim_params }, { lof_params });
simulation->addElement(lof);

element::SigmoidFunction ael_af = { x_shift, steepness };
element::NeuralFieldParameters ael_params = { tau, resting_level, ael_af };
const auto ael = factory.createElement(element::NEURAL_FIELD,
	{ "ael", dim_params }, { ael_params });
simulation->addElement(ael);

//stimuli
element::GaussStimulusParameters hand_position_gsp = { stimulus_sigma, 0, 0, circularity, true };
const auto hand_position_stimulus = factory.createElement(element::GAUSS_STIMULUS,
	{ "hand position stimulus", dim_params }, { hand_position_gsp });
simulation->addElement(hand_position_stimulus);


element::GaussStimulusParameters sof_gsp = { stimulus_sigma, stimulus_amplitude, 15, circularity, true };
const auto sof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 4", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_1);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 45, circularity, true };
const auto sof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 3", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_2);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 135, circularity, true };
const auto sof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 2", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_3);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 165, circularity, true };
const auto sof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 1", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_4);

element::GaussStimulusParameters lof_gsp = { stimulus_sigma, stimulus_amplitude, 75, circularity, true };
const auto lof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 4", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_1);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 105, circularity, true };
const auto lof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 3", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_2);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 195, circularity, true };
const auto lof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 2", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_3);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 225, circularity, true };
const auto lof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 1", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_4);



//self excitations kernels
element::GaussKernelParameters aol_aol_k_params = { 20, 2, -0.01, circularity, true};
const auto aol_aol_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> aol", dim_params }, { aol_aol_k_params });
simulation->addElement(aol_aol_k);

element::MexicanHatKernelParameters sosf_sosf_k_params = { 18.92, 22.22, 50, 20, -0.23, circularity, true };
const auto sosf_sosf_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "sosf -> sosf", dim_params }, { sosf_sosf_k_params });
simulation->addElement(sosf_sosf_k);

element::MexicanHatKernelParameters loif_loif_k_params = { 20, 22, 100, 10, -0.01, circularity, true };
const auto loif_loif_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "loif -> loif", dim_params }, { loif_loif_k_params });
simulation->addElement(loif_loif_k);

element::GaussKernelParameters sof_sof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto sof_sof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sof", dim_params }, { sof_sof_k_params });
simulation->addElement(sof_sof_k);

element::GaussKernelParameters lof_lof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lof_lof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lof", dim_params }, { lof_lof_k_params });
simulation->addElement(lof_lof_k);

element::MexicanHatKernelParameters ael_ael_k_params = { 5.09, 7.85, 100, 30, -0.42, circularity, true };
const auto ael_ael_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "ael -> ael", dim_params }, { ael_ael_k_params });
simulation->addElement(ael_ael_k);

//interction kernels
element::GaussKernelParameters aol_sosf_k_params = { 5, -6.65, 0.0, circularity, true };
const auto aol_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> sosf", dim_params }, { aol_sosf_k_params });
simulation->addElement(aol_sosf_k);

element::GaussKernelParameters aol_loif_k_params = { 5, 14, 0.0, circularity, true };
const auto aol_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> loif", dim_params }, { aol_loif_k_params });
simulation->addElement(aol_loif_k);

element::GaussKernelParameters sof_sosf_k_params = { 5, 10.17, 0.0, circularity, true };
const auto sof_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sosf", dim_params }, { sof_sosf_k_params });
simulation->addElement(sof_sosf_k);

element::GaussKernelParameters lof_loif_k_params = { 5, 12, 0.0, circularity, true };
const auto lof_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> loif", dim_params }, { lof_loif_k_params });
simulation->addElement(lof_loif_k);


element::GaussKernelParameters sosf_ael_k_params = { 5, 42, 0.0, circularity, true };
const auto sosf_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sosf -> ael", dim_params }, { sosf_ael_k_params });
simulation->addElement(sosf_ael_k);

element::GaussKernelParameters loif_ael_k_params = { 5, 26, 0.0, circularity, true };
const auto loif_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "loif -> ael", dim_params }, { loif_ael_k_params });
simulation->addElement(loif_ael_k);

//noise

element::NormalNoiseParameters ael_nn_params = { 0.45 };
const auto ael_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise ael", dim_params }, ael_nn_params);
simulation->addElement(ael_nn);

	element::NormalNoiseParameters sosf_nn_params = { 0.32 };
const auto sosf_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise sosf", dim_params }, sosf_nn_params);
simulation->addElement(sosf_nn);

//interactions
//self-excitation
simulation->createInteraction("aol", "output", "aol -> aol");
simulation->createInteraction("aol -> aol", "output", "aol");

simulation->createInteraction("sosf", "output", "sosf -> sosf");
simulation->createInteraction("sosf -> sosf", "output", "sosf");

simulation->createInteraction("loif", "output", "loif -> loif");
simulation->createInteraction("loif -> loif", "output", "loif");

simulation->createInteraction("sof", "output", "sof -> sof");
simulation->createInteraction("sof -> sof", "output", "sof");

simulation->createInteraction("lof", "output", "lof -> lof");
simulation->createInteraction("lof -> lof", "output", "lof");

simulation->createInteraction("ael", "output", "ael -> ael");
simulation->createInteraction("ael -> ael", "output", "ael");

//stimuli'
simulation->createInteraction("hand position stimulus", "output", "aol");

simulation->createInteraction("short stimulus 1", "output", "sof");
simulation->createInteraction("short stimulus 2", "output", "sof");
simulation->createInteraction("short stimulus 3", "output", "sof");
simulation->createInteraction("short stimulus 4", "output", "sof");

simulation->createInteraction("long stimulus 1", "output", "lof");
simulation->createInteraction("long stimulus 2", "output", "lof");
simulation->createInteraction("long stimulus 3", "output", "lof");
simulation->createInteraction("long stimulus 4", "output", "lof");
//interactions

simulation->createInteraction("aol", "output", "aol -> sosf");
simulation->createInteraction("aol -> sosf", "output", "sosf");

simulation->createInteraction("aol", "output", "aol -> loif");
simulation->createInteraction("aol -> loif", "output", "loif");

simulation->createInteraction("sof", "output", "sof -> sosf");
simulation->createInteraction("sof -> sosf", "output", "sosf");

simulation->createInteraction("lof", "output", "lof -> loif");
simulation->createInteraction("lof -> loif", "output", "loif");

simulation->createInteraction("sosf", "output", "sosf -> ael");
simulation->createInteraction("sosf -> ael", "output", "ael");
simulation->createInteraction("loif", "output", "loif -> ael");
simulation->createInteraction("loif -> ael", "output", "ael");


//noise
simulation->createInteraction("normal noise ael", "output", "ael");
simulation->createInteraction("normal noise sosf", "output", "sosf");


return simulation;
}

std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitectureKnownBlue(const std::string& id, const double& deltaT)

{
	using namespace dnf_composer;
auto simulation = std::make_shared<Simulation>(id, deltaT, 0, 0);

element::ElementFactory factory;
element::ElementDimensions dim_params{ 240, 0.5 };
constexpr bool circularity = true;
constexpr bool normalization = false;
constexpr double tau = 100;
constexpr double resting_level = -10;
constexpr double x_shift = 0;
constexpr double steepness = 5;
constexpr double stimulus_sigma = 4;
constexpr double stimulus_amplitude = 15;
constexpr double noise_amplitude = 0.05;

//neural fields
const element::SigmoidFunction aol_af = { x_shift, steepness };
element::NeuralFieldParameters aol_params = { tau, resting_level, aol_af };
const auto aol = factory.createElement(element::NEURAL_FIELD,
	{ "aol", dim_params }, { aol_params });
simulation->addElement(aol);

const element::SigmoidFunction sosf_af = { x_shift, steepness };
element::NeuralFieldParameters sosf_params = { tau, resting_level, sosf_af };
const auto sosf = factory.createElement(element::NEURAL_FIELD,
	{ "sosf", dim_params }, { sosf_params });
simulation->addElement(sosf);

const element::SigmoidFunction loif_af = { x_shift, steepness };
element::NeuralFieldParameters loif_params = { tau, resting_level, loif_af };
const auto loif = factory.createElement(element::NEURAL_FIELD,
	{ "loif", dim_params }, { loif_params });
simulation->addElement(loif);

element::SigmoidFunction sof_af = { x_shift, steepness };
element::NeuralFieldParameters sof_params = { tau, resting_level, sof_af };
const auto sof = factory.createElement(element::NEURAL_FIELD,
	{ "sof", dim_params }, { sof_params });
simulation->addElement(sof);

element::SigmoidFunction lof_af = { x_shift, steepness };
element::NeuralFieldParameters lof_params = { tau, resting_level, lof_af };
const auto lof = factory.createElement(element::NEURAL_FIELD,
	{ "lof", dim_params }, { lof_params });
simulation->addElement(lof);

element::SigmoidFunction bof_af= { x_shift, steepness };
element::NeuralFieldParameters bof_params = { tau, resting_level, bof_af };
const auto bof = factory.createElement(element::NEURAL_FIELD,
	{ "bof", dim_params }, { bof_params });
simulation->addElement(bof);

element::SigmoidFunction yof_af= { x_shift, steepness };
element::NeuralFieldParameters yof_params = { tau, resting_level, yof_af };
const auto yof = factory.createElement(element::NEURAL_FIELD,
	{ "yof", dim_params }, { yof_params });
simulation->addElement(yof);

element::SigmoidFunction yhf_af= { x_shift, steepness };
element::NeuralFieldParameters yhf_params = { tau, resting_level, yhf_af };
const auto yhf = factory.createElement(element::NEURAL_FIELD,
	{ "yhf", dim_params }, { yhf_params });
simulation->addElement(yhf);

element::SigmoidFunction bhf_af= { x_shift, steepness };
element::NeuralFieldParameters bhf_params = { tau, resting_level, bhf_af };
const auto bhf = factory.createElement(element::NEURAL_FIELD,
	{ "bhf", dim_params }, { bhf_params });
simulation->addElement(bhf);

element::SigmoidFunction shf_af= { x_shift, steepness };
element::NeuralFieldParameters shf_params = { tau, resting_level, shf_af };
const auto shf = factory.createElement(element::NEURAL_FIELD,
	{ "shf", dim_params }, { shf_params });
simulation->addElement(shf);

element::SigmoidFunction lhf_af= { x_shift, steepness };
element::NeuralFieldParameters lhf_params = { tau, resting_level, lhf_af };
const auto lhf = factory.createElement(element::NEURAL_FIELD,
	{ "lhf", dim_params }, { lhf_params });
simulation->addElement(lhf);

element::SigmoidFunction ael_af = { x_shift, steepness };
element::NeuralFieldParameters ael_params = { tau, resting_level, ael_af };
const auto ael = factory.createElement(element::NEURAL_FIELD,
	{ "ael", dim_params }, { ael_params });
simulation->addElement(ael);

//stimuli
element::GaussStimulusParameters hand_position_gsp = { stimulus_sigma, 0, 0, circularity, true };
const auto hand_position_stimulus = factory.createElement(element::GAUSS_STIMULUS,
	{ "hand position stimulus", dim_params }, { hand_position_gsp });
simulation->addElement(hand_position_stimulus);


element::GaussStimulusParameters sof_gsp = { stimulus_sigma, stimulus_amplitude, 15, circularity, true };
const auto sof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 4", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_1);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 45, circularity, true };
const auto sof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 3", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_2);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 135, circularity, true };
const auto sof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 2", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_3);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 165, circularity, true };
const auto sof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 1", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_4);

element::GaussStimulusParameters lof_gsp = { stimulus_sigma, stimulus_amplitude, 75, circularity, true };
const auto lof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 4", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_1);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 105, circularity, true };
const auto lof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 3", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_2);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 195, circularity, true };
const auto lof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 2", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_3);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 225, circularity, true };
const auto lof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 1", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_4);



//self excitations kernels
element::GaussKernelParameters aol_aol_k_params = { 20, 2, -0.01, circularity, true};
const auto aol_aol_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> aol", dim_params }, { aol_aol_k_params });
simulation->addElement(aol_aol_k);

element::MexicanHatKernelParameters sosf_sosf_k_params = { 18.92, 22.22, 50, 20, -0.23, circularity, true };
const auto sosf_sosf_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "sosf -> sosf", dim_params }, { sosf_sosf_k_params });
simulation->addElement(sosf_sosf_k);

element::MexicanHatKernelParameters loif_loif_k_params = { 20, 22, 100, 10, -0.01, circularity, true };
const auto loif_loif_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "loif -> loif", dim_params }, { loif_loif_k_params });
simulation->addElement(loif_loif_k);

element::GaussKernelParameters sof_sof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto sof_sof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sof", dim_params }, { sof_sof_k_params });
simulation->addElement(sof_sof_k);
element::GaussKernelParameters lof_lof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lof_lof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lof", dim_params }, { lof_lof_k_params });
simulation->addElement(lof_lof_k);
element::GaussKernelParameters bof_bof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto bof_bof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> bof", dim_params }, { bof_bof_k_params });
simulation->addElement(bof_bof_k);
element::GaussKernelParameters yof_yof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto yof_yof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yof", dim_params }, { yof_yof_k_params });
simulation->addElement(yof_yof_k);

element::GaussKernelParameters yhf_yhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto yhf_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> yhf", dim_params }, { yhf_yhf_k_params });
simulation->addElement(yhf_yhf_k);
element::GaussKernelParameters bhf_bhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto bhf_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> bhf", dim_params }, { bhf_bhf_k_params });	
simulation->addElement(bhf_bhf_k);
element::GaussKernelParameters shf_shf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto shf_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "shf -> shf", dim_params }, { shf_shf_k_params });	
simulation->addElement(shf_shf_k);
element::GaussKernelParameters lhf_lhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lhf_lhf_k = factory.createElement(element::GAUSS_KERNEL,	
	{ "lhf -> lhf", dim_params }, { lhf_lhf_k_params });	
simulation->addElement(lhf_lhf_k);

element::MexicanHatKernelParameters ael_ael_k_params = { 5.09, 7.85, 100, 30, -0.42, circularity, true };
const auto ael_ael_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "ael -> ael", dim_params }, { ael_ael_k_params });
simulation->addElement(ael_ael_k);

//interction kernels
element::GaussKernelParameters aol_sosf_k_params = { 5, -6.65, 0.0, circularity, true };
const auto aol_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> sosf", dim_params }, { aol_sosf_k_params });
simulation->addElement(aol_sosf_k);

element::GaussKernelParameters aol_loif_k_params = { 5, 12, 0.0, circularity, true };
const auto aol_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> loif", dim_params }, { aol_loif_k_params });
simulation->addElement(aol_loif_k);

element::GaussKernelParameters sof_sosf_k_params = { 5, 10.17, 0.0, circularity, true };
const auto sof_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sosf", dim_params }, { sof_sosf_k_params });
simulation->addElement(sof_sosf_k);

element::GaussKernelParameters lof_loif_k_params = { 5, 12, 0.0, circularity, true };
const auto lof_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> loif", dim_params }, { lof_loif_k_params });
simulation->addElement(lof_loif_k);


element::GaussKernelParameters sosf_ael_k_params = { 5, 42, 0.0, circularity, true };
const auto sosf_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sosf -> ael", dim_params }, { sosf_ael_k_params });
simulation->addElement(sosf_ael_k);

element::GaussKernelParameters loif_ael_k_params = { 5, 26, 0.0, circularity, true };
const auto loif_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "loif -> ael", dim_params }, { loif_ael_k_params });
simulation->addElement(loif_ael_k);


element::GaussKernelParameters yof_yhf_k_params = { 4, 25, 0.0, circularity, true };
const auto yof_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yhf", dim_params }, { yof_yhf_k_params });
simulation->addElement(yof_yhf_k);
element::GaussKernelParameters bof_bhf_k_params = { 4, 25, 0.0, circularity, true };	
const auto bof_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> bhf", dim_params }, { bof_bhf_k_params });
simulation->addElement(bof_bhf_k);
element::GaussKernelParameters sof_shf_k_params = { 4, 25, 0.0, circularity, true };	
const auto sof_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> shf", dim_params }, { sof_shf_k_params });
simulation->addElement(sof_shf_k);
element::GaussKernelParameters lof_lhf_k_params = { 4, 25, 0.0, circularity, true };	
const auto lof_lhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lhf", dim_params }, { lof_lhf_k_params });	
simulation->addElement(lof_lhf_k);


element::GaussKernelParameters yhf_sosf_k_params = { 5, 10, -0.01, circularity, true };
const auto yhf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> sosf", dim_params }, { yhf_sosf_k_params });
simulation->addElement(yhf_sosf_k);

element::GaussKernelParameters yhf_loif_k_params = { 5, 2, 0, circularity, true };
const auto yhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> loif", dim_params }, { yhf_loif_k_params });
simulation->addElement(yhf_loif_k);

element::GaussKernelParameters bhf_sosf_k_params = { 5, 10, -0.01, circularity, true };
const auto bhf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> sosf", dim_params }, { bhf_sosf_k_params });
simulation->addElement(bhf_sosf_k);

element::GaussKernelParameters bhf_loif_k_params = { 5, 2, 0, circularity, true };
const auto bhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> loif", dim_params }, { bhf_loif_k_params });
simulation->addElement(bhf_loif_k);

element::GaussKernelParameters shf_sosf_k_params = { 5, 11, -0.01, circularity, true };
const auto shf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "shf -> sosf", dim_params }, { shf_sosf_k_params });
simulation->addElement(shf_sosf_k);

element::GaussKernelParameters lhf_loif_k_params = { 5, 3, -0.01, circularity, true };
const auto lhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lhf -> loif", dim_params }, { lhf_loif_k_params });
simulation->addElement(lhf_loif_k);


//noise

element::NormalNoiseParameters ael_nn_params = { 0.45 };
const auto ael_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise ael", dim_params }, ael_nn_params);
simulation->addElement(ael_nn);

	element::NormalNoiseParameters sosf_nn_params = { 0.32 };
const auto sosf_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise sosf", dim_params }, sosf_nn_params);
simulation->addElement(sosf_nn);

//interactions
//self-excitation
simulation->createInteraction("aol", "output", "aol -> aol");
simulation->createInteraction("aol -> aol", "output", "aol");

simulation->createInteraction("sosf", "output", "sosf -> sosf");
simulation->createInteraction("sosf -> sosf", "output", "sosf");

simulation->createInteraction("loif", "output", "loif -> loif");
simulation->createInteraction("loif -> loif", "output", "loif");

simulation->createInteraction("sof", "output", "sof -> sof");
simulation->createInteraction("sof -> sof", "output", "sof");
simulation->createInteraction("lof", "output", "lof -> lof");
simulation->createInteraction("lof -> lof", "output", "lof");
simulation->createInteraction("bof", "output", "bof -> bof");
simulation->createInteraction("bof -> bof", "output", "bof");
simulation->createInteraction("yof", "output", "yof -> yof");
simulation->createInteraction("yof -> yof", "output", "yof");

simulation->createInteraction("yhf", "output", "yhf -> yhf");
simulation->createInteraction("yhf -> yhf", "output", "yhf");
simulation->createInteraction("bhf", "output", "bhf -> bhf");
simulation->createInteraction("bhf -> bhf", "output", "bhf");
simulation->createInteraction("shf", "output", "shf -> shf");
simulation->createInteraction("shf -> shf", "output", "shf");
simulation->createInteraction("lhf", "output", "lhf -> lhf");
simulation->createInteraction("lhf -> lhf", "output", "lhf");


simulation->createInteraction("ael", "output", "ael -> ael");
simulation->createInteraction("ael -> ael", "output", "ael");

//stimuli'
simulation->createInteraction("hand position stimulus", "output", "aol");

simulation->createInteraction("short stimulus 1", "output", "sof");
simulation->createInteraction("short stimulus 2", "output", "sof");
simulation->createInteraction("short stimulus 3", "output", "sof");
simulation->createInteraction("short stimulus 4", "output", "sof");

simulation->createInteraction("long stimulus 1", "output", "lof");
simulation->createInteraction("long stimulus 2", "output", "lof");
simulation->createInteraction("long stimulus 3", "output", "lof");
simulation->createInteraction("long stimulus 4", "output", "lof");


simulation->createInteraction("long stimulus 3", "output", "bof");
simulation->createInteraction("long stimulus 4", "output", "bof");
simulation->createInteraction("short stimulus 3", "output", "bof");
simulation->createInteraction("short stimulus 4", "output", "bof");

simulation->createInteraction("long stimulus 1", "output", "yof");
simulation->createInteraction("long stimulus 2", "output", "yof");
simulation->createInteraction("short stimulus 1", "output", "yof");
simulation->createInteraction("short stimulus 2", "output", "yof");


//interactions

simulation->createInteraction("aol", "output", "aol -> sosf");
simulation->createInteraction("aol -> sosf", "output", "sosf");

simulation->createInteraction("aol", "output", "aol -> loif");
simulation->createInteraction("aol -> loif", "output", "loif");

simulation->createInteraction("sof", "output", "sof -> sosf");
simulation->createInteraction("sof -> sosf", "output", "sosf");

simulation->createInteraction("lof", "output", "lof -> loif");
simulation->createInteraction("lof -> loif", "output", "loif");

simulation->createInteraction("yof", "output", "yof -> yhf");
simulation->createInteraction("yof -> yhf", "output", "yhf");
simulation->createInteraction("bof", "output", "bof -> bhf");
simulation->createInteraction("bof -> bhf", "output", "bhf");
simulation->createInteraction("sof", "output", "sof -> shf");
simulation->createInteraction("sof -> shf", "output", "shf");
simulation->createInteraction("lof", "output", "lof -> lhf");
simulation->createInteraction("lof -> lhf", "output", "lhf");

//simulation->createInteraction("yhf", "output", "yhf -> sosf");
//simulation->createInteraction("yhf -> sosf", "output", "sosf");
//simulation->createInteraction("yhf", "output", "yhf -> loif");
//simulation->createInteraction("yhf -> loif", "output", "loif");

simulation->createInteraction("bhf", "output", "bhf -> sosf");
simulation->createInteraction("bhf -> sosf", "output", "sosf");
simulation->createInteraction("bhf", "output", "bhf -> loif");
simulation->createInteraction("bhf -> loif", "output", "loif");

//simulation->createInteraction("shf", "output", "shf -> sosf");
//simulation->createInteraction("shf -> sosf", "output", "sosf");

//simulation->createInteraction("lhf", "output", "lhf -> loif");
//simulation->createInteraction("lhf -> loif", "output", "loif");


simulation->createInteraction("sosf", "output", "sosf -> ael");
simulation->createInteraction("sosf -> ael", "output", "ael");
simulation->createInteraction("loif", "output", "loif -> ael");
simulation->createInteraction("loif -> ael", "output", "ael");


//noise
simulation->createInteraction("normal noise ael", "output", "ael");
simulation->createInteraction("normal noise sosf", "output", "sosf");


return simulation;
}

std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitectureKnownYellow(const std::string& id, const double& deltaT)

{
	using namespace dnf_composer;
auto simulation = std::make_shared<Simulation>(id, deltaT, 0, 0);

element::ElementFactory factory;
element::ElementDimensions dim_params{ 240, 0.5 };
constexpr bool circularity = true;
constexpr bool normalization = false;
constexpr double tau = 100;
constexpr double resting_level = -10;
constexpr double x_shift = 0;
constexpr double steepness = 5;
constexpr double stimulus_sigma = 4;
constexpr double stimulus_amplitude = 15;
constexpr double noise_amplitude = 0.05;

//neural fields
const element::SigmoidFunction aol_af = { x_shift, steepness };
element::NeuralFieldParameters aol_params = { tau, resting_level, aol_af };
const auto aol = factory.createElement(element::NEURAL_FIELD,
	{ "aol", dim_params }, { aol_params });
simulation->addElement(aol);

const element::SigmoidFunction sosf_af = { x_shift, steepness };
element::NeuralFieldParameters sosf_params = { tau, resting_level, sosf_af };
const auto sosf = factory.createElement(element::NEURAL_FIELD,
	{ "sosf", dim_params }, { sosf_params });
simulation->addElement(sosf);

const element::SigmoidFunction loif_af = { x_shift, steepness };
element::NeuralFieldParameters loif_params = { tau, resting_level, loif_af };
const auto loif = factory.createElement(element::NEURAL_FIELD,
	{ "loif", dim_params }, { loif_params });
simulation->addElement(loif);

element::SigmoidFunction sof_af = { x_shift, steepness };
element::NeuralFieldParameters sof_params = { tau, resting_level, sof_af };
const auto sof = factory.createElement(element::NEURAL_FIELD,
	{ "sof", dim_params }, { sof_params });
simulation->addElement(sof);

element::SigmoidFunction lof_af = { x_shift, steepness };
element::NeuralFieldParameters lof_params = { tau, resting_level, lof_af };
const auto lof = factory.createElement(element::NEURAL_FIELD,
	{ "lof", dim_params }, { lof_params });
simulation->addElement(lof);

element::SigmoidFunction bof_af= { x_shift, steepness };
element::NeuralFieldParameters bof_params = { tau, resting_level, bof_af };
const auto bof = factory.createElement(element::NEURAL_FIELD,
	{ "bof", dim_params }, { bof_params });
simulation->addElement(bof);

element::SigmoidFunction yof_af= { x_shift, steepness };
element::NeuralFieldParameters yof_params = { tau, resting_level, yof_af };
const auto yof = factory.createElement(element::NEURAL_FIELD,
	{ "yof", dim_params }, { yof_params });
simulation->addElement(yof);

element::SigmoidFunction yhf_af= { x_shift, steepness };
element::NeuralFieldParameters yhf_params = { tau, resting_level, yhf_af };
const auto yhf = factory.createElement(element::NEURAL_FIELD,
	{ "yhf", dim_params }, { yhf_params });
simulation->addElement(yhf);

element::SigmoidFunction bhf_af= { x_shift, steepness };
element::NeuralFieldParameters bhf_params = { tau, resting_level, bhf_af };
const auto bhf = factory.createElement(element::NEURAL_FIELD,
	{ "bhf", dim_params }, { bhf_params });
simulation->addElement(bhf);

element::SigmoidFunction shf_af= { x_shift, steepness };
element::NeuralFieldParameters shf_params = { tau, resting_level, shf_af };
const auto shf = factory.createElement(element::NEURAL_FIELD,
	{ "shf", dim_params }, { shf_params });
simulation->addElement(shf);

element::SigmoidFunction lhf_af= { x_shift, steepness };
element::NeuralFieldParameters lhf_params = { tau, resting_level, lhf_af };
const auto lhf = factory.createElement(element::NEURAL_FIELD,
	{ "lhf", dim_params }, { lhf_params });
simulation->addElement(lhf);

element::SigmoidFunction ael_af = { x_shift, steepness };
element::NeuralFieldParameters ael_params = { tau, resting_level, ael_af };
const auto ael = factory.createElement(element::NEURAL_FIELD,
	{ "ael", dim_params }, { ael_params });
simulation->addElement(ael);

//stimuli
element::GaussStimulusParameters hand_position_gsp = { stimulus_sigma, 0, 0, circularity, true };
const auto hand_position_stimulus = factory.createElement(element::GAUSS_STIMULUS,
	{ "hand position stimulus", dim_params }, { hand_position_gsp });
simulation->addElement(hand_position_stimulus);


element::GaussStimulusParameters sof_gsp = { stimulus_sigma, stimulus_amplitude, 15, circularity, true };
const auto sof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 4", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_1);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 45, circularity, true };
const auto sof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 3", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_2);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 135, circularity, true };
const auto sof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 2", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_3);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 165, circularity, true };
const auto sof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 1", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_4);

element::GaussStimulusParameters lof_gsp = { stimulus_sigma, stimulus_amplitude, 75, circularity, true };
const auto lof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 4", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_1);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 105, circularity, true };
const auto lof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 3", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_2);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 195, circularity, true };
const auto lof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 2", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_3);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 225, circularity, true };
const auto lof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 1", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_4);



//self excitations kernels
element::GaussKernelParameters aol_aol_k_params = { 20, 2, -0.01, circularity, true};
const auto aol_aol_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> aol", dim_params }, { aol_aol_k_params });
simulation->addElement(aol_aol_k);

element::MexicanHatKernelParameters sosf_sosf_k_params = { 18.92, 22.22, 50, 20, -0.23, circularity, true };
const auto sosf_sosf_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "sosf -> sosf", dim_params }, { sosf_sosf_k_params });
simulation->addElement(sosf_sosf_k);

element::MexicanHatKernelParameters loif_loif_k_params = { 20, 22, 100, 10, -0.01, circularity, true };
const auto loif_loif_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "loif -> loif", dim_params }, { loif_loif_k_params });
simulation->addElement(loif_loif_k);

element::GaussKernelParameters sof_sof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto sof_sof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sof", dim_params }, { sof_sof_k_params });
simulation->addElement(sof_sof_k);
element::GaussKernelParameters lof_lof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lof_lof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lof", dim_params }, { lof_lof_k_params });
simulation->addElement(lof_lof_k);
element::GaussKernelParameters bof_bof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto bof_bof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> bof", dim_params }, { bof_bof_k_params });
simulation->addElement(bof_bof_k);
element::GaussKernelParameters yof_yof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto yof_yof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yof", dim_params }, { yof_yof_k_params });
simulation->addElement(yof_yof_k);

element::GaussKernelParameters yhf_yhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto yhf_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> yhf", dim_params }, { yhf_yhf_k_params });
simulation->addElement(yhf_yhf_k);
element::GaussKernelParameters bhf_bhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto bhf_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> bhf", dim_params }, { bhf_bhf_k_params });	
simulation->addElement(bhf_bhf_k);
element::GaussKernelParameters shf_shf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto shf_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "shf -> shf", dim_params }, { shf_shf_k_params });	
simulation->addElement(shf_shf_k);
element::GaussKernelParameters lhf_lhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lhf_lhf_k = factory.createElement(element::GAUSS_KERNEL,	
	{ "lhf -> lhf", dim_params }, { lhf_lhf_k_params });	
simulation->addElement(lhf_lhf_k);

element::MexicanHatKernelParameters ael_ael_k_params = { 5.09, 7.85, 100, 30, -0.42, circularity, true };
const auto ael_ael_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "ael -> ael", dim_params }, { ael_ael_k_params });
simulation->addElement(ael_ael_k);

//interction kernels
element::GaussKernelParameters aol_sosf_k_params = { 5, -6.65, 0.0, circularity, true };
const auto aol_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> sosf", dim_params }, { aol_sosf_k_params });
simulation->addElement(aol_sosf_k);

element::GaussKernelParameters aol_loif_k_params = { 5, 12, 0.0, circularity, true };
const auto aol_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> loif", dim_params }, { aol_loif_k_params });
simulation->addElement(aol_loif_k);

element::GaussKernelParameters sof_sosf_k_params = { 5, 10.17, 0.0, circularity, true };
const auto sof_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sosf", dim_params }, { sof_sosf_k_params });
simulation->addElement(sof_sosf_k);

element::GaussKernelParameters lof_loif_k_params = { 5, 12, 0.0, circularity, true };
const auto lof_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> loif", dim_params }, { lof_loif_k_params });
simulation->addElement(lof_loif_k);


element::GaussKernelParameters sosf_ael_k_params = { 5, 42, 0.0, circularity, true };
const auto sosf_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sosf -> ael", dim_params }, { sosf_ael_k_params });
simulation->addElement(sosf_ael_k);

element::GaussKernelParameters loif_ael_k_params = { 5, 26, 0.0, circularity, true };
const auto loif_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "loif -> ael", dim_params }, { loif_ael_k_params });
simulation->addElement(loif_ael_k);


element::GaussKernelParameters yof_yhf_k_params = { 4, 25, 0.0, circularity, true };
const auto yof_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yhf", dim_params }, { yof_yhf_k_params });
simulation->addElement(yof_yhf_k);
element::GaussKernelParameters bof_bhf_k_params = { 4, 25, 0.0, circularity, true };	
const auto bof_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> bhf", dim_params }, { bof_bhf_k_params });
simulation->addElement(bof_bhf_k);
element::GaussKernelParameters sof_shf_k_params = { 4, 25, 0.0, circularity, true };	
const auto sof_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> shf", dim_params }, { sof_shf_k_params });
simulation->addElement(sof_shf_k);
element::GaussKernelParameters lof_lhf_k_params = { 4, 25, 0.0, circularity, true };	
const auto lof_lhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lhf", dim_params }, { lof_lhf_k_params });	
simulation->addElement(lof_lhf_k);


element::GaussKernelParameters yhf_sosf_k_params = { 5, 10, -0.01, circularity, true };
const auto yhf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> sosf", dim_params }, { yhf_sosf_k_params });
simulation->addElement(yhf_sosf_k);

element::GaussKernelParameters yhf_loif_k_params = { 5, 2, 0, circularity, true };
const auto yhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> loif", dim_params }, { yhf_loif_k_params });
simulation->addElement(yhf_loif_k);

element::GaussKernelParameters bhf_sosf_k_params = { 5, 10, -0.01, circularity, true };
const auto bhf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> sosf", dim_params }, { bhf_sosf_k_params });
simulation->addElement(bhf_sosf_k);

element::GaussKernelParameters bhf_loif_k_params = { 5, 2, 0, circularity, true };
const auto bhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> loif", dim_params }, { bhf_loif_k_params });
simulation->addElement(bhf_loif_k);

element::GaussKernelParameters shf_sosf_k_params = { 5, 11, -0.01, circularity, true };
const auto shf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "shf -> sosf", dim_params }, { shf_sosf_k_params });
simulation->addElement(shf_sosf_k);

element::GaussKernelParameters lhf_loif_k_params = { 5, 3, -0.01, circularity, true };
const auto lhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lhf -> loif", dim_params }, { lhf_loif_k_params });
simulation->addElement(lhf_loif_k);


//noise

element::NormalNoiseParameters ael_nn_params = { 0.45 };
const auto ael_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise ael", dim_params }, ael_nn_params);
simulation->addElement(ael_nn);

	element::NormalNoiseParameters sosf_nn_params = { 0.32 };
const auto sosf_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise sosf", dim_params }, sosf_nn_params);
simulation->addElement(sosf_nn);

//interactions
//self-excitation
simulation->createInteraction("aol", "output", "aol -> aol");
simulation->createInteraction("aol -> aol", "output", "aol");

simulation->createInteraction("sosf", "output", "sosf -> sosf");
simulation->createInteraction("sosf -> sosf", "output", "sosf");

simulation->createInteraction("loif", "output", "loif -> loif");
simulation->createInteraction("loif -> loif", "output", "loif");

simulation->createInteraction("sof", "output", "sof -> sof");
simulation->createInteraction("sof -> sof", "output", "sof");
simulation->createInteraction("lof", "output", "lof -> lof");
simulation->createInteraction("lof -> lof", "output", "lof");
simulation->createInteraction("bof", "output", "bof -> bof");
simulation->createInteraction("bof -> bof", "output", "bof");
simulation->createInteraction("yof", "output", "yof -> yof");
simulation->createInteraction("yof -> yof", "output", "yof");

simulation->createInteraction("yhf", "output", "yhf -> yhf");
simulation->createInteraction("yhf -> yhf", "output", "yhf");
simulation->createInteraction("bhf", "output", "bhf -> bhf");
simulation->createInteraction("bhf -> bhf", "output", "bhf");
simulation->createInteraction("shf", "output", "shf -> shf");
simulation->createInteraction("shf -> shf", "output", "shf");
simulation->createInteraction("lhf", "output", "lhf -> lhf");
simulation->createInteraction("lhf -> lhf", "output", "lhf");


simulation->createInteraction("ael", "output", "ael -> ael");
simulation->createInteraction("ael -> ael", "output", "ael");

//stimuli'
simulation->createInteraction("hand position stimulus", "output", "aol");

simulation->createInteraction("short stimulus 1", "output", "sof");
simulation->createInteraction("short stimulus 2", "output", "sof");
simulation->createInteraction("short stimulus 3", "output", "sof");
simulation->createInteraction("short stimulus 4", "output", "sof");

simulation->createInteraction("long stimulus 1", "output", "lof");
simulation->createInteraction("long stimulus 2", "output", "lof");
simulation->createInteraction("long stimulus 3", "output", "lof");
simulation->createInteraction("long stimulus 4", "output", "lof");


simulation->createInteraction("long stimulus 3", "output", "bof");
simulation->createInteraction("long stimulus 4", "output", "bof");
simulation->createInteraction("short stimulus 3", "output", "bof");
simulation->createInteraction("short stimulus 4", "output", "bof");

simulation->createInteraction("long stimulus 1", "output", "yof");
simulation->createInteraction("long stimulus 2", "output", "yof");
simulation->createInteraction("short stimulus 1", "output", "yof");
simulation->createInteraction("short stimulus 2", "output", "yof");


//interactions

simulation->createInteraction("aol", "output", "aol -> sosf");
simulation->createInteraction("aol -> sosf", "output", "sosf");

simulation->createInteraction("aol", "output", "aol -> loif");
simulation->createInteraction("aol -> loif", "output", "loif");

simulation->createInteraction("sof", "output", "sof -> sosf");
simulation->createInteraction("sof -> sosf", "output", "sosf");

simulation->createInteraction("lof", "output", "lof -> loif");
simulation->createInteraction("lof -> loif", "output", "loif");

simulation->createInteraction("yof", "output", "yof -> yhf");
simulation->createInteraction("yof -> yhf", "output", "yhf");
simulation->createInteraction("bof", "output", "bof -> bhf");
simulation->createInteraction("bof -> bhf", "output", "bhf");
simulation->createInteraction("sof", "output", "sof -> shf");
simulation->createInteraction("sof -> shf", "output", "shf");
simulation->createInteraction("lof", "output", "lof -> lhf");
simulation->createInteraction("lof -> lhf", "output", "lhf");

simulation->createInteraction("yhf", "output", "yhf -> sosf");
simulation->createInteraction("yhf -> sosf", "output", "sosf");
simulation->createInteraction("yhf", "output", "yhf -> loif");
simulation->createInteraction("yhf -> loif", "output", "loif");

//simulation->createInteraction("bhf", "output", "bhf -> sosf");
//simulation->createInteraction("bhf -> sosf", "output", "sosf");
//simulation->createInteraction("bhf", "output", "bhf -> loif");
//simulation->createInteraction("bhf -> loif", "output", "loif");

//simulation->createInteraction("shf", "output", "shf -> sosf");
//simulation->createInteraction("shf -> sosf", "output", "sosf");

//simulation->createInteraction("lhf", "output", "lhf -> loif");
//simulation->createInteraction("lhf -> loif", "output", "loif");


simulation->createInteraction("sosf", "output", "sosf -> ael");
simulation->createInteraction("sosf -> ael", "output", "ael");
simulation->createInteraction("loif", "output", "loif -> ael");
simulation->createInteraction("loif -> ael", "output", "ael");


//noise
simulation->createInteraction("normal noise ael", "output", "ael");
simulation->createInteraction("normal noise sosf", "output", "sosf");


return simulation;
}

std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitectureKnownLong(const std::string& id, const double& deltaT)

{
using namespace dnf_composer;
auto simulation = std::make_shared<Simulation>(id, deltaT, 0, 0);

element::ElementFactory factory;
element::ElementDimensions dim_params{ 240, 0.5 };
constexpr bool circularity = true;
constexpr bool normalization = false;
constexpr double tau = 100;
constexpr double resting_level = -10;
constexpr double x_shift = 0;
constexpr double steepness = 5;
constexpr double stimulus_sigma = 4;
constexpr double stimulus_amplitude = 15;
constexpr double noise_amplitude = 0.05;

//neural fields
const element::SigmoidFunction aol_af = { x_shift, steepness };
element::NeuralFieldParameters aol_params = { tau, resting_level, aol_af };
const auto aol = factory.createElement(element::NEURAL_FIELD,
	{ "aol", dim_params }, { aol_params });
simulation->addElement(aol);

const element::SigmoidFunction sosf_af = { x_shift, steepness };
element::NeuralFieldParameters sosf_params = { tau, resting_level, sosf_af };
const auto sosf = factory.createElement(element::NEURAL_FIELD,
	{ "sosf", dim_params }, { sosf_params });
simulation->addElement(sosf);

const element::SigmoidFunction loif_af = { x_shift, steepness };
element::NeuralFieldParameters loif_params = { tau, resting_level, loif_af };
const auto loif = factory.createElement(element::NEURAL_FIELD,
	{ "loif", dim_params }, { loif_params });
simulation->addElement(loif);

element::SigmoidFunction sof_af = { x_shift, steepness };
element::NeuralFieldParameters sof_params = { tau, resting_level, sof_af };
const auto sof = factory.createElement(element::NEURAL_FIELD,
	{ "sof", dim_params }, { sof_params });
simulation->addElement(sof);

element::SigmoidFunction lof_af = { x_shift, steepness };
element::NeuralFieldParameters lof_params = { tau, resting_level, lof_af };
const auto lof = factory.createElement(element::NEURAL_FIELD,
	{ "lof", dim_params }, { lof_params });
simulation->addElement(lof);

element::SigmoidFunction bof_af= { x_shift, steepness };
element::NeuralFieldParameters bof_params = { tau, resting_level, bof_af };
const auto bof = factory.createElement(element::NEURAL_FIELD,
	{ "bof", dim_params }, { bof_params });
simulation->addElement(bof);

element::SigmoidFunction yof_af= { x_shift, steepness };
element::NeuralFieldParameters yof_params = { tau, resting_level, yof_af };
const auto yof = factory.createElement(element::NEURAL_FIELD,
	{ "yof", dim_params }, { yof_params });
simulation->addElement(yof);

element::SigmoidFunction yhf_af= { x_shift, steepness };
element::NeuralFieldParameters yhf_params = { tau, resting_level, yhf_af };
const auto yhf = factory.createElement(element::NEURAL_FIELD,
	{ "yhf", dim_params }, { yhf_params });
simulation->addElement(yhf);

element::SigmoidFunction bhf_af= { x_shift, steepness };
element::NeuralFieldParameters bhf_params = { tau, resting_level, bhf_af };
const auto bhf = factory.createElement(element::NEURAL_FIELD,
	{ "bhf", dim_params }, { bhf_params });
simulation->addElement(bhf);

element::SigmoidFunction shf_af= { x_shift, steepness };
element::NeuralFieldParameters shf_params = { tau, resting_level, shf_af };
const auto shf = factory.createElement(element::NEURAL_FIELD,
	{ "shf", dim_params }, { shf_params });
simulation->addElement(shf);

element::SigmoidFunction lhf_af= { x_shift, steepness };
element::NeuralFieldParameters lhf_params = { tau, resting_level, lhf_af };
const auto lhf = factory.createElement(element::NEURAL_FIELD,
	{ "lhf", dim_params }, { lhf_params });
simulation->addElement(lhf);

element::SigmoidFunction ael_af = { x_shift, steepness };
element::NeuralFieldParameters ael_params = { tau, resting_level, ael_af };
const auto ael = factory.createElement(element::NEURAL_FIELD,
	{ "ael", dim_params }, { ael_params });
simulation->addElement(ael);

//stimuli
element::GaussStimulusParameters hand_position_gsp = { stimulus_sigma, 0, 0, circularity, true };
const auto hand_position_stimulus = factory.createElement(element::GAUSS_STIMULUS,
	{ "hand position stimulus", dim_params }, { hand_position_gsp });
simulation->addElement(hand_position_stimulus);


element::GaussStimulusParameters sof_gsp = { stimulus_sigma, stimulus_amplitude, 15, circularity, true };
const auto sof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 4", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_1);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 45, circularity, true };
const auto sof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 3", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_2);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 135, circularity, true };
const auto sof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 2", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_3);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 165, circularity, true };
const auto sof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 1", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_4);

element::GaussStimulusParameters lof_gsp = { stimulus_sigma, stimulus_amplitude, 75, circularity, true };
const auto lof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 4", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_1);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 105, circularity, true };
const auto lof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 3", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_2);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 195, circularity, true };
const auto lof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 2", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_3);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 225, circularity, true };
const auto lof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 1", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_4);



//self excitations kernels
element::GaussKernelParameters aol_aol_k_params = { 20, 2, -0.01, circularity, true};
const auto aol_aol_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> aol", dim_params }, { aol_aol_k_params });
simulation->addElement(aol_aol_k);

element::MexicanHatKernelParameters sosf_sosf_k_params = { 18.92, 22.22, 50, 20, -0.23, circularity, true };
const auto sosf_sosf_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "sosf -> sosf", dim_params }, { sosf_sosf_k_params });
simulation->addElement(sosf_sosf_k);

element::MexicanHatKernelParameters loif_loif_k_params = { 20, 22, 100, 10, -0.01, circularity, true };
const auto loif_loif_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "loif -> loif", dim_params }, { loif_loif_k_params });
simulation->addElement(loif_loif_k);

element::GaussKernelParameters sof_sof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto sof_sof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sof", dim_params }, { sof_sof_k_params });
simulation->addElement(sof_sof_k);
element::GaussKernelParameters lof_lof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lof_lof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lof", dim_params }, { lof_lof_k_params });
simulation->addElement(lof_lof_k);
element::GaussKernelParameters bof_bof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto bof_bof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> bof", dim_params }, { bof_bof_k_params });
simulation->addElement(bof_bof_k);
element::GaussKernelParameters yof_yof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto yof_yof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yof", dim_params }, { yof_yof_k_params });
simulation->addElement(yof_yof_k);

element::GaussKernelParameters yhf_yhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto yhf_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> yhf", dim_params }, { yhf_yhf_k_params });
simulation->addElement(yhf_yhf_k);
element::GaussKernelParameters bhf_bhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto bhf_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> bhf", dim_params }, { bhf_bhf_k_params });	
simulation->addElement(bhf_bhf_k);
element::GaussKernelParameters shf_shf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto shf_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "shf -> shf", dim_params }, { shf_shf_k_params });	
simulation->addElement(shf_shf_k);
element::GaussKernelParameters lhf_lhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lhf_lhf_k = factory.createElement(element::GAUSS_KERNEL,	
	{ "lhf -> lhf", dim_params }, { lhf_lhf_k_params });	
simulation->addElement(lhf_lhf_k);

element::MexicanHatKernelParameters ael_ael_k_params = { 5.09, 7.85, 100, 30, -0.42, circularity, true };
const auto ael_ael_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "ael -> ael", dim_params }, { ael_ael_k_params });
simulation->addElement(ael_ael_k);

//interction kernels
element::GaussKernelParameters aol_sosf_k_params = { 5, -6.65, 0.0, circularity, true };
const auto aol_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> sosf", dim_params }, { aol_sosf_k_params });
simulation->addElement(aol_sosf_k);

element::GaussKernelParameters aol_loif_k_params = { 5, 12, 0.0, circularity, true };
const auto aol_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> loif", dim_params }, { aol_loif_k_params });
simulation->addElement(aol_loif_k);

element::GaussKernelParameters sof_sosf_k_params = { 5, 10.17, 0.0, circularity, true };
const auto sof_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sosf", dim_params }, { sof_sosf_k_params });
simulation->addElement(sof_sosf_k);

element::GaussKernelParameters lof_loif_k_params = { 5, 12, 0.0, circularity, true };
const auto lof_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> loif", dim_params }, { lof_loif_k_params });
simulation->addElement(lof_loif_k);


element::GaussKernelParameters sosf_ael_k_params = { 5, 42, 0.0, circularity, true };
const auto sosf_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sosf -> ael", dim_params }, { sosf_ael_k_params });
simulation->addElement(sosf_ael_k);

element::GaussKernelParameters loif_ael_k_params = { 5, 26, 0.0, circularity, true };
const auto loif_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "loif -> ael", dim_params }, { loif_ael_k_params });
simulation->addElement(loif_ael_k);


element::GaussKernelParameters yof_yhf_k_params = { 4, 25, 0.0, circularity, true };
const auto yof_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yhf", dim_params }, { yof_yhf_k_params });
simulation->addElement(yof_yhf_k);
element::GaussKernelParameters bof_bhf_k_params = { 4, 25, 0.0, circularity, true };	
const auto bof_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> bhf", dim_params }, { bof_bhf_k_params });
simulation->addElement(bof_bhf_k);
element::GaussKernelParameters sof_shf_k_params = { 4, 25, 0.0, circularity, true };	
const auto sof_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> shf", dim_params }, { sof_shf_k_params });
simulation->addElement(sof_shf_k);
element::GaussKernelParameters lof_lhf_k_params = { 4, 25, 0.0, circularity, true };	
const auto lof_lhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lhf", dim_params }, { lof_lhf_k_params });	
simulation->addElement(lof_lhf_k);


element::GaussKernelParameters yhf_sosf_k_params = { 5, 10, -0.01, circularity, true };
const auto yhf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> sosf", dim_params }, { yhf_sosf_k_params });
simulation->addElement(yhf_sosf_k);

element::GaussKernelParameters yhf_loif_k_params = { 5, 2, 0, circularity, true };
const auto yhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> loif", dim_params }, { yhf_loif_k_params });
simulation->addElement(yhf_loif_k);

element::GaussKernelParameters bhf_sosf_k_params = { 5, 10, -0.01, circularity, true };
const auto bhf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> sosf", dim_params }, { bhf_sosf_k_params });
simulation->addElement(bhf_sosf_k);

element::GaussKernelParameters bhf_loif_k_params = { 5, 2, 0, circularity, true };
const auto bhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> loif", dim_params }, { bhf_loif_k_params });
simulation->addElement(bhf_loif_k);

element::GaussKernelParameters shf_sosf_k_params = { 5, 11, -0.01, circularity, true };
const auto shf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "shf -> sosf", dim_params }, { shf_sosf_k_params });
simulation->addElement(shf_sosf_k);

element::GaussKernelParameters lhf_loif_k_params = { 5, 3, -0.01, circularity, true };
const auto lhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lhf -> loif", dim_params }, { lhf_loif_k_params });
simulation->addElement(lhf_loif_k);


//noise

element::NormalNoiseParameters ael_nn_params = { 0.45 };
const auto ael_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise ael", dim_params }, ael_nn_params);
simulation->addElement(ael_nn);

	element::NormalNoiseParameters sosf_nn_params = { 0.32 };
const auto sosf_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise sosf", dim_params }, sosf_nn_params);
simulation->addElement(sosf_nn);

//interactions
//self-excitation
simulation->createInteraction("aol", "output", "aol -> aol");
simulation->createInteraction("aol -> aol", "output", "aol");

simulation->createInteraction("sosf", "output", "sosf -> sosf");
simulation->createInteraction("sosf -> sosf", "output", "sosf");

simulation->createInteraction("loif", "output", "loif -> loif");
simulation->createInteraction("loif -> loif", "output", "loif");

simulation->createInteraction("sof", "output", "sof -> sof");
simulation->createInteraction("sof -> sof", "output", "sof");
simulation->createInteraction("lof", "output", "lof -> lof");
simulation->createInteraction("lof -> lof", "output", "lof");
simulation->createInteraction("bof", "output", "bof -> bof");
simulation->createInteraction("bof -> bof", "output", "bof");
simulation->createInteraction("yof", "output", "yof -> yof");
simulation->createInteraction("yof -> yof", "output", "yof");

simulation->createInteraction("yhf", "output", "yhf -> yhf");
simulation->createInteraction("yhf -> yhf", "output", "yhf");
simulation->createInteraction("bhf", "output", "bhf -> bhf");
simulation->createInteraction("bhf -> bhf", "output", "bhf");
simulation->createInteraction("shf", "output", "shf -> shf");
simulation->createInteraction("shf -> shf", "output", "shf");
simulation->createInteraction("lhf", "output", "lhf -> lhf");
simulation->createInteraction("lhf -> lhf", "output", "lhf");


simulation->createInteraction("ael", "output", "ael -> ael");
simulation->createInteraction("ael -> ael", "output", "ael");

//stimuli'
simulation->createInteraction("hand position stimulus", "output", "aol");

simulation->createInteraction("short stimulus 1", "output", "sof");
simulation->createInteraction("short stimulus 2", "output", "sof");
simulation->createInteraction("short stimulus 3", "output", "sof");
simulation->createInteraction("short stimulus 4", "output", "sof");

simulation->createInteraction("long stimulus 1", "output", "lof");
simulation->createInteraction("long stimulus 2", "output", "lof");
simulation->createInteraction("long stimulus 3", "output", "lof");
simulation->createInteraction("long stimulus 4", "output", "lof");


simulation->createInteraction("long stimulus 3", "output", "bof");
simulation->createInteraction("long stimulus 4", "output", "bof");
simulation->createInteraction("short stimulus 3", "output", "bof");
simulation->createInteraction("short stimulus 4", "output", "bof");

simulation->createInteraction("long stimulus 1", "output", "yof");
simulation->createInteraction("long stimulus 2", "output", "yof");
simulation->createInteraction("short stimulus 1", "output", "yof");
simulation->createInteraction("short stimulus 2", "output", "yof");


//interactions

simulation->createInteraction("aol", "output", "aol -> sosf");
simulation->createInteraction("aol -> sosf", "output", "sosf");

simulation->createInteraction("aol", "output", "aol -> loif");
simulation->createInteraction("aol -> loif", "output", "loif");

simulation->createInteraction("sof", "output", "sof -> sosf");
simulation->createInteraction("sof -> sosf", "output", "sosf");

simulation->createInteraction("lof", "output", "lof -> loif");
simulation->createInteraction("lof -> loif", "output", "loif");

simulation->createInteraction("yof", "output", "yof -> yhf");
simulation->createInteraction("yof -> yhf", "output", "yhf");
simulation->createInteraction("bof", "output", "bof -> bhf");
simulation->createInteraction("bof -> bhf", "output", "bhf");
simulation->createInteraction("sof", "output", "sof -> shf");
simulation->createInteraction("sof -> shf", "output", "shf");
simulation->createInteraction("lof", "output", "lof -> lhf");
simulation->createInteraction("lof -> lhf", "output", "lhf");

//simulation->createInteraction("yhf", "output", "yhf -> sosf");
//simulation->createInteraction("yhf -> sosf", "output", "sosf");
//simulation->createInteraction("yhf", "output", "yhf -> loif");
//simulation->createInteraction("yhf -> loif", "output", "loif");

//simulation->createInteraction("bhf", "output", "bhf -> sosf");
//simulation->createInteraction("bhf -> sosf", "output", "sosf");
//simulation->createInteraction("bhf", "output", "bhf -> loif");
//simulation->createInteraction("bhf -> loif", "output", "loif");

//simulation->createInteraction("shf", "output", "shf -> sosf");
//simulation->createInteraction("shf -> sosf", "output", "sosf");

simulation->createInteraction("lhf", "output", "lhf -> loif");
simulation->createInteraction("lhf -> loif", "output", "loif");


simulation->createInteraction("sosf", "output", "sosf -> ael");
simulation->createInteraction("sosf -> ael", "output", "ael");
simulation->createInteraction("loif", "output", "loif -> ael");
simulation->createInteraction("loif -> ael", "output", "ael");


//noise
simulation->createInteraction("normal noise ael", "output", "ael");
simulation->createInteraction("normal noise sosf", "output", "sosf");


return simulation;
}

std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitectureKnownShort(const std::string& id, const double& deltaT)

{
using namespace dnf_composer;
auto simulation = std::make_shared<Simulation>(id, deltaT, 0, 0);

element::ElementFactory factory;
element::ElementDimensions dim_params{ 240, 0.5 };
constexpr bool circularity = true;
constexpr bool normalization = false;
constexpr double tau = 100;
constexpr double resting_level = -10;
constexpr double x_shift = 0;
constexpr double steepness = 5;
constexpr double stimulus_sigma = 4;
constexpr double stimulus_amplitude = 15;
constexpr double noise_amplitude = 0.05;

//neural fields
const element::SigmoidFunction aol_af = { x_shift, steepness };
element::NeuralFieldParameters aol_params = { tau, resting_level, aol_af };
const auto aol = factory.createElement(element::NEURAL_FIELD,
	{ "aol", dim_params }, { aol_params });
simulation->addElement(aol);

const element::SigmoidFunction sosf_af = { x_shift, steepness };
element::NeuralFieldParameters sosf_params = { tau, resting_level, sosf_af };
const auto sosf = factory.createElement(element::NEURAL_FIELD,
	{ "sosf", dim_params }, { sosf_params });
simulation->addElement(sosf);

const element::SigmoidFunction loif_af = { x_shift, steepness };
element::NeuralFieldParameters loif_params = { tau, resting_level, loif_af };
const auto loif = factory.createElement(element::NEURAL_FIELD,
	{ "loif", dim_params }, { loif_params });
simulation->addElement(loif);

element::SigmoidFunction sof_af = { x_shift, steepness };
element::NeuralFieldParameters sof_params = { tau, resting_level, sof_af };
const auto sof = factory.createElement(element::NEURAL_FIELD,
	{ "sof", dim_params }, { sof_params });
simulation->addElement(sof);

element::SigmoidFunction lof_af = { x_shift, steepness };
element::NeuralFieldParameters lof_params = { tau, resting_level, lof_af };
const auto lof = factory.createElement(element::NEURAL_FIELD,
	{ "lof", dim_params }, { lof_params });
simulation->addElement(lof);

element::SigmoidFunction bof_af= { x_shift, steepness };
element::NeuralFieldParameters bof_params = { tau, resting_level, bof_af };
const auto bof = factory.createElement(element::NEURAL_FIELD,
	{ "bof", dim_params }, { bof_params });
simulation->addElement(bof);

element::SigmoidFunction yof_af= { x_shift, steepness };
element::NeuralFieldParameters yof_params = { tau, resting_level, yof_af };
const auto yof = factory.createElement(element::NEURAL_FIELD,
	{ "yof", dim_params }, { yof_params });
simulation->addElement(yof);

element::SigmoidFunction yhf_af= { x_shift, steepness };
element::NeuralFieldParameters yhf_params = { tau, resting_level, yhf_af };
const auto yhf = factory.createElement(element::NEURAL_FIELD,
	{ "yhf", dim_params }, { yhf_params });
simulation->addElement(yhf);

element::SigmoidFunction bhf_af= { x_shift, steepness };
element::NeuralFieldParameters bhf_params = { tau, resting_level, bhf_af };
const auto bhf = factory.createElement(element::NEURAL_FIELD,
	{ "bhf", dim_params }, { bhf_params });
simulation->addElement(bhf);

element::SigmoidFunction shf_af= { x_shift, steepness };
element::NeuralFieldParameters shf_params = { tau, resting_level, shf_af };
const auto shf = factory.createElement(element::NEURAL_FIELD,
	{ "shf", dim_params }, { shf_params });
simulation->addElement(shf);

element::SigmoidFunction lhf_af= { x_shift, steepness };
element::NeuralFieldParameters lhf_params = { tau, resting_level, lhf_af };
const auto lhf = factory.createElement(element::NEURAL_FIELD,
	{ "lhf", dim_params }, { lhf_params });
simulation->addElement(lhf);

element::SigmoidFunction ael_af = { x_shift, steepness };
element::NeuralFieldParameters ael_params = { tau, resting_level, ael_af };
const auto ael = factory.createElement(element::NEURAL_FIELD,
	{ "ael", dim_params }, { ael_params });
simulation->addElement(ael);

//stimuli
element::GaussStimulusParameters hand_position_gsp = { stimulus_sigma, 0, 0, circularity, true };
const auto hand_position_stimulus = factory.createElement(element::GAUSS_STIMULUS,
	{ "hand position stimulus", dim_params }, { hand_position_gsp });
simulation->addElement(hand_position_stimulus);


element::GaussStimulusParameters sof_gsp = { stimulus_sigma, stimulus_amplitude, 15, circularity, true };
const auto sof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 4", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_1);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 45, circularity, true };
const auto sof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 3", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_2);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 135, circularity, true };
const auto sof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 2", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_3);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 165, circularity, true };
const auto sof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 1", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_4);

element::GaussStimulusParameters lof_gsp = { stimulus_sigma, stimulus_amplitude, 75, circularity, true };
const auto lof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 4", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_1);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 105, circularity, true };
const auto lof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 3", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_2);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 195, circularity, true };
const auto lof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 2", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_3);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 225, circularity, true };
const auto lof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 1", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_4);



//self excitations kernels
element::GaussKernelParameters aol_aol_k_params = { 20, 2, -0.01, circularity, true};
const auto aol_aol_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> aol", dim_params }, { aol_aol_k_params });
simulation->addElement(aol_aol_k);

element::MexicanHatKernelParameters sosf_sosf_k_params = { 18.92, 35, 150, 100, -0.23, circularity, true };
const auto sosf_sosf_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "sosf -> sosf", dim_params }, { sosf_sosf_k_params });
simulation->addElement(sosf_sosf_k);

element::MexicanHatKernelParameters loif_loif_k_params = { 20, 22, 100, 10, -0.01, circularity, true };
const auto loif_loif_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "loif -> loif", dim_params }, { loif_loif_k_params });
simulation->addElement(loif_loif_k);

element::GaussKernelParameters sof_sof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto sof_sof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sof", dim_params }, { sof_sof_k_params });
simulation->addElement(sof_sof_k);
element::GaussKernelParameters lof_lof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lof_lof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lof", dim_params }, { lof_lof_k_params });
simulation->addElement(lof_lof_k);
element::GaussKernelParameters bof_bof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto bof_bof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> bof", dim_params }, { bof_bof_k_params });
simulation->addElement(bof_bof_k);
element::GaussKernelParameters yof_yof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto yof_yof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yof", dim_params }, { yof_yof_k_params });
simulation->addElement(yof_yof_k);

element::GaussKernelParameters yhf_yhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto yhf_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> yhf", dim_params }, { yhf_yhf_k_params });
simulation->addElement(yhf_yhf_k);
element::GaussKernelParameters bhf_bhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto bhf_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> bhf", dim_params }, { bhf_bhf_k_params });	
simulation->addElement(bhf_bhf_k);
element::GaussKernelParameters shf_shf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto shf_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "shf -> shf", dim_params }, { shf_shf_k_params });	
simulation->addElement(shf_shf_k);
element::GaussKernelParameters lhf_lhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lhf_lhf_k = factory.createElement(element::GAUSS_KERNEL,	
	{ "lhf -> lhf", dim_params }, { lhf_lhf_k_params });	
simulation->addElement(lhf_lhf_k);

element::MexicanHatKernelParameters ael_ael_k_params = { 5.09, 7.85, 100, 30, -0.42, circularity, true };
const auto ael_ael_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "ael -> ael", dim_params }, { ael_ael_k_params });
simulation->addElement(ael_ael_k);

//interction kernels
element::GaussKernelParameters aol_sosf_k_params = { 5, -6.65, 0.0, circularity, true };
const auto aol_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> sosf", dim_params }, { aol_sosf_k_params });
simulation->addElement(aol_sosf_k);

element::GaussKernelParameters aol_loif_k_params = { 5, 12, 0.0, circularity, true };
const auto aol_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> loif", dim_params }, { aol_loif_k_params });
simulation->addElement(aol_loif_k);

element::GaussKernelParameters sof_sosf_k_params = { 5, 10.17, 0.0, circularity, true };
const auto sof_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sosf", dim_params }, { sof_sosf_k_params });
simulation->addElement(sof_sosf_k);

element::GaussKernelParameters lof_loif_k_params = { 5, 12, 0.0, circularity, true };
const auto lof_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> loif", dim_params }, { lof_loif_k_params });
simulation->addElement(lof_loif_k);


element::GaussKernelParameters sosf_ael_k_params = { 5, 42, 0.0, circularity, true };
const auto sosf_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sosf -> ael", dim_params }, { sosf_ael_k_params });
simulation->addElement(sosf_ael_k);

element::GaussKernelParameters loif_ael_k_params = { 5, 26, 0.0, circularity, true };
const auto loif_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "loif -> ael", dim_params }, { loif_ael_k_params });
simulation->addElement(loif_ael_k);


element::GaussKernelParameters yof_yhf_k_params = { 4, 25, 0.0, circularity, true };
const auto yof_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yhf", dim_params }, { yof_yhf_k_params });
simulation->addElement(yof_yhf_k);
element::GaussKernelParameters bof_bhf_k_params = { 4, 25, 0.0, circularity, true };	
const auto bof_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> bhf", dim_params }, { bof_bhf_k_params });
simulation->addElement(bof_bhf_k);
element::GaussKernelParameters sof_shf_k_params = { 4, 25, 0.0, circularity, true };	
const auto sof_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> shf", dim_params }, { sof_shf_k_params });
simulation->addElement(sof_shf_k);
element::GaussKernelParameters lof_lhf_k_params = { 4, 25, 0.0, circularity, true };	
const auto lof_lhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lhf", dim_params }, { lof_lhf_k_params });	
simulation->addElement(lof_lhf_k);


element::GaussKernelParameters yhf_sosf_k_params = { 5, 10, -0.01, circularity, true };
const auto yhf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> sosf", dim_params }, { yhf_sosf_k_params });
simulation->addElement(yhf_sosf_k);

element::GaussKernelParameters yhf_loif_k_params = { 5, 2, 0, circularity, true };
const auto yhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> loif", dim_params }, { yhf_loif_k_params });
simulation->addElement(yhf_loif_k);

element::GaussKernelParameters bhf_sosf_k_params = { 5, 10, -0.01, circularity, true };
const auto bhf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> sosf", dim_params }, { bhf_sosf_k_params });
simulation->addElement(bhf_sosf_k);

element::GaussKernelParameters bhf_loif_k_params = { 5, 2, 0, circularity, true };
const auto bhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> loif", dim_params }, { bhf_loif_k_params });
simulation->addElement(bhf_loif_k);

element::GaussKernelParameters shf_sosf_k_params = { 5, 11, -0.01, circularity, true };
const auto shf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "shf -> sosf", dim_params }, { shf_sosf_k_params });
simulation->addElement(shf_sosf_k);

element::GaussKernelParameters lhf_loif_k_params = { 5, 3, -0.01, circularity, true };
const auto lhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lhf -> loif", dim_params }, { lhf_loif_k_params });
simulation->addElement(lhf_loif_k);


//noise

element::NormalNoiseParameters ael_nn_params = { 0.45 };
const auto ael_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise ael", dim_params }, ael_nn_params);
simulation->addElement(ael_nn);

	element::NormalNoiseParameters sosf_nn_params = { 0.32 };
const auto sosf_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise sosf", dim_params }, sosf_nn_params);
simulation->addElement(sosf_nn);

//interactions
//self-excitation
simulation->createInteraction("aol", "output", "aol -> aol");
simulation->createInteraction("aol -> aol", "output", "aol");

simulation->createInteraction("sosf", "output", "sosf -> sosf");
simulation->createInteraction("sosf -> sosf", "output", "sosf");

simulation->createInteraction("loif", "output", "loif -> loif");
simulation->createInteraction("loif -> loif", "output", "loif");

simulation->createInteraction("sof", "output", "sof -> sof");
simulation->createInteraction("sof -> sof", "output", "sof");
simulation->createInteraction("lof", "output", "lof -> lof");
simulation->createInteraction("lof -> lof", "output", "lof");
simulation->createInteraction("bof", "output", "bof -> bof");
simulation->createInteraction("bof -> bof", "output", "bof");
simulation->createInteraction("yof", "output", "yof -> yof");
simulation->createInteraction("yof -> yof", "output", "yof");

simulation->createInteraction("yhf", "output", "yhf -> yhf");
simulation->createInteraction("yhf -> yhf", "output", "yhf");
simulation->createInteraction("bhf", "output", "bhf -> bhf");
simulation->createInteraction("bhf -> bhf", "output", "bhf");
simulation->createInteraction("shf", "output", "shf -> shf");
simulation->createInteraction("shf -> shf", "output", "shf");
simulation->createInteraction("lhf", "output", "lhf -> lhf");
simulation->createInteraction("lhf -> lhf", "output", "lhf");


simulation->createInteraction("ael", "output", "ael -> ael");
simulation->createInteraction("ael -> ael", "output", "ael");

//stimuli'
simulation->createInteraction("hand position stimulus", "output", "aol");

simulation->createInteraction("short stimulus 1", "output", "sof");
simulation->createInteraction("short stimulus 2", "output", "sof");
simulation->createInteraction("short stimulus 3", "output", "sof");
simulation->createInteraction("short stimulus 4", "output", "sof");

simulation->createInteraction("long stimulus 1", "output", "lof");
simulation->createInteraction("long stimulus 2", "output", "lof");
simulation->createInteraction("long stimulus 3", "output", "lof");
simulation->createInteraction("long stimulus 4", "output", "lof");


simulation->createInteraction("long stimulus 3", "output", "bof");
simulation->createInteraction("long stimulus 4", "output", "bof");
simulation->createInteraction("short stimulus 3", "output", "bof");
simulation->createInteraction("short stimulus 4", "output", "bof");

simulation->createInteraction("long stimulus 1", "output", "yof");
simulation->createInteraction("long stimulus 2", "output", "yof");
simulation->createInteraction("short stimulus 1", "output", "yof");
simulation->createInteraction("short stimulus 2", "output", "yof");


//interactions

simulation->createInteraction("aol", "output", "aol -> sosf");
simulation->createInteraction("aol -> sosf", "output", "sosf");

simulation->createInteraction("aol", "output", "aol -> loif");
simulation->createInteraction("aol -> loif", "output", "loif");

simulation->createInteraction("sof", "output", "sof -> sosf");
simulation->createInteraction("sof -> sosf", "output", "sosf");

simulation->createInteraction("lof", "output", "lof -> loif");
simulation->createInteraction("lof -> loif", "output", "loif");

simulation->createInteraction("yof", "output", "yof -> yhf");
simulation->createInteraction("yof -> yhf", "output", "yhf");
simulation->createInteraction("bof", "output", "bof -> bhf");
simulation->createInteraction("bof -> bhf", "output", "bhf");
simulation->createInteraction("sof", "output", "sof -> shf");
simulation->createInteraction("sof -> shf", "output", "shf");
simulation->createInteraction("lof", "output", "lof -> lhf");
simulation->createInteraction("lof -> lhf", "output", "lhf");

//simulation->createInteraction("yhf", "output", "yhf -> sosf");
//simulation->createInteraction("yhf -> sosf", "output", "sosf");
//simulation->createInteraction("yhf", "output", "yhf -> loif");
//simulation->createInteraction("yhf -> loif", "output", "loif");

//simulation->createInteraction("bhf", "output", "bhf -> sosf");
//simulation->createInteraction("bhf -> sosf", "output", "sosf");
//simulation->createInteraction("bhf", "output", "bhf -> loif");
//simulation->createInteraction("bhf -> loif", "output", "loif");

simulation->createInteraction("shf", "output", "shf -> sosf");
simulation->createInteraction("shf -> sosf", "output", "sosf");

//simulation->createInteraction("lhf", "output", "lhf -> loif");
//simulation->createInteraction("lhf -> loif", "output", "loif");


simulation->createInteraction("sosf", "output", "sosf -> ael");
simulation->createInteraction("sosf -> ael", "output", "ael");
simulation->createInteraction("loif", "output", "loif -> ael");
simulation->createInteraction("loif -> ael", "output", "ael");


//noise
simulation->createInteraction("normal noise ael", "output", "ael");
simulation->createInteraction("normal noise sosf", "output", "sosf");


return simulation;
}

std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitectureInferred(const std::string& id, const double& deltaT)
{
	using namespace dnf_composer;
auto simulation = std::make_shared<Simulation>(id, deltaT, 0, 0);

element::ElementFactory factory;
element::ElementDimensions dim_params{ 240, 0.5 };
constexpr bool circularity = true;
constexpr bool normalization = false;
constexpr double tau = 100;
constexpr double resting_level = -10;
constexpr double x_shift = 0;
constexpr double steepness = 5;
constexpr double stimulus_sigma = 4;
constexpr double stimulus_amplitude = 15;
constexpr double noise_amplitude = 0.05;

//neural fields
const element::SigmoidFunction aol_af = { x_shift, steepness };
element::NeuralFieldParameters aol_params = { tau, resting_level, aol_af };
const auto aol = factory.createElement(element::NEURAL_FIELD,
	{ "aol", dim_params }, { aol_params });
simulation->addElement(aol);

const element::SigmoidFunction sosf_af = { x_shift, steepness };
element::NeuralFieldParameters sosf_params = { tau, resting_level, sosf_af };
const auto sosf = factory.createElement(element::NEURAL_FIELD,
	{ "sosf", dim_params }, { sosf_params });
simulation->addElement(sosf);

const element::SigmoidFunction loif_af = { x_shift, steepness };
element::NeuralFieldParameters loif_params = { tau, resting_level, loif_af };
const auto loif = factory.createElement(element::NEURAL_FIELD,
	{ "loif", dim_params }, { loif_params });
simulation->addElement(loif);

const element::SigmoidFunction soif_af = { x_shift, steepness };
element::NeuralFieldParameters soif_params = { tau, resting_level, soif_af };
const auto soif = factory.createElement(element::NEURAL_FIELD,
	{ "soif", dim_params }, { soif_params });
simulation->addElement(soif);

const element::SigmoidFunction boif_af = { x_shift, steepness };
element::NeuralFieldParameters boif_params = { tau, resting_level, boif_af };
const auto boif = factory.createElement(element::NEURAL_FIELD,
	{ "boif", dim_params }, { boif_params });
simulation->addElement(boif);

const element::SigmoidFunction yoif_af = { x_shift, steepness };
element::NeuralFieldParameters yoif_params = { tau, resting_level, yoif_af };
const auto yoif = factory.createElement(element::NEURAL_FIELD,
	{ "yoif", dim_params }, { yoif_params });
simulation->addElement(yoif);

element::SigmoidFunction sof_af = { x_shift, steepness };
element::NeuralFieldParameters sof_params = { tau, resting_level, sof_af };
const auto sof = factory.createElement(element::NEURAL_FIELD,
	{ "sof", dim_params }, { sof_params });
simulation->addElement(sof);

element::SigmoidFunction lof_af = { x_shift, steepness };
element::NeuralFieldParameters lof_params = { tau, resting_level, lof_af };
const auto lof = factory.createElement(element::NEURAL_FIELD,
	{ "lof", dim_params }, { lof_params });
simulation->addElement(lof);

element::SigmoidFunction bof_af= { x_shift, steepness };
element::NeuralFieldParameters bof_params = { tau, resting_level, bof_af };
const auto bof = factory.createElement(element::NEURAL_FIELD,
	{ "bof", dim_params }, { bof_params });
simulation->addElement(bof);

element::SigmoidFunction yof_af= { x_shift, steepness };
element::NeuralFieldParameters yof_params = { tau, resting_level, yof_af };
const auto yof = factory.createElement(element::NEURAL_FIELD,
	{ "yof", dim_params }, { yof_params });
simulation->addElement(yof);

element::SigmoidFunction yhf_af= { x_shift, steepness };
element::NeuralFieldParameters yhf_params = { tau, resting_level, yhf_af };
const auto yhf = factory.createElement(element::NEURAL_FIELD,
	{ "yhf", dim_params }, { yhf_params });
simulation->addElement(yhf);

element::SigmoidFunction bhf_af= { x_shift, steepness };
element::NeuralFieldParameters bhf_params = { tau, resting_level, bhf_af };
const auto bhf = factory.createElement(element::NEURAL_FIELD,
	{ "bhf", dim_params }, { bhf_params });
simulation->addElement(bhf);

element::SigmoidFunction shf_af= { x_shift, steepness };
element::NeuralFieldParameters shf_params = { tau, resting_level, shf_af };
const auto shf = factory.createElement(element::NEURAL_FIELD,
	{ "shf", dim_params }, { shf_params });
simulation->addElement(shf);

element::SigmoidFunction lhf_af= { x_shift, steepness };
element::NeuralFieldParameters lhf_params = { tau, resting_level, lhf_af };
const auto lhf = factory.createElement(element::NEURAL_FIELD,
	{ "lhf", dim_params }, { lhf_params });
simulation->addElement(lhf);


element::NeuralFieldParameters tf_params = { 200, resting_level, lhf_af };
const auto yht = factory.createElement(element::NEURAL_FIELD,
	{ "yht", dim_params }, { tf_params });
simulation->addElement(yht);
const auto bht = factory.createElement(element::NEURAL_FIELD,
	{ "bht", dim_params }, { yhf_params });
simulation->addElement(bht);
const auto sht = factory.createElement(element::NEURAL_FIELD,
	{ "sht", dim_params }, { yhf_params });
simulation->addElement(sht);
const auto lht = factory.createElement(element::NEURAL_FIELD,
	{ "lht", dim_params }, { yhf_params });
simulation->addElement(lht);


element::SigmoidFunction ael_af = { x_shift, steepness };
element::NeuralFieldParameters ael_params = { tau, resting_level, ael_af };
const auto ael = factory.createElement(element::NEURAL_FIELD,
	{ "ael", dim_params }, { ael_params });
simulation->addElement(ael);

//stimuli
element::GaussStimulusParameters hand_position_gsp = { stimulus_sigma, 0, 0, circularity, true };
const auto hand_position_stimulus = factory.createElement(element::GAUSS_STIMULUS,
	{ "hand position stimulus", dim_params }, { hand_position_gsp });
simulation->addElement(hand_position_stimulus);


element::GaussStimulusParameters sof_gsp = { stimulus_sigma, stimulus_amplitude, 15, circularity, true };
const auto sof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 4", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_1);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 45, circularity, true };
const auto sof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 3", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_2);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 135, circularity, true };
const auto sof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 2", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_3);
sof_gsp = { stimulus_sigma, stimulus_amplitude, 165, circularity, true };
const auto sof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "short stimulus 1", dim_params }, { sof_gsp });
simulation->addElement(sof_stimulus_4);

element::GaussStimulusParameters lof_gsp = { stimulus_sigma, stimulus_amplitude, 75, circularity, true };
const auto lof_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 4", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_1);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 105, circularity, true };
const auto lof_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 3", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_2);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 195, circularity, true };
const auto lof_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 2", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_3);
lof_gsp = { stimulus_sigma, stimulus_amplitude, 225, circularity, true };
const auto lof_stimulus_4 = factory.createElement(element::GAUSS_STIMULUS,
	{ "long stimulus 1", dim_params }, { lof_gsp });
simulation->addElement(lof_stimulus_4);



//self excitations kernels
element::GaussKernelParameters aol_aol_k_params = { 20, 2, -0.01, circularity, true};
const auto aol_aol_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> aol", dim_params }, { aol_aol_k_params });
simulation->addElement(aol_aol_k);

element::MexicanHatKernelParameters sosf_sosf_k_params = { 18.92, 28, 100, 50, -0.23, circularity, true };
const auto sosf_sosf_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "sosf -> sosf", dim_params }, { sosf_sosf_k_params });
simulation->addElement(sosf_sosf_k);


element::MexicanHatKernelParameters loif_loif_k_params = { 20, 22, 100, 40, -0.01, circularity, true };
const auto loif_loif_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "loif -> loif", dim_params }, { loif_loif_k_params });
simulation->addElement(loif_loif_k);

element::MexicanHatKernelParameters soif_soif_k_params = { 20, 22, 100, 40, -0.01, circularity, true };
const auto soif_soif_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "soif -> soif", dim_params }, { soif_soif_k_params });
simulation->addElement(soif_soif_k);

element::MexicanHatKernelParameters boif_boif_k_params = { 20, 22, 100, 40, -0.01, circularity, true };
const auto boif_boif_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "boif -> boif", dim_params }, { boif_boif_k_params });
simulation->addElement(boif_boif_k);

element::MexicanHatKernelParameters yoif_yoif_k_params = { 20, 22, 100, 40, -0.01, circularity, true };
const auto yoif_yoif_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "yoif -> yoif", dim_params }, { yoif_yoif_k_params });
simulation->addElement(yoif_yoif_k);


/*
element::GaussKernelParameters soif_soif_k_params = { 20, 22, -0.01, circularity, true }; //width, amplitude, shift
const auto soif_soif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "soif -> soif", dim_params }, { soif_soif_k_params });
simulation->addElement(soif_soif_k);
const auto boif_boif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "boif -> boif", dim_params }, { soif_soif_k_params });
simulation->addElement(boif_boif_k);
const auto yoif_yoif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yoif -> yoif", dim_params }, { soif_soif_k_params });
simulation->addElement(yoif_yoif_k);
*/

element::GaussKernelParameters sof_sof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto sof_sof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sof", dim_params }, { sof_sof_k_params });
simulation->addElement(sof_sof_k);
element::GaussKernelParameters lof_lof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lof_lof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lof", dim_params }, { lof_lof_k_params });
simulation->addElement(lof_lof_k);
element::GaussKernelParameters bof_bof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto bof_bof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> bof", dim_params }, { bof_bof_k_params });
simulation->addElement(bof_bof_k);
element::GaussKernelParameters yof_yof_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto yof_yof_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yof", dim_params }, { yof_yof_k_params });
simulation->addElement(yof_yof_k);

element::GaussKernelParameters yhf_yhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto yhf_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> yhf", dim_params }, { yhf_yhf_k_params });
simulation->addElement(yhf_yhf_k);
element::GaussKernelParameters bhf_bhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto bhf_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> bhf", dim_params }, { bhf_bhf_k_params });	
simulation->addElement(bhf_bhf_k);
element::GaussKernelParameters shf_shf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto shf_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "shf -> shf", dim_params }, { shf_shf_k_params });	
simulation->addElement(shf_shf_k);
element::GaussKernelParameters lhf_lhf_k_params = { 20, 2, -0.01, circularity, true }; //width, amplitude, shift
const auto lhf_lhf_k = factory.createElement(element::GAUSS_KERNEL,	
	{ "lhf -> lhf", dim_params }, { lhf_lhf_k_params });	
simulation->addElement(lhf_lhf_k);

element::MexicanHatKernelParameters bht_bht_k_params = { 20,20,50,5, -0.0001, true, true }; //width, amplitude, shift
const auto bht_bht_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "bht -> bht", dim_params }, { bht_bht_k_params });
simulation->addElement(bht_bht_k);
element::MexicanHatKernelParameters yht_yht_k_params = { 20,20,50,5, -0.0001, true, true }; //width, amplitude, shift
const auto yht_yht_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "yht -> yht", dim_params }, { yht_yht_k_params });
simulation->addElement(yht_yht_k);
element::MexicanHatKernelParameters sht_sht_k_params = { 20,20,50,5, -0.0001, true, true }; //width, amplitude, shift
const auto sht_sht_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "sht -> sht", dim_params }, { sht_sht_k_params });
simulation->addElement(sht_sht_k);
element::MexicanHatKernelParameters lht_lht_k_params = { 20,20,50,5, -0.0001, true, true }; //width, amplitude, shift
const auto lht_lht_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "lht -> lht", dim_params }, { lht_lht_k_params });

element::MexicanHatKernelParameters ael_ael_k_params = { 5.09, 10, 160, 50, -0.42, circularity, true };
const auto ael_ael_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
	{ "ael -> ael", dim_params }, { ael_ael_k_params });
simulation->addElement(ael_ael_k);

//interction kernels
element::GaussKernelParameters aol_sosf_k_params = { 5, -6.65, 0.0, circularity, true };
const auto aol_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> sosf", dim_params }, { aol_sosf_k_params });
simulation->addElement(aol_sosf_k);

element::GaussKernelParameters aol_loif_k_params = { 5, 10, -0.01, circularity, true };
const auto aol_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> loif", dim_params }, { aol_loif_k_params });
simulation->addElement(aol_loif_k);
const auto aol_boif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> boif", dim_params }, { aol_loif_k_params });
simulation->addElement(aol_boif_k);
const auto aol_yoif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> yoif", dim_params }, { aol_loif_k_params });
simulation->addElement(aol_yoif_k);
const auto aol_soif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "aol -> soif", dim_params }, { aol_loif_k_params });
simulation->addElement(aol_soif_k);



element::GaussKernelParameters sof_sosf_k_params = { 2.96, 10.75, -0.01, circularity, true };
const auto sof_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> sosf", dim_params }, { sof_sosf_k_params });
simulation->addElement(sof_sosf_k);
element::GaussKernelParameters lof_loif_k_params = { 5, 12, -0.01, circularity, true };
const auto lof_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> loif", dim_params }, { lof_loif_k_params });
simulation->addElement(lof_loif_k);
const auto bof_boif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> boif", dim_params }, { lof_loif_k_params });
simulation->addElement(bof_boif_k);
const auto yof_yoif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yoif", dim_params }, { lof_loif_k_params });
simulation->addElement(yof_yoif_k);
const auto sof_soif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> soif", dim_params }, { lof_loif_k_params });
simulation->addElement(sof_soif_k);



element::GaussKernelParameters sosf_ael_k_params = { 5, 40, -0.01, circularity, true };
const auto sosf_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sosf -> ael", dim_params }, { sosf_ael_k_params });
simulation->addElement(sosf_ael_k);

element::GaussKernelParameters loif_ael_k_params = { 5, 50, -0.01, circularity, true };
const auto loif_ael_k = factory.createElement(element::GAUSS_KERNEL,
	{ "loif -> ael", dim_params }, { loif_ael_k_params });
simulation->addElement(loif_ael_k);

element::GaussKernelParameters loif_lht_k_params = { 5, 20, -0.01, false, true };
const auto loif_lht_k = factory.createElement(element::GAUSS_KERNEL,
	{ "loif -> lht", dim_params }, { loif_lht_k_params });
simulation->addElement(loif_lht_k);
const auto boif_bht_k = factory.createElement(element::GAUSS_KERNEL,
	{ "boif -> bht", dim_params }, { loif_lht_k_params });
simulation->addElement(boif_bht_k);
const auto yoif_yht_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yoif -> yht", dim_params }, { loif_lht_k_params });	
simulation->addElement(yoif_yht_k);
const auto soif_sht_k = factory.createElement(element::GAUSS_KERNEL,
	{ "soif -> sht", dim_params }, { loif_lht_k_params });	
simulation->addElement(soif_sht_k);


element::GaussKernelParameters yof_yhf_k_params = { 4, 12, -0.01, circularity, true };
const auto yof_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yof -> yhf", dim_params }, { yof_yhf_k_params });
simulation->addElement(yof_yhf_k);
element::GaussKernelParameters bof_bhf_k_params = { 4, 12, -0.01, circularity, true };	
const auto bof_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bof -> bhf", dim_params }, { bof_bhf_k_params });
simulation->addElement(bof_bhf_k);
element::GaussKernelParameters sof_shf_k_params = { 4, 12, -0.01, circularity, true };	
const auto sof_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sof -> shf", dim_params }, { sof_shf_k_params });
simulation->addElement(sof_shf_k);
element::GaussKernelParameters lof_lhf_k_params = { 4, 12, -0.01, circularity, true };	
const auto lof_lhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lof -> lhf", dim_params }, { lof_lhf_k_params });	
simulation->addElement(lof_lhf_k);


element::GaussKernelParameters yhf_sosf_k_params = { 5, 7, -0.01, circularity, true };
const auto yhf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> sosf", dim_params }, { yhf_sosf_k_params });
simulation->addElement(yhf_sosf_k);

element::GaussKernelParameters yhf_loif_k_params = { 5, 2, 0, circularity, true };
const auto yhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yhf -> loif", dim_params }, { yhf_loif_k_params });
simulation->addElement(yhf_loif_k);

element::GaussKernelParameters bhf_sosf_k_params = { 5, 7, -0.01, circularity, true };
const auto bhf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> sosf", dim_params }, { bhf_sosf_k_params });
simulation->addElement(bhf_sosf_k);

element::GaussKernelParameters bhf_loif_k_params = { 5, 2, 0, circularity, true };
const auto bhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bhf -> loif", dim_params }, { bhf_loif_k_params });
simulation->addElement(bhf_loif_k);

element::GaussKernelParameters shf_sosf_k_params = { 5, 11, -0.01, circularity, true };
const auto shf_sosf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "shf -> sosf", dim_params }, { shf_sosf_k_params });
simulation->addElement(shf_sosf_k);

element::GaussKernelParameters lhf_loif_k_params = { 5, 3, -0.01, circularity, true };
const auto lhf_loif_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lhf -> loif", dim_params }, { lhf_loif_k_params });
simulation->addElement(lhf_loif_k);


element::GaussKernelParameters ht_hf_k_params = { 2, 0, 0.2, circularity, true };
const auto lht_lhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lht -> lhf", dim_params }, { ht_hf_k_params });
simulation->addElement(lht_lhf_k);
const auto bht_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bht -> bhf", dim_params }, { ht_hf_k_params });
simulation->addElement(bht_bhf_k);
const auto yht_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yht -> yhf", dim_params }, { ht_hf_k_params });
simulation->addElement(yht_yhf_k);
const auto sht_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sht -> shf", dim_params }, { ht_hf_k_params });
simulation->addElement(sht_shf_k);

element::GaussKernelParameters ht_hf_inh_k_params = { 2, 0, -0.2, circularity, true };
const auto lht_shf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "lht -> shf", dim_params }, { ht_hf_inh_k_params });
simulation->addElement(lht_shf_k);
const auto bht_yhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "bht -> yhf", dim_params }, { ht_hf_inh_k_params });
simulation->addElement(bht_yhf_k);
const auto yht_bhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "yht -> bhf", dim_params }, { ht_hf_inh_k_params });
simulation->addElement(yht_bhf_k);
const auto sht_lhf_k = factory.createElement(element::GAUSS_KERNEL,
	{ "sht -> lhf", dim_params }, { ht_hf_inh_k_params });
simulation->addElement(sht_lhf_k);


//noise

element::NormalNoiseParameters ael_nn_params = { 0.45 };
const auto ael_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise ael", dim_params }, ael_nn_params);
simulation->addElement(ael_nn);

	element::NormalNoiseParameters sosf_nn_params = { 0.32 };
const auto sosf_nn = factory.createElement(element::NORMAL_NOISE,
	{ "normal noise sosf", dim_params }, sosf_nn_params);
simulation->addElement(sosf_nn);

//interactions
//self-excitation
simulation->createInteraction("aol", "output", "aol -> aol");
simulation->createInteraction("aol -> aol", "output", "aol");

simulation->createInteraction("sosf", "output", "sosf -> sosf");
simulation->createInteraction("sosf -> sosf", "output", "sosf");

simulation->createInteraction("loif", "output", "loif -> loif");
simulation->createInteraction("loif -> loif", "output", "loif");
simulation->createInteraction("boif", "output", "boif -> boif");
simulation->createInteraction("boif -> boif", "output", "boif");
simulation->createInteraction("yoif", "output", "yoif -> yoif");
simulation->createInteraction("yoif -> yoif", "output", "yoif");	
simulation->createInteraction("soif", "output", "soif -> soif");
simulation->createInteraction("soif -> soif", "output", "soif");

simulation->createInteraction("sof", "output", "sof -> sof");
simulation->createInteraction("sof -> sof", "output", "sof");
simulation->createInteraction("lof", "output", "lof -> lof");
simulation->createInteraction("lof -> lof", "output", "lof");
simulation->createInteraction("bof", "output", "bof -> bof");
simulation->createInteraction("bof -> bof", "output", "bof");
simulation->createInteraction("yof", "output", "yof -> yof");
simulation->createInteraction("yof -> yof", "output", "yof");

simulation->createInteraction("yhf", "output", "yhf -> yhf");
simulation->createInteraction("yhf -> yhf", "output", "yhf");
simulation->createInteraction("bhf", "output", "bhf -> bhf");
simulation->createInteraction("bhf -> bhf", "output", "bhf");
simulation->createInteraction("shf", "output", "shf -> shf");
simulation->createInteraction("shf -> shf", "output", "shf");
simulation->createInteraction("lhf", "output", "lhf -> lhf");
simulation->createInteraction("lhf -> lhf", "output", "lhf");

simulation->createInteraction("yht", "output", "yht -> yht");
simulation->createInteraction("yht -> yht", "output", "yht");
simulation->createInteraction("bht", "output", "bht -> bht");
simulation->createInteraction("bht -> bht", "output", "bht");
simulation->createInteraction("sht", "output", "sht -> sht");
simulation->createInteraction("sht -> sht", "output", "sht");
simulation->createInteraction("lht", "output", "lht -> lht");
simulation->createInteraction("lht -> lht", "output", "lht");


simulation->createInteraction("ael", "output", "ael -> ael");
simulation->createInteraction("ael -> ael", "output", "ael");

//stimuli'
simulation->createInteraction("hand position stimulus", "output", "aol");

simulation->createInteraction("short stimulus 1", "output", "sof");
simulation->createInteraction("short stimulus 2", "output", "sof");
simulation->createInteraction("short stimulus 3", "output", "sof");
simulation->createInteraction("short stimulus 4", "output", "sof");

simulation->createInteraction("long stimulus 1", "output", "lof");
simulation->createInteraction("long stimulus 2", "output", "lof");
simulation->createInteraction("long stimulus 3", "output", "lof");
simulation->createInteraction("long stimulus 4", "output", "lof");


simulation->createInteraction("long stimulus 3", "output", "bof");
simulation->createInteraction("long stimulus 4", "output", "bof");
simulation->createInteraction("short stimulus 3", "output", "bof");
simulation->createInteraction("short stimulus 4", "output", "bof");

simulation->createInteraction("long stimulus 1", "output", "yof");
simulation->createInteraction("long stimulus 2", "output", "yof");
simulation->createInteraction("short stimulus 1", "output", "yof");
simulation->createInteraction("short stimulus 2", "output", "yof");


//interactions

simulation->createInteraction("aol", "output", "aol -> sosf");
simulation->createInteraction("aol -> sosf", "output", "sosf");

simulation->createInteraction("aol", "output", "aol -> loif");
simulation->createInteraction("aol -> loif", "output", "loif");
simulation->createInteraction("aol", "output", "aol -> boif");
simulation->createInteraction("aol -> boif", "output", "boif");
simulation->createInteraction("aol", "output", "aol -> yoif");
simulation->createInteraction("aol -> yoif", "output", "yoif");
simulation->createInteraction("aol", "output", "aol -> soif");
simulation->createInteraction("aol -> soif", "output", "soif");

simulation->createInteraction("sof", "output", "sof -> sosf");
simulation->createInteraction("sof -> sosf", "output", "sosf");

simulation->createInteraction("lof", "output", "lof -> loif");
simulation->createInteraction("lof -> loif", "output", "loif");
simulation->createInteraction("bof", "output", "bof -> boif");
simulation->createInteraction("bof -> boif", "output", "boif");
simulation->createInteraction("yof", "output", "yof -> yoif");
simulation->createInteraction("yof -> yoif", "output", "yoif");
simulation->createInteraction("sof", "output", "sof -> soif");
simulation->createInteraction("sof -> soif", "output", "soif");

simulation->createInteraction("yof", "output", "yof -> yhf");
simulation->createInteraction("yof -> yhf", "output", "yhf");
simulation->createInteraction("bof", "output", "bof -> bhf");
simulation->createInteraction("bof -> bhf", "output", "bhf");
simulation->createInteraction("sof", "output", "sof -> shf");
simulation->createInteraction("sof -> shf", "output", "shf");
simulation->createInteraction("lof", "output", "lof -> lhf");
simulation->createInteraction("lof -> lhf", "output", "lhf");

simulation->createInteraction("yhf", "output", "yhf -> sosf");
simulation->createInteraction("yhf -> sosf", "output", "sosf");
simulation->createInteraction("yhf", "output", "yhf -> loif");
simulation->createInteraction("yhf -> loif", "output", "loif");

simulation->createInteraction("bhf", "output", "bhf -> sosf");
simulation->createInteraction("bhf -> sosf", "output", "sosf");
simulation->createInteraction("bhf", "output", "bhf -> loif");
simulation->createInteraction("bhf -> loif", "output", "loif");

simulation->createInteraction("shf", "output", "shf -> sosf");
simulation->createInteraction("shf -> sosf", "output", "sosf");

simulation->createInteraction("lhf", "output", "lhf -> loif");
simulation->createInteraction("lhf -> loif", "output", "loif");


simulation->createInteraction("sosf", "output", "sosf -> ael");
simulation->createInteraction("sosf -> ael", "output", "ael");
simulation->createInteraction("loif", "output", "loif -> ael");
simulation->createInteraction("loif -> ael", "output", "ael");

simulation->createInteraction("loif", "output", "loif -> lht");
simulation->createInteraction("loif -> lht", "output", "lht");
simulation->createInteraction("boif", "output", "boif -> bht");	
simulation->createInteraction("boif -> bht", "output", "bht");
simulation->createInteraction("yoif", "output", "yoif -> yht");
simulation->createInteraction("yoif -> yht", "output", "yht");
simulation->createInteraction("soif", "output", "soif -> sht");
simulation->createInteraction("soif -> sht", "output", "sht");

simulation->createInteraction("lht", "output", "lht -> lhf");
simulation->createInteraction("lht -> lhf", "output", "lhf");
simulation->createInteraction("bht", "output", "bht -> bhf");
simulation->createInteraction("bht -> bhf", "output", "bhf");
simulation->createInteraction("yht", "output", "yht -> yhf");
simulation->createInteraction("yht -> yhf", "output", "yhf");
simulation->createInteraction("sht", "output", "sht -> shf");
simulation->createInteraction("sht -> shf", "output", "shf");
simulation->createInteraction("lht", "output", "lht -> shf");
simulation->createInteraction("lht -> shf", "output", "shf");
simulation->createInteraction("bht", "output", "bht -> yhf");
simulation->createInteraction("bht -> yhf", "output", "yhf");
simulation->createInteraction("yht", "output", "yht -> bhf");
simulation->createInteraction("yht -> bhf", "output", "bhf");
simulation->createInteraction("sht", "output", "sht -> lhf");
simulation->createInteraction("sht -> lhf", "output", "lhf");





//noise
simulation->createInteraction("normal noise ael", "output", "ael");
simulation->createInteraction("normal noise sosf", "output", "sosf");


return simulation;
}

std::shared_ptr<dnf_composer::Simulation> getDynamicNeuralFieldArchitectureNoAnticipation(const std::string& id, const double& deltaT)
{
	using namespace dnf_composer;
	auto simulation = std::make_shared<Simulation>(id, deltaT, 0, 0);

	element::ElementFactory factory;
	element::ElementDimensions dim_params{ 50, 0.5 };
	constexpr bool circularity = true;
	constexpr bool normalization = false;
	constexpr double tau = 100;
	constexpr double resting_level = -5;
	constexpr double x_shift = 0;
	constexpr double steepness = 4;
	constexpr double stimulus_sigma = 3;
	constexpr double stimulus_amplitude = 5;
	constexpr double noise_amplitude = 0.05;

	// Action observation layer
	element::GaussStimulusParameters hand_position_gsp = { stimulus_sigma + 1, 0, 0, circularity, true };
	const auto hand_position_stimulus = factory.createElement(element::GAUSS_STIMULUS,
		{ "hand position stimulus", dim_params }, { hand_position_gsp });
	simulation->addElement(hand_position_stimulus);

	const element::SigmoidFunction aol_af = { x_shift, steepness };
	element::NeuralFieldParameters aol_params = { tau, resting_level, aol_af };
	const auto aol = factory.createElement(element::NEURAL_FIELD,
		{ "aol", dim_params }, { aol_params });
	simulation->addElement(aol);

	element::GaussKernelParameters aol_aol_k_params = { 1, 1.5, 0.0, circularity, normalization };
	const auto aol_aol_k = factory.createElement(element::GAUSS_KERNEL,
		{ "aol -> aol", dim_params }, { aol_aol_k_params });
	simulation->addElement(aol_aol_k);

	const element::NormalNoiseParameters aol_nn_params = { noise_amplitude };
	const auto aol_nn = factory.createElement(element::NORMAL_NOISE,
		{ "normal noise aol", dim_params }, aol_nn_params);
	simulation->addElement(aol_nn);

	simulation->createInteraction("aol", "output", "aol -> aol");
	simulation->createInteraction("aol -> aol", "output", "aol");
	simulation->createInteraction("normal noise aol", "output", "aol");
	simulation->createInteraction("hand position stimulus", "output", "aol");

	// Action simulation layer
	const element::SigmoidFunction asl_af = { x_shift, steepness };
	element::NeuralFieldParameters asl_params = { tau, resting_level, asl_af };
	const auto asl = factory.createElement(element::NEURAL_FIELD,
		{ "asl", dim_params }, { asl_params });
	simulation->addElement(asl);

	element::GaussKernelParameters asl_asl_k_params = { 1, 1.5, 0.0, circularity, normalization };
	const auto asl_asl_k = factory.createElement(element::GAUSS_KERNEL,
		{ "asl -> asl", dim_params }, { asl_asl_k_params });
	simulation->addElement(asl_asl_k);

	element::GaussKernelParameters aol_asl_k_params = { 3.0, 0.755, 0.0, circularity, normalization };
	const auto aol_asl_k = factory.createElement(element::GAUSS_KERNEL,
		{ "aol -> asl", dim_params }, { aol_asl_k_params });
	simulation->addElement(aol_asl_k);

	const element::NormalNoiseParameters asl_nn_params = { noise_amplitude };
	const auto asl_nn = factory.createElement(element::NORMAL_NOISE,
		{ "normal noise asl", dim_params }, asl_nn_params);
	simulation->addElement(asl_nn);

	simulation->createInteraction("asl", "output", "asl -> asl");
	simulation->createInteraction("asl -> asl", "output", "asl");
	simulation->createInteraction("normal noise asl", "output", "asl");
	//simulation->createInteraction("aol", "output", "aol -> asl");
	//simulation->createInteraction("aol -> asl", "output", "asl");

	// Object memory layer
	element::GaussStimulusParameters orl_gsp = { stimulus_sigma, stimulus_amplitude, 12.5, circularity, normalization };
	const auto orl_stimulus_1 = factory.createElement(element::GAUSS_STIMULUS,
		{ "object stimulus 3", dim_params }, { orl_gsp });
	simulation->addElement(orl_stimulus_1);

	orl_gsp = { stimulus_sigma, stimulus_amplitude, 25, circularity, normalization };
	const auto orl_stimulus_2 = factory.createElement(element::GAUSS_STIMULUS,
		{ "object stimulus 2", dim_params }, { orl_gsp });
	simulation->addElement(orl_stimulus_2);

	orl_gsp = { stimulus_sigma, stimulus_amplitude, 37.5, circularity, normalization };
	const auto orl_stimulus_3 = factory.createElement(element::GAUSS_STIMULUS,
		{ "object stimulus 1", dim_params }, { orl_gsp });
	simulation->addElement(orl_stimulus_3);

	element::SigmoidFunction orl_af = { x_shift, steepness };
	element::NeuralFieldParameters orl_params = { tau, resting_level, orl_af };
	const auto orl = factory.createElement(element::NEURAL_FIELD,
		{ "orl", dim_params }, { orl_params });
	simulation->addElement(orl);

	element::MexicanHatKernelParameters orl_orl_k_params = { 1, 2, 0.5, 1.5, 0.0, circularity, normalization };
	const auto orl_orl_k = factory.createElement(element::MEXICAN_HAT_KERNEL,
		{ "orl -> orl", dim_params }, { orl_orl_k_params });
	simulation->addElement(orl_orl_k);

	element::GaussKernelParameters orl_asl_k_params = { 1.9, 0.7, 0.0, circularity, normalization };
	const auto orl_asl_k = factory.createElement(element::GAUSS_KERNEL,
		{ "orl -> asl", dim_params }, { orl_asl_k_params });
	simulation->addElement(orl_asl_k);

	element::NormalNoiseParameters orl_nn_params = { noise_amplitude };
	const auto orl_nn = factory.createElement(element::NORMAL_NOISE,
		{ "normal noise orl", dim_params }, orl_nn_params);
	simulation->addElement(orl_nn);

	//simulation->createInteraction("orl", "output", "orl -> asl");
	//simulation->createInteraction("orl -> asl", "output", "asl");
	simulation->createInteraction("orl", "output", "orl -> orl");
	simulation->createInteraction("orl -> orl", "output", "orl");
	simulation->createInteraction("normal noise orl", "output", "orl");
	simulation->createInteraction("object stimulus 1", "output", "orl");
	simulation->createInteraction("object stimulus 2", "output", "orl");
	simulation->createInteraction("object stimulus 3", "output", "orl");

	// Action execution layer
	element::SigmoidFunction ael_af = { x_shift, steepness };
	element::NeuralFieldParameters ael_params = { tau, resting_level, ael_af };
	const auto ael = factory.createElement(element::NEURAL_FIELD,
		{ "ael", dim_params }, { ael_params });
	simulation->addElement(ael);

	element::GaussKernelParameters asl_ael_k_params = { 4, -2.5, 0.0, circularity, normalization };
	const auto asl_ael_k = factory.createElement(element::GAUSS_KERNEL,
		{ "asl -> ael", dim_params }, { asl_ael_k_params });
	simulation->addElement(asl_ael_k);

	element::GaussKernelParameters ael_ael_k_params = { 2.5, 2.8, -1.1, circularity, normalization };
	const auto ael_ael_k = factory.createElement(element::GAUSS_KERNEL,
		{ "ael -> ael", dim_params }, { ael_ael_k_params });
	simulation->addElement(ael_ael_k);

	element::GaussKernelParameters orl_ael_k_params = { 3, 1.5, 0.0, circularity, normalization };
	const auto orl_ael_k = factory.createElement(element::GAUSS_KERNEL,
		{ "orl -> ael", dim_params }, { orl_ael_k_params });
	simulation->addElement(orl_ael_k);

	element::NormalNoiseParameters ael_nn_params = { noise_amplitude };
	const auto ael_nn = factory.createElement(element::NORMAL_NOISE,
		{ "normal noise ael", dim_params }, ael_nn_params);
	simulation->addElement(ael_nn);

	simulation->createInteraction("ael", "output", "ael -> ael");
	simulation->createInteraction("ael -> ael", "output", "ael");
	simulation->createInteraction("normal noise ael", "output", "ael");
	simulation->createInteraction("asl", "output", "asl -> ael");
	simulation->createInteraction("asl -> ael", "output", "ael");
	simulation->createInteraction("orl -> ael", "output", "ael");
	simulation->createInteraction("orl", "output", "orl -> ael");

	return simulation;
}