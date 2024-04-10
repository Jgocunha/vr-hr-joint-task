#include "coppeliasim_handler.h"

CoppeliasimHandler::CoppeliasimHandler()
	: incomingSignalsClient("127.0.0.1", 19999),
	outgoingSignalsClient("127.0.0.1", 19998),
	handClient("127.0.0.1", 19995)
{
	incomingSignalsClient.setLogMode(coppeliasim_cpp::LogMode::NO_LOGS);
	outgoingSignalsClient.setLogMode(coppeliasim_cpp::LogMode::NO_LOGS);
	handClient.setLogMode(coppeliasim_cpp::LogMode::NO_LOGS);
}

CoppeliasimHandler::~CoppeliasimHandler()
{
	end();
}

void CoppeliasimHandler::init()
{
	incomingSignalsThread = std::thread(&CoppeliasimHandler::incomingSignalsLoop, this);
	outgoingSignalsThread = std::thread(&CoppeliasimHandler::outgoingSignalsLoop, this);
	handThread = std::thread(&CoppeliasimHandler::readHandPosition, this);
}


void CoppeliasimHandler::incomingSignalsLoop()
{
	
	while (!incomingSignalsClient.initialize());

	incomingSignalsClient.startSimulation();

	resetSignals();

	while (isConnected())
	{
		readSignals();
		//printSignals();
	}
}

void CoppeliasimHandler::outgoingSignalsLoop()
{
	while (!outgoingSignalsClient.initialize());

	while (outgoingSignalsClient.isConnected())
	{
		writeSignals();
	}
}


void CoppeliasimHandler::setSignals(const OutgoingSignals& signals)
{
	outgoingSignals = signals;
}


IncomingSignals CoppeliasimHandler::getSignals() const
{
	return incomingSignals;
}

void CoppeliasimHandler::readHandPosition()
{
	while (!handClient.initialize());

	hand.objectHandle = handClient.getObjectHandle("RightController");

    while (handClient.isConnected())
    {
        coppeliasim_cpp::Pose pos = handClient.getObjectPose(hand.objectHandle);
		hand.pose = { {pos.position.x,
			pos.position.y,
			pos.position.z},
			{pos.orientation.alpha,
			pos.orientation.beta,
			pos.orientation.gamma}
		};
    }
}


Pose CoppeliasimHandler::getHandPose() const
{
	return hand.pose;
}


void CoppeliasimHandler::end()
{
	if (isConnected())
		incomingSignalsClient.stopSimulation();
	incomingSignalsThread.join();
	outgoingSignalsThread.join();
	handThread.join();
}

bool CoppeliasimHandler::isConnected() const
{
	return incomingSignalsClient.isConnected();
}

void CoppeliasimHandler::readSignals()
{
	incomingSignals.simStarted = incomingSignalsClient.getIntegerSignal(IncomingSignals::SIM_STARTED);
	incomingSignals.object1 = incomingSignalsClient.getIntegerSignal(IncomingSignals::OBJECT1_EXISTS);
	incomingSignals.object2 = incomingSignalsClient.getIntegerSignal(IncomingSignals::OBJECT2_EXISTS);
	incomingSignals.object3 = incomingSignalsClient.getIntegerSignal(IncomingSignals::OBJECT3_EXISTS);
	incomingSignals.robotApproaching = incomingSignalsClient.getIntegerSignal(IncomingSignals::ROBOT_APPROACH);
	incomingSignals.robotGraspObj1 = incomingSignalsClient.getIntegerSignal(IncomingSignals::ROBOT_GRASP_OBJ1);
	incomingSignals.robotGraspObj2 = incomingSignalsClient.getIntegerSignal(IncomingSignals::ROBOT_GRASP_OBJ2);
	incomingSignals.robotGraspObj3 = incomingSignalsClient.getIntegerSignal(IncomingSignals::ROBOT_GRASP_OBJ3);
	incomingSignals.robotPlaceObj1 = incomingSignalsClient.getIntegerSignal(IncomingSignals::ROBOT_PLACE_OBJ1);
	incomingSignals.robotPlaceObj2 = incomingSignalsClient.getIntegerSignal(IncomingSignals::ROBOT_PLACE_OBJ2);
	incomingSignals.robotPlaceObj3 = incomingSignalsClient.getIntegerSignal(IncomingSignals::ROBOT_PLACE_OBJ3);
	incomingSignals.humanGraspObj1 = incomingSignalsClient.getIntegerSignal(IncomingSignals::HUMAN_GRASP_OBJ1);
	incomingSignals.humanGraspObj2 = incomingSignalsClient.getIntegerSignal(IncomingSignals::HUMAN_GRASP_OBJ2);
	incomingSignals.humanGraspObj3 = incomingSignalsClient.getIntegerSignal(IncomingSignals::HUMAN_GRASP_OBJ3);
	incomingSignals.humanPlaceObj1 = incomingSignalsClient.getIntegerSignal(IncomingSignals::HUMAN_PLACE_OBJ1);
	incomingSignals.humanPlaceObj2 = incomingSignalsClient.getIntegerSignal(IncomingSignals::HUMAN_PLACE_OBJ2);
	incomingSignals.humanPlaceObj3 = incomingSignalsClient.getIntegerSignal(IncomingSignals::HUMAN_PLACE_OBJ3);
	incomingSignals.canRestart = incomingSignalsClient.getIntegerSignal(IncomingSignals::CAN_RESTART);
	incomingSignals.restart = incomingSignalsClient.getIntegerSignal(IncomingSignals::RESTART);
}

void CoppeliasimHandler::writeSignals() const
{
	outgoingSignalsClient.setIntegerSignal(OutgoingSignals::START_SIM, outgoingSignals.startSim);
	outgoingSignalsClient.setIntegerSignal(OutgoingSignals::TARGET_OBJECT, outgoingSignals.targetObject);
}

void CoppeliasimHandler::resetSignals() const
{
	incomingSignalsClient.setIntegerSignal(OutgoingSignals::START_SIM, 0);
	incomingSignalsClient.setIntegerSignal(OutgoingSignals::TARGET_OBJECT, 0);

	incomingSignalsClient.setIntegerSignal(IncomingSignals::SIM_STARTED, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::OBJECT1_EXISTS, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::OBJECT2_EXISTS, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::OBJECT3_EXISTS, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::ROBOT_APPROACH, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::ROBOT_GRASP_OBJ1, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::ROBOT_GRASP_OBJ2, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::ROBOT_GRASP_OBJ3, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::ROBOT_PLACE_OBJ1, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::ROBOT_PLACE_OBJ2, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::ROBOT_PLACE_OBJ3, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::HUMAN_GRASP_OBJ1, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::HUMAN_GRASP_OBJ2, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::HUMAN_GRASP_OBJ3, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::HUMAN_PLACE_OBJ1, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::HUMAN_PLACE_OBJ2, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::HUMAN_PLACE_OBJ3, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::CAN_RESTART, 0);
	incomingSignalsClient.setIntegerSignal(IncomingSignals::RESTART, 0);
}

void CoppeliasimHandler::printSignals() const
{
	/*std::cout << "Signals:" << std::endl;
	std::cout << "----------------" << std::endl;
	std::cout << "Start Sim: " << signals.startSim << std::endl;
	std::cout << "Sim Started: " << signals.simStarted << std::endl;
	std::cout << "Can Restart: " << signals.canRestart << std::endl;
	std::cout << "Restart: " << signals.restart << std::endl;
	std::cout << "Object 1: " << signals.object1 << std::endl;
	std::cout << "Object 2: " << signals.object2 << std::endl;
	std::cout << "Object 3: " << signals.object3 << std::endl;
	std::cout << "Target Object: " << signals.targetObject << std::endl;
	std::cout << "Robot Approaching: " << signals.robotApproaching << std::endl;
	std::cout << "Robot Grasp Obj 1: " << signals.robotGraspObj1 << std::endl;
	std::cout << "Robot Grasp Obj 2: " << signals.robotGraspObj2 << std::endl;
	std::cout << "Robot Grasp Obj 3: " << signals.robotGraspObj3 << std::endl;
	std::cout << "Robot Place Obj 1: " << signals.robotPlaceObj1 << std::endl;
	std::cout << "Robot Place Obj 2: " << signals.robotPlaceObj2 << std::endl;
	std::cout << "Robot Place Obj 3: " << signals.robotPlaceObj3 << std::endl;
	std::cout << "Human Grasp Obj 1: " << signals.humanGraspObj1 << std::endl;
	std::cout << "Human Grasp Obj 2: " << signals.humanGraspObj2 << std::endl;
	std::cout << "Human Grasp Obj 3: " << signals.humanGraspObj3 << std::endl;
	std::cout << "Human Place Obj 1: " << signals.humanPlaceObj1 << std::endl;
	std::cout << "Human Place Obj 2: " << signals.humanPlaceObj2 << std::endl;
	std::cout << "Human Place Obj 3: " << signals.humanPlaceObj3 << std::endl;
	std::endl(std::cout);*/
}
