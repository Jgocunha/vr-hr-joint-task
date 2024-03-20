
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
		// keep trying to initialize the connection
		while (!client.initialize());

		resetSignals();
		client.startSimulation();

		while (isConnected())
		{
			if (wereSignalsChanged)
				writeSignals();
			readSignals();
			updateData();
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

void CoppeliasimHandler::setSignal(const std::string& signalName, const int signalValue)
{
	client.setIntegerSignal(signalName, signalValue);
	log(dnf_composer::tools::logger::LogLevel::INFO, "Signal " + signalName + " was written as " + std::to_string(signalValue) + '\n');
	//wereSignalsChanged = true;
}

void CoppeliasimHandler::setSignals(const Signals& sign)
{
	wereSignalsChanged = true;
	signals = sign;
}

Signals CoppeliasimHandler::getSignals() const
{
	return signals;
}

SimulationData CoppeliasimHandler::getSimulationData() const
{
	return data;
}

bool CoppeliasimHandler::isConnected() const
{
	return client.isConnected();
}

void CoppeliasimHandler::writeSignals()
{
	client.setIntegerSignal(SignalSignatures::START_SIM, signals.startSim);

	wereSignalsChanged = false;
}

void CoppeliasimHandler::readSignals()
{
	signals.simStarted = client.getIntegerSignal(SignalSignatures::SIM_STARTED);
	signals.objectsCreated = client.getIntegerSignal(SignalSignatures::OBJECTS_CREATED);
	signals.object1 = client.getIntegerSignal(SignalSignatures::OBJECT1_EXISTS);
	signals.object2 = client.getIntegerSignal(SignalSignatures::OBJECT2_EXISTS);
	signals.object3 = client.getIntegerSignal(SignalSignatures::OBJECT3_EXISTS);
	signals.objectGrasped = client.getIntegerSignal(SignalSignatures::OBJECT_GRASPED);
	signals.objectPlaced = client.getIntegerSignal(SignalSignatures::OBJECT_PLACED);
}

void CoppeliasimHandler::updateData()
{
	const int object1Handle = client.getObjectHandle("object_1");
	data.object1Position = client.getObjectPosition(object1Handle);
	const int object2Handle = client.getObjectHandle("object_2");
	data.object2Position = client.getObjectPosition(object2Handle);
	const int object3Handle = client.getObjectHandle("object_3");
	data.object3Position = client.getObjectPosition(object3Handle);
	//const int handHandle = client.getObjectHandle("RightController");
	//data.handPosition = client.getObjectPosition(handHandle);
}

void CoppeliasimHandler::resetSignals() const
{
	client.setIntegerSignal(SignalSignatures::START_SIM, false);
	client.setIntegerSignal(SignalSignatures::SIM_STARTED, false);
	client.setIntegerSignal(SignalSignatures::OBJECTS_CREATED, false);
	client.setIntegerSignal(SignalSignatures::TARGET_OBJECT, 0);
	client.setIntegerSignal(SignalSignatures::OBJECT1_EXISTS, false);
	client.setIntegerSignal(SignalSignatures::OBJECT2_EXISTS, false);
	client.setIntegerSignal(SignalSignatures::OBJECT3_EXISTS, false);
	client.setIntegerSignal(SignalSignatures::OBJECT_GRASPED, false);
	client.setIntegerSignal(SignalSignatures::OBJECT_PLACED, false);
}

