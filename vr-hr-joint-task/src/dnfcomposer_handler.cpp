#include "dnfcomposer_handler.h"


DNFComposerHandler::DNFComposerHandler(const SimulationParameters& simParams)
{
	simulation = std::make_shared<dnf_composer::Simulation>(simParams.identifier, simParams.deltaT, 0, 0);
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

void DNFComposerHandler::setupUserInterface() const
{
	application->activateUserInterfaceWindow(dnf_composer::user_interface::LOG_WINDOW);
	application->activateUserInterfaceWindow(std::make_shared<ExperimentWindow>());
}

