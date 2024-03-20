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

void DNFComposerHandler::run()
{
	try
	{
		application->init();

		bool userRequestClose = false;
		while (!userRequestClose)
		{
			application->step();
			userRequestClose = application->getCloseUI();
			Sleep(2);
		}
		application->close();
	}
	catch (const dnf_composer::Exception& ex)
	{
		const std::string errorMessage = "Exception: " + std::string(ex.what()) + " ErrorCode: " + std::to_string(static_cast<int>(ex.getErrorCode())) + ". \n";
		log(dnf_composer::tools::logger::LogLevel::FATAL, errorMessage, dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		//return static_cast<int>(ex.getErrorCode());
	}
	catch (const std::exception& ex)
	{
		log(dnf_composer::tools::logger::LogLevel::FATAL, "Exception caught: " + std::string(ex.what()) + ". \n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		//return 1;
	}
	catch (...)
	{
		log(dnf_composer::tools::logger::LogLevel::FATAL, "Unknown exception occurred. \n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		//return 1;
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
	//const double amplitude = calculateClosenessToObjects(calculateDistanceToObjects(handPosition));
	// double position = normalizeHandPosition(handPosition.y);
	const dnf_composer::element::GaussStimulusParameters new_params{aol_stimulus->getParameters().sigma, hand_proximity, hand_y };
	aol_stimulus->setParameters(new_params);
}

int DNFComposerHandler::getTargetObject() const
{
	const auto ael = std::dynamic_pointer_cast<dnf_composer::element::NeuralField>(simulation->getElement("ael"));
	const double centroid = ael->getCentroid();
	const int size = ael->getMaxSpatialDimension();

	// Function to calculate the circular distance between two points
	auto circularDistance = [size](double point1, double point2) -> double {
		const double directDistance = std::abs(point1 - point2);
		const double circularDistance = size - directDistance;
		return std::min(directDistance, circularDistance);
		};

	// Calculate distances to the three points
	double distanceTo30 = circularDistance(centroid, 30);
	double distanceTo60 = circularDistance(centroid, 60);
	double distanceTo0 = circularDistance(centroid, 0); // This also works for distance to 100 due to circularity

	// Determine the closest target and return the corresponding value
	const double minDistance = std::min({ distanceTo30, distanceTo60, distanceTo0 });

	// 0 - 3 // 30 - 2 // 60 - 1
	if (minDistance == distanceTo30)
		return 2;
	if (minDistance == distanceTo60) 
		return 1;
	return 3;
}

void DNFComposerHandler::addTargetObject(int objectIndex) const
{
	if(objectIndex == 1)
	{
		auto oml_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 1"));
		const dnf_composer::element::GaussStimulusParameters new_params = { 3, 5, 60 };
		oml_stimulus->setParameters(new_params);
	}
	if (objectIndex == 2)
	{
		auto oml_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 2"));
		const dnf_composer::element::GaussStimulusParameters new_params = { 3, 5, 30 };
		oml_stimulus->setParameters(new_params);
	}
	if(objectIndex == 3)
	{
		auto oml_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 3"));
		const dnf_composer::element::GaussStimulusParameters new_params = { 3, 5, 0 };
		oml_stimulus->setParameters(new_params);
	}
}

void DNFComposerHandler::removeTargetObject(int objectIndex) const
{
	auto oml_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 1"));
	if(objectIndex == 2)
		oml_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 2"));
	else if(objectIndex == 3)
		oml_stimulus = std::dynamic_pointer_cast<dnf_composer::element::GaussStimulus>(simulation->getElement("object stimulus 3"));

	const dnf_composer::element::GaussStimulusParameters new_params = { 1, 0, 0 };
	oml_stimulus->setParameters(new_params);
}

void DNFComposerHandler::setupUserInterface() const
{
	using namespace dnf_composer;
	element::ElementSpatialDimensionParameters dim_params{ 90, 1.0 };

	// Create User Interface windows
	//application->activateUserInterfaceWindow(user_interface::SIMULATION_WINDOW);
	application->activateUserInterfaceWindow(user_interface::LOG_WINDOW);
	application->activateUserInterfaceWindow(user_interface::ELEMENT_WINDOW);
	//application->activateUserInterfaceWindow(user_interface::MONITORING_WINDOW);

	constexpr int yMax = 10;
	constexpr int yMin = 8;

	// Create a plot for each neural field
	user_interface::PlotParameters aolPlotParameters;
	aolPlotParameters.annotations = { "Action observation layer", "Spatial dimension", "Amplitude" };
	aolPlotParameters.dimensions = { 0, dim_params.x_max, -yMin, yMax, dim_params.d_x };
	const auto aolPlotWindow = std::make_shared<user_interface::PlotWindow>(simulation, aolPlotParameters);
	aolPlotWindow->addPlottingData("aol", "activation");
	aolPlotWindow->addPlottingData("aol", "input");
	aolPlotWindow->addPlottingData("aol", "output");
	application->activateUserInterfaceWindow(aolPlotWindow);

	user_interface::PlotParameters aslPlotParameters;
	aslPlotParameters.annotations = { "Action simulation layer", "Spatial dimension", "Amplitude" };
	aslPlotParameters.dimensions = { 0, dim_params.x_max, -15, yMax, dim_params.d_x };
	const auto aslPlotWindow = std::make_shared<user_interface::PlotWindow>(simulation, aslPlotParameters);
	aslPlotWindow->addPlottingData("asl", "activation");
	aslPlotWindow->addPlottingData("asl", "input");
	aslPlotWindow->addPlottingData("asl", "output");
	application->activateUserInterfaceWindow(aslPlotWindow);

	user_interface::PlotParameters omlPlotParameters;
	omlPlotParameters.annotations = { "Object memory layer", "Spatial dimension", "Amplitude" };
	omlPlotParameters.dimensions = { 0, dim_params.x_max, -yMin, yMax, dim_params.d_x };
	const auto omlPlotWindow = std::make_shared<user_interface::PlotWindow>(simulation, omlPlotParameters);
	omlPlotWindow->addPlottingData("oml", "activation");
	omlPlotWindow->addPlottingData("oml", "input");
	omlPlotWindow->addPlottingData("oml", "output");
	application->activateUserInterfaceWindow(omlPlotWindow);

	user_interface::PlotParameters aelPlotParameters;
	aelPlotParameters.annotations = { "Action execution layer", "Spatial dimension", "Amplitude" };
	aelPlotParameters.dimensions = { 0, dim_params.x_max, -yMin, yMax, dim_params.d_x };
	const auto aelPlotWindow = std::make_shared<user_interface::PlotWindow>(simulation, aelPlotParameters);
	aelPlotWindow->addPlottingData("ael", "activation");
	aelPlotWindow->addPlottingData("ael", "input");
	aelPlotWindow->addPlottingData("ael", "output");
	application->activateUserInterfaceWindow(aelPlotWindow);
}