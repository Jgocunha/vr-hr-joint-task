#include "dnfcomposer_handler.h"


DNFComposerHandler::DNFComposerHandler(const SimulationParameters& simParams)
	: experimentWindow(std::make_shared<ExperimentWindow>())
{
	simulation = getDynamicNeuralFieldArchitecture(simParams.identifier, simParams.deltaT);
	application = std::make_shared<dnf_composer::Application>(simulation);
	setupUserInterface();
}

DNFComposerHandler::~DNFComposerHandler()
{
	close();
}

void DNFComposerHandler::init()
{
	log(dnf_composer::tools::logger::LogLevel::INFO, "DNFComposer Handler: Thread will start.\n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
	dnfcomposerThread = std::thread(&DNFComposerHandler::run, this);
}

void DNFComposerHandler::run() const
{
	try
	{
		application->init();

		bool userRequestClose = false;
		while (!userRequestClose)
		{
			application->step();
			userRequestClose = application->getCloseUI();
			Sleep(10);
		}
		application->close();
	}
	catch (const dnf_composer::Exception& ex)
	{
		const std::string errorMessage = "Exception: " + std::string(ex.what()) + " ErrorCode: " + std::to_string(static_cast<int>(ex.getErrorCode())) + ". \n";
		log(dnf_composer::tools::logger::LogLevel::FATAL, errorMessage, dnf_composer::tools::logger::LogOutputMode::CONSOLE);
	}
	catch (const std::exception& ex)
	{
		log(dnf_composer::tools::logger::LogLevel::FATAL, "Exception caught: " + std::string(ex.what()) + ". \n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
	}
	catch (...)
	{
		log(dnf_composer::tools::logger::LogLevel::FATAL, "Unknown exception occurred. \n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
	}
}

void DNFComposerHandler::close()
{
	dnfcomposerThread.join();
	log(dnf_composer::tools::logger::LogLevel::INFO, "DNFComposer Handler: Thread has finished its execution.\n");
}

void DNFComposerHandler::setHandStimulus(const double& hand_y, const double& hand_proximity) const
{
	const auto aol_stimulus = 
		std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("hand position stimulus"));
	const dnf_composer::element::GaussStimulusParameters new_params{aol_stimulus->getParameters().sigma, hand_proximity, hand_y, false, false };
	aol_stimulus->setParameters(new_params);
}

int DNFComposerHandler::getTargetObject() const
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

	// 0 - 3 // 30 - 2 // 60 - 1
	if (minDistance == distanceToObject1)
		return 1;
	if (minDistance == distanceToObject2)
		return 2;
	if (minDistance == distanceToObject3)
		return 3;
	return 0;
}

void DNFComposerHandler::addTargetObject(int objectIndex) const
{
	if(objectIndex == 1)
	{
		const auto orl_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 1"));
		const auto orl_stimulus_parameters = orl_stimulus->getParameters();
		const dnf_composer::element::GaussStimulusParameters new_params = { orl_stimulus_parameters.sigma, 5, orl_stimulus_parameters.position, false, false};
		orl_stimulus->setParameters(new_params);
	}
	if (objectIndex == 2)
	{
		const auto orl_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 2"));
		const auto orl_stimulus_parameters = orl_stimulus->getParameters();
		const dnf_composer::element::GaussStimulusParameters new_params = { orl_stimulus_parameters.sigma, 5, orl_stimulus_parameters.position, false, false };
		orl_stimulus->setParameters(new_params);
	}
	if(objectIndex == 3)
	{
		const auto orl_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 3"));
		const auto orl_stimulus_parameters = orl_stimulus->getParameters();
		const dnf_composer::element::GaussStimulusParameters new_params = { orl_stimulus_parameters.sigma, 5, orl_stimulus_parameters.position, false, false };
		orl_stimulus->setParameters(new_params);
	}
}

void DNFComposerHandler::removeTargetObject(int objectIndex) const
{
	auto orl_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 1"));
	auto orl_stimulus_parameters = orl_stimulus->getParameters();
	if(objectIndex == 2)
	{
		orl_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 2"));
		orl_stimulus_parameters = orl_stimulus->getParameters();
	}
	else if(objectIndex == 3)
	{
		orl_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 3"));
		orl_stimulus_parameters = orl_stimulus->getParameters();
	}

	const dnf_composer::element::GaussStimulusParameters new_params = { orl_stimulus_parameters.sigma, 0, orl_stimulus_parameters.position, false, false };
	orl_stimulus->setParameters(new_params);
}

void DNFComposerHandler::setupUserInterface() const
{
	using namespace dnf_composer;
	element::ElementSpatialDimensionParameters dim_params{ 50, 0.5 };

	// Create User Interface windows
	//application->activateUserInterfaceWindow(user_interface::SIMULATION_WINDOW);
	application->activateUserInterfaceWindow(user_interface::LOG_WINDOW);
	//application->activateUserInterfaceWindow(user_interface::ELEMENT_WINDOW);
	//application->activateUserInterfaceWindow(user_interface::MONITORING_WINDOW);

	constexpr int yMax = 10;
	constexpr int yMin = 8;

	// Create a plot for each neural field
	user_interface::PlotParameters aolPlotParameters;
	aolPlotParameters.annotations = { "Action observation layer", "Spatial dimension", "Amplitude" };
	aolPlotParameters.dimensions = { 0, dim_params.x_max, -yMin, yMax+2, dim_params.d_x };
	aolPlotParameters.renderDataSelector = false;
	const auto aolPlotWindow = std::make_shared<user_interface::PlotWindow>(simulation, aolPlotParameters);
	aolPlotWindow->addPlottingData("aol", "activation");
	aolPlotWindow->addPlottingData("aol", "input");
	aolPlotWindow->addPlottingData("aol", "output");
	application->activateUserInterfaceWindow(aolPlotWindow);

	user_interface::PlotParameters aslPlotParameters;
	aslPlotParameters.annotations = { "Action simulation layer", "Spatial dimension", "Amplitude" };
	aslPlotParameters.dimensions = { 0, dim_params.x_max, -yMin, yMax, dim_params.d_x };
	aslPlotParameters.renderDataSelector = false;
	const auto aslPlotWindow = std::make_shared<user_interface::PlotWindow>(simulation, aslPlotParameters);
	aslPlotWindow->addPlottingData("asl", "activation");
	aslPlotWindow->addPlottingData("asl", "input");
	aslPlotWindow->addPlottingData("asl", "output");
	application->activateUserInterfaceWindow(aslPlotWindow);

	user_interface::PlotParameters orlPlotParameters;
	orlPlotParameters.annotations = { "Object representation layer", "Spatial dimension", "Amplitude" };
	orlPlotParameters.dimensions = { 0, dim_params.x_max, -yMin, yMax, dim_params.d_x };
	orlPlotParameters.renderDataSelector = false;
	const auto orlPlotWindow = std::make_shared<user_interface::PlotWindow>(simulation, orlPlotParameters);
	orlPlotWindow->addPlottingData("orl", "activation");
	orlPlotWindow->addPlottingData("orl", "input");
	orlPlotWindow->addPlottingData("orl", "output");
	application->activateUserInterfaceWindow(orlPlotWindow);

	user_interface::PlotParameters aelPlotParameters;
	aelPlotParameters.annotations = { "Action execution layer", "Spatial dimension", "Amplitude" };
	aelPlotParameters.dimensions = { 0, dim_params.x_max, -yMin-20, yMax, dim_params.d_x };
	aelPlotParameters.renderDataSelector = false;
	const auto aelPlotWindow = std::make_shared<user_interface::PlotWindow>(simulation, aelPlotParameters);
	aelPlotWindow->addPlottingData("ael", "activation");
	aelPlotWindow->addPlottingData("ael", "input");
	aelPlotWindow->addPlottingData("ael", "output");
	application->activateUserInterfaceWindow(aelPlotWindow);
}