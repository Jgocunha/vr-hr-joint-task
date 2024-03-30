
#include "coppeliasim_handler.h"



CoppeliasimHandler::CoppeliasimHandler()
{
	client.setLogMode(coppeliasim_cpp::LogMode::NO_LOGS);
}

CoppeliasimHandler::~CoppeliasimHandler()
{
	close();
}

void CoppeliasimHandler::init()
{
	log(dnf_composer::tools::logger::LogLevel::INFO, "Coppeliasim Handler: Thread will start.\n");
	coppeliasimThread = std::thread(&CoppeliasimHandler::run, this);
}

void CoppeliasimHandler::run()
{
	try
	{
		while (!client.initialize());

		resetSignals();
		handHandle = client.getObjectHandle("RightController");
		client.startSimulation();

		while (isConnected())
		{
			if (wereSignalsChanged)
				writeSignals();
			readSignals();
			Sleep(10);
		}

		resetSignals();
		close();
	}
	catch (const std::exception& e)
	{
		log(dnf_composer::tools::logger::LogLevel::FATAL, "Coppeliasim Handler: " + std::string(e.what()) + '\n', dnf_composer::tools::logger::LogOutputMode::CONSOLE);
	}
	catch (...)
	{
		log(dnf_composer::tools::logger::LogLevel::FATAL, "Coppeliasim Handler: An unexpected error occurred.\n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
	}
}

void CoppeliasimHandler::close()
{
	if(isConnected())
		client.stopSimulation();
	coppeliasimThread.join();
	log(dnf_composer::tools::logger::LogLevel::INFO, "Coppeliasim Handler: Thread has finished its execution.\n");
}

void CoppeliasimHandler::setSignals(const OutgoingSignals& signals)
{
	signals_out = signals;
	wereSignalsChanged = true;
}


IncomingSignals CoppeliasimHandler::getSignals() const
{
	return signals_in;
}

bool CoppeliasimHandler::isConnected() const
{
	return client.isConnected();
}

void CoppeliasimHandler::resetSignals() const
{
	client.setIntegerSignal(SignalSignatures::START_SIM, true);
	client.setIntegerSignal(SignalSignatures::SIM_STARTED, false);
	client.setIntegerSignal(SignalSignatures::TARGET_OBJECT, 0);
	client.setIntegerSignal(SignalSignatures::OBJECT1_EXISTS, false);
	client.setIntegerSignal(SignalSignatures::OBJECT2_EXISTS, false);
	client.setIntegerSignal(SignalSignatures::OBJECT3_EXISTS, false);
	client.setIntegerSignal(SignalSignatures::OBJECT_GRASPED, false);
	client.setIntegerSignal(SignalSignatures::OBJECT_PLACED, false);
	client.setFloatSignal(SignalSignatures::HAND_PROXIMITY, 0.00f);
	client.setFloatSignal(SignalSignatures::HAND_Y, 0.00f);
}

void CoppeliasimHandler::readSignals()
{
	signals_in.simStarted = client.getIntegerSignal(SignalSignatures::SIM_STARTED);
	signals_in.object1 = client.getIntegerSignal(SignalSignatures::OBJECT1_EXISTS);
	signals_in.object2 = client.getIntegerSignal(SignalSignatures::OBJECT2_EXISTS);
	signals_in.object3 = client.getIntegerSignal(SignalSignatures::OBJECT3_EXISTS);
	signals_in.objectGrasped = client.getIntegerSignal(SignalSignatures::OBJECT_GRASPED);
	signals_in.objectPlaced = client.getIntegerSignal(SignalSignatures::OBJECT_PLACED);
	signals_in.hand_proximity = client.getFloatSignal(SignalSignatures::HAND_PROXIMITY);
	signals_in.hand_y = client.getFloatSignal(SignalSignatures::HAND_Y);
}

void CoppeliasimHandler::writeSignals()
{
	client.setIntegerSignal(SignalSignatures::START_SIM, signals_out.startSim);
	client.setIntegerSignal(SignalSignatures::TARGET_OBJECT, signals_out.targetObject);

	wereSignalsChanged = false;
}

void CoppeliasimHandler::printSignals() const
{
	log(dnf_composer::tools::logger::LogLevel::INFO, "Reading incoming signals\n");

	std::string msg = "Signal SIM_STARTED : " + std::to_string(signals_in.simStarted) + '\n';
	log(dnf_composer::tools::logger::LogLevel::INFO, msg);
	msg = "Signal OBJ_1 : " + std::to_string(signals_in.object1) + '\n';
	log(dnf_composer::tools::logger::LogLevel::INFO, msg);
	msg = "Signal OBJ_2 : " + std::to_string(signals_in.object2) + '\n';
	log(dnf_composer::tools::logger::LogLevel::INFO, msg);
	msg = "Signal OBJ_3 : " + std::to_string(signals_in.object3) + '\n';
	log(dnf_composer::tools::logger::LogLevel::INFO, msg);
	msg = "Signal OBJ_GRASPED : " + std::to_string(signals_in.objectGrasped) + '\n';
	log(dnf_composer::tools::logger::LogLevel::INFO, msg);
	msg = "Signal OBJ_PLACED : " + std::to_string(signals_in.objectPlaced) + '\n';
	log(dnf_composer::tools::logger::LogLevel::INFO, msg);
	msg = "Signal HAND_PROX : " + std::to_string(signals_in.hand_proximity) + '\n';
	log(dnf_composer::tools::logger::LogLevel::INFO, msg);
	msg = "Signal HAND_Y : " + std::to_string(signals_in.hand_y) + '\n';
	log(dnf_composer::tools::logger::LogLevel::INFO, msg);

	log(dnf_composer::tools::logger::LogLevel::INFO, "\n\n");

	log(dnf_composer::tools::logger::LogLevel::INFO, "Writing outgoing signals\n");

	msg = "Signal START_SIM : " + std::to_string(signals_out.startSim) + '\n';
	log(dnf_composer::tools::logger::LogLevel::INFO, msg);
	msg = "Signal TARGET_OBJECT : " + std::to_string(signals_out.targetObject) + '\n';
	log(dnf_composer::tools::logger::LogLevel::INFO, msg);

	log(dnf_composer::tools::logger::LogLevel::INFO, "\n\n");
}
