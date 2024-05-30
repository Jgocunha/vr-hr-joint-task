#include "dnf_composer_handler.h"

DnfComposerHandler::DnfComposerHandler(DnfArchitectureType dnf, double deltaT)
	: dnf(dnf)
{
	switch (dnf)
	{
	case DnfArchitectureType::HAND_MOTION:
		simulation = getDynamicNeuralFieldArchitectureHandMotion("dnf arch", deltaT);
		break;
	case DnfArchitectureType::ACTION_LIKELIHOOD:
		simulation = getDynamicNeuralFieldArchitectureActionLikelihood("dnf arch", deltaT);
		break;
	case DnfArchitectureType::NO_ANTICIPATION:
	case DnfArchitectureType::BASELINE:
		simulation = getDynamicNeuralFieldArchitectureNoAnticipation("dnf arch", deltaT);
		break;
	}
	application = std::make_shared<dnf_composer::Application>(simulation);
	setupUserInterface();
}

DnfComposerHandler::~DnfComposerHandler()
{
	end();
}

void DnfComposerHandler::init()
{
	simulationThread = std::thread(&DnfComposerHandler::run, this);
}

void DnfComposerHandler::run()
{
	application->init();
	bool userRequestedExit = false;
	while (!userRequestedExit || !killEverything)
	{
		application->step();
		userRequestedExit = application->hasUIBeenClosed();
	}
	application->close();
	if (simulationThread.joinable())
		simulationThread.join();
}

void DnfComposerHandler::end()
{
	if (simulationThread.joinable())
		simulationThread.join();
}

void DnfComposerHandler::setHandStimulus(const Position& position, bool object1, bool object2, bool object3) const
{
	switch (dnf)
	{
	case DnfArchitectureType::HAND_MOTION:
		setHandStimulusDependingOnHumanHandPosition(position);
		break;
	case DnfArchitectureType::ACTION_LIKELIHOOD:
		setHandStimulusDependingOnHumanActionLikelihood(position, object1, object2, object3);
		break;
	case DnfArchitectureType::NO_ANTICIPATION:
		// Do nothing
		break;
	}
}

int DnfComposerHandler::getTargetObject() const
{
	const auto ael = std::dynamic_pointer_cast<dnf_composer::element::NeuralField>(simulation->getElement("ael"));
	const double centroid = ael->getCentroid();
	if (centroid < 0)
		return 0;

	const int size = ael->getMaxSpatialDimension();

	// Function to calculate the circular distance between two points
	auto circularDistance = [size](double point1, double point2) -> double {
		const double directDistance = std::abs(point1 - point2);
		const double circularDistance = size - directDistance;
		return std::min(directDistance, circularDistance);
		};

	// Calculate distances to the three points
	double distanceToObject1 = circularDistance(centroid, 37.5);
	double distanceToObject2 = circularDistance(centroid, 25);
	double distanceToObject3 = circularDistance(centroid, 12.5);

	// Determine the closest target and return the corresponding value
	const double minDistance = std::min({ distanceToObject1, distanceToObject2, distanceToObject3 });

	if (minDistance == distanceToObject1)
		return 1;
	if (minDistance == distanceToObject2)
		return 2;
	if (minDistance == distanceToObject3)
		return 3;
	return 0;
}

void DnfComposerHandler::setAvailableObjectsInTheWorkspace(bool object1, bool object2, bool object3) const
{
	auto orl_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 1"));
	auto orl_stimulus_parameters = orl_stimulus->getParameters();
	double amplitude = object1 ? 1 : 0;
	dnf_composer::element::GaussStimulusParameters new_params = { orl_stimulus_parameters.sigma, 5*amplitude, orl_stimulus_parameters.position, false, false };
	orl_stimulus->setParameters(new_params);

	orl_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 2"));
	orl_stimulus_parameters = orl_stimulus->getParameters();
	amplitude = object2 ? 1 : 0;
	new_params = { orl_stimulus_parameters.sigma, 5*amplitude, orl_stimulus_parameters.position, false, false };
	orl_stimulus->setParameters(new_params);

	orl_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 3"));
	orl_stimulus_parameters = orl_stimulus->getParameters();
	amplitude = object3 ? 1 : 0;
	new_params = { orl_stimulus_parameters.sigma, 5*amplitude, orl_stimulus_parameters.position, false, false };
	orl_stimulus->setParameters(new_params);
}

void DnfComposerHandler::setHandStimulusDependingOnHumanActionLikelihood(const Position& position, bool object1, bool object2, bool object3) const
{
	static Position handPrevious = position;
	static const Position objPosition1 = { 0.000,  0.125, 0.716 };
	static const Position objPosition2 = { 0.000,  0.000, 0.716 };
	static const Position objPosition3 = { 0.000, -0.125, 0.716 };
	static constexpr double tau = 0.1;
	static constexpr double sigma = 0.05;
	static constexpr double scalar = 5;

	using Clock = std::chrono::high_resolution_clock;
	auto lastTime = Clock::now();

	const auto currentTime = Clock::now();
	const std::chrono::duration<double> elapsed = currentTime - lastTime;
	const double deltaTime = elapsed.count();

	// Check if elapsed time is too small
	if (deltaTime < std::numeric_limits<double>::epsilon())
		return;

	double likelihood_1 = calculateLikelihoodOfHumanAction(position, handPrevious, objPosition1, deltaTime, tau, sigma);
	double likelihood_2 = calculateLikelihoodOfHumanAction(position, handPrevious, objPosition2, deltaTime, tau, sigma);
	double likelihood_3 = calculateLikelihoodOfHumanAction(position, handPrevious, objPosition3, deltaTime, tau, sigma);

	if (!object1)
		likelihood_1 = 0.0;
	if (!object2)
		likelihood_2 = 0.0;
	if (!object3)
		likelihood_3 = 0.0;


	const auto aol_stimulus_1 = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("hand position stimulus 1"));
	const dnf_composer::element::GaussStimulusParameters new_params{ aol_stimulus_1->getParameters().sigma, scalar * likelihood_1, aol_stimulus_1->getParameters().position, false, false };
	aol_stimulus_1->setParameters(new_params);

	const auto aol_stimulus_2 = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("hand position stimulus 2"));
	const dnf_composer::element::GaussStimulusParameters new_params_2{ aol_stimulus_2->getParameters().sigma, scalar * likelihood_2, aol_stimulus_2->getParameters().position, false, false };
	aol_stimulus_2->setParameters(new_params_2);

	const auto aol_stimulus_3 = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("hand position stimulus 3"));
	const dnf_composer::element::GaussStimulusParameters new_params_3{ aol_stimulus_3->getParameters().sigma, scalar * likelihood_3, aol_stimulus_3->getParameters().position, false, false };
	aol_stimulus_3->setParameters(new_params_3);

	handPrevious = position;
	lastTime = currentTime;
}

void DnfComposerHandler::setHandStimulusDependingOnHumanHandPosition(const Position& position) const
{
	const auto aol_stimulus =
		std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("hand position stimulus"));

	const double proximity = calculateHandProximityToObjects(
		calculateHandDistanceToObjects(position));
	const double y = normalizeHandPosition(position.y);

	const dnf_composer::element::GaussStimulusParameters new_params{ aol_stimulus->getParameters().sigma, proximity, y, false, false };
	aol_stimulus->setParameters(new_params);
}

double DnfComposerHandler::calculateHandDistanceToObjects(const Position& position)
{
	// Table center and dimensions
	static constexpr double tableCenterX = 0.0;
	static constexpr double tableCenterZ = 0.641 + 0.08;

	const double distanceX = std::abs(position.x - tableCenterX);
	const double distanceZ = std::abs(position.z - tableCenterZ);
	const double distance = std::sqrt(distanceX * distanceX + distanceZ * distanceZ);

	return distance;
}

double DnfComposerHandler::calculateHandProximityToObjects(double distance)
{
	// Ensure distance is always greater than zero to avoid division by zero
	static constexpr double safeZone = 0.01;
	distance = std::max(distance, safeZone);
	return 1.0 / distance;
}

double DnfComposerHandler::normalizeHandPosition(double handPositionY)
{
	// Define the min and max of the table in Y dimension
	static constexpr double yMin = -0.25;
	static constexpr double yMax = 0.25;
	// Define the min and max of the scale
	static constexpr double scaleMin = 0;
	static constexpr double scaleMax = 50;

	// Normalize posY to the 0-50 scale
	const double normalizedScale = scaleMin + (scaleMax - scaleMin) * (handPositionY - yMin) / (yMax - yMin);

	return normalizedScale;
}

void DnfComposerHandler::setupUserInterface() const
{
	using namespace dnf_composer;
	element::ElementSpatialDimensionParameters dim_params{ 50, 0.5 };

	// Create User Interface windows
	application->addWindow<imgui_kit::LogWindow>();
	application->addWindow<user_interface::FieldMetricsWindow>();

	constexpr int yMax = 10;
	constexpr int yMin = 8;

	// Create a plot for each neural field
	user_interface::PlotParameters aolPlotParameters;
	aolPlotParameters.annotations = { "Action observation layer", "Spatial dimension", "Amplitude" };
	aolPlotParameters.dimensions = { 0, dim_params.x_max, -yMin, yMax + 10, dim_params.d_x };
	auto aolVisualization = createVisualization(simulation);
	aolVisualization->addPlottingData("aol", "activation");
	aolVisualization->addPlottingData("aol", "input");
	aolVisualization->addPlottingData("aol", "output");
	application->addWindow<user_interface::PlotWindow>(aolVisualization, aolPlotParameters);

	user_interface::PlotParameters aslPlotParameters;
	aslPlotParameters.annotations = { "Action simulation layer", "Spatial dimension", "Amplitude" };
	aslPlotParameters.dimensions = { 0, dim_params.x_max, -yMin, yMax, dim_params.d_x };
	auto aslVisualization = createVisualization(simulation);
	aslVisualization->addPlottingData("asl", "activation");
	aslVisualization->addPlottingData("asl", "input");
	aslVisualization->addPlottingData("asl", "output");
	application->addWindow<user_interface::PlotWindow>(aslVisualization, aslPlotParameters);

	user_interface::PlotParameters orlPlotParameters;
	orlPlotParameters.annotations = { "Object representation layer", "Spatial dimension", "Amplitude" };
	orlPlotParameters.dimensions = { 0, dim_params.x_max, -yMin, yMax, dim_params.d_x };
	auto orlVisualization = createVisualization(simulation);
	orlVisualization->addPlottingData("orl", "activation");
	orlVisualization->addPlottingData("orl", "input");
	orlVisualization->addPlottingData("orl", "output");
	application->addWindow<user_interface::PlotWindow>(orlVisualization, orlPlotParameters);

	user_interface::PlotParameters aelPlotParameters;
	aelPlotParameters.annotations = { "Action execution layer", "Spatial dimension", "Amplitude" };
	aelPlotParameters.dimensions = { 0, dim_params.x_max, -yMin - 20, yMax, dim_params.d_x };
	auto aelVisualization = createVisualization(simulation);
	aelVisualization->addPlottingData("ael", "activation");
	aelVisualization->addPlottingData("ael", "input");
	aelVisualization->addPlottingData("ael", "output");
	application->addWindow<user_interface::PlotWindow>(aelVisualization, aelPlotParameters);
}