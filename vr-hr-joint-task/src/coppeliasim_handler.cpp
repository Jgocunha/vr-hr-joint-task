
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
			//if (wereSignalsChanged)
				//writeSignals();
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
	//log(dnf_composer::tools::logger::LogLevel::INFO, "Signal " + signalName + " was written as " + std::to_string(signalValue) + '\n');
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

Data CoppeliasimHandler::getSimulationData() const
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

//void CoppeliasimHandler::readSignals()
//{
//	// Define how many times to retry reading and the delay between retries (in milliseconds)
//	const int retryCount = 5;
//	const int retryDelayMs = 3;
//
//	// Temporary variables to store signal values during confirmation process
//	int tempSimStarted, tempObjectsCreated, tempObject1, tempObject2, tempObject3, tempObjectGrasped, tempObjectPlaced;
//
//	// A lambda function to read a signal multiple times and check for consistency
//	auto readSignalWithConfirmation = [this, retryCount, retryDelayMs](const std::string& signalName) -> int {
//		int lastValue = client.getIntegerSignal(signalName);
//		bool valueConfirmed = false;
//
//		for (int i = 1; i < retryCount; ++i) {
//			Sleep(retryDelayMs); // Wait a bit before the next read
//			int newValue = client.getIntegerSignal(signalName);
//			if (newValue == lastValue) {
//				valueConfirmed = true;
//				break; // The value is consistent, proceed
//			}
//			lastValue = newValue; // Update the last value for the next iteration
//		}
//
//		if (valueConfirmed) {
//			return lastValue; // Return the confirmed value
//		}
//		else {
//			// Handle the case where values were not consistent
//			// For now, we return the last read value, but you might want to handle this differently
//			return lastValue;
//		}
//	};
//
//	// Use the lambda function to read and confirm each signal
//	signals.simStarted = readSignalWithConfirmation(SignalSignatures::SIM_STARTED);
//	signals.objectsCreated = readSignalWithConfirmation(SignalSignatures::OBJECTS_CREATED);
//	signals.object1 = readSignalWithConfirmation(SignalSignatures::OBJECT1_EXISTS);
//	signals.object2 = readSignalWithConfirmation(SignalSignatures::OBJECT2_EXISTS);
//	signals.object3 = readSignalWithConfirmation(SignalSignatures::OBJECT3_EXISTS);
//	signals.objectGrasped = readSignalWithConfirmation(SignalSignatures::OBJECT_GRASPED);
//	signals.objectPlaced = readSignalWithConfirmation(SignalSignatures::OBJECT_PLACED);
//}



void CoppeliasimHandler::updateData()
{
	const int handHandle = client.getObjectHandle("RightController");
	data.handPosition = client.getObjectPosition(handHandle);
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

