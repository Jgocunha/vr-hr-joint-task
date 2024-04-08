#include "coppeliasim_handler.h"

CoppeliasimHandler::CoppeliasimHandler()
	: signalClient("127.0.0.1", 19999),
		handClient("127.0.0.1", 19998)
{
	signalClient.setLogMode(coppeliasim_cpp::LogMode::NO_LOGS);
	handClient.setLogMode(coppeliasim_cpp::LogMode::NO_LOGS);
}

CoppeliasimHandler::~CoppeliasimHandler()
{
	end();
}

void CoppeliasimHandler::init()
{
	signalThread = std::thread(&CoppeliasimHandler::readAndWriteSignals, this);
}


void CoppeliasimHandler::readAndWriteSignals()
{
	
	while (!signalClient.initialize());

	signalClient.startSimulation();

	handThread = std::thread(&CoppeliasimHandler::readHandPosition, this);

	resetSignals();

	while (isConnected())
	{
		readSignals();
		writeSignals();
		printSignals();
	}

	handThread.join();
}

void CoppeliasimHandler::setSignals(bool startSim, int targetObject)
{
	signals.startSim = startSim;
	signals.targetObject = targetObject;
}


Signals CoppeliasimHandler::getSignals() const
{
	return signals;
}

void CoppeliasimHandler::readHandPosition()
{
	while (!handClient.initialize());

	hand.objectHandle = handClient.getObjectHandle("RightController");

    while (handClient.isConnected())
    {
        coppeliasim_cpp::Position pos = handClient.getObjectPosition(hand.objectHandle);
        hand.position = { pos.x, pos.y, pos.z };
    }
}


Position CoppeliasimHandler::getHandPosition() const
{
	return hand.position;
}


void CoppeliasimHandler::end()
{
	if (isConnected())
		signalClient.stopSimulation();
	signalThread.join();
}

bool CoppeliasimHandler::isConnected() const
{
	return signalClient.isConnected();
}

void CoppeliasimHandler::readSignals()
{
	signals.simStarted = signalClient.getIntegerSignal(SignalSignatures::SIM_STARTED);
	signals.object1 = signalClient.getIntegerSignal(SignalSignatures::OBJECT1_EXISTS);
	signals.object2 = signalClient.getIntegerSignal(SignalSignatures::OBJECT2_EXISTS);
	signals.object3 = signalClient.getIntegerSignal(SignalSignatures::OBJECT3_EXISTS);
}

void CoppeliasimHandler::writeSignals() const
{
	signalClient.setIntegerSignal(SignalSignatures::START_SIM, signals.startSim);
	signalClient.setIntegerSignal(SignalSignatures::TARGET_OBJECT, signals.targetObject);
}

void CoppeliasimHandler::resetSignals() const
{
	signalClient.setIntegerSignal(SignalSignatures::START_SIM, 0);
	signalClient.setIntegerSignal(SignalSignatures::SIM_STARTED, 0);
	signalClient.setIntegerSignal(SignalSignatures::TARGET_OBJECT, 0);
	signalClient.setIntegerSignal(SignalSignatures::OBJECT1_EXISTS, 0);
	signalClient.setIntegerSignal(SignalSignatures::OBJECT2_EXISTS, 0);
	signalClient.setIntegerSignal(SignalSignatures::OBJECT3_EXISTS, 0);
}

void CoppeliasimHandler::printSignals() const
{
	std::cout << "Signals:" << std::endl;
	std::cout << "----------------" << std::endl;
	std::cout << "Sim Started: " << signals.simStarted << std::endl;
	std::cout << "Object 1: " << signals.object1 << std::endl;
	std::cout << "Object 2: " << signals.object2 << std::endl;
	std::cout << "Object 3: " << signals.object3 << std::endl;
	std::cout << "Start Sim: " << signals.startSim << std::endl;
	std::cout << "Target Object: " << signals.targetObject << std::endl;
	std::endl(std::cout);
}
