
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

		handHandle = client.getObjectHandle("RightController");

		while (isConnected())
		{
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

void CoppeliasimHandler::setSignal(const std::string& signalName, const int signalValue)
{
	client.setIntegerSignal(signalName, signalValue);
	//log(dnf_composer::tools::logger::LogLevel::INFO, "Signal " + signalName + " was written as " + std::to_string(signalValue) + '\n');
}

Signals CoppeliasimHandler::getSignals() const
{
	return signals;
}

bool CoppeliasimHandler::isConnected() const
{
	return client.isConnected();
}

void CoppeliasimHandler::readSignals()
{
	signals.simStarted = client.getIntegerSignal(SignalSignatures::SIM_STARTED);
	signals.object1 = client.getIntegerSignal(SignalSignatures::OBJECT1_EXISTS);
	signals.object2 = client.getIntegerSignal(SignalSignatures::OBJECT2_EXISTS);
	signals.object3 = client.getIntegerSignal(SignalSignatures::OBJECT3_EXISTS);
	signals.objectGrasped = client.getIntegerSignal(SignalSignatures::OBJECT_GRASPED);
	signals.objectPlaced = client.getIntegerSignal(SignalSignatures::OBJECT_PLACED);
	signals.hand_proximity = client.getFloatSignal(SignalSignatures::HAND_PROXIMITY);
	signals.hand_y = client.getFloatSignal(SignalSignatures::HAND_Y);
}

void CoppeliasimHandler::resetSignals() const
{
	client.setIntegerSignal(SignalSignatures::START_SIM, false);
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

bool CoppeliasimHandler::hasSignalMajorityValue(const std::string& signalName, int requiredValue, int sampleSize) const
{
	int count = 0;
	for (int i = 0; i < sampleSize; ++i)
	{
		int signalValue = client.getIntegerSignal(signalName);
		if (signalValue == requiredValue)
		{
			++count;
		}
	}
	// Check if the majority of the samples have the required value
	return count > sampleSize / 2;
}

