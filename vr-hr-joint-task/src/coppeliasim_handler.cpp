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

	signals.robotApproaching = signalClient.getIntegerSignal(SignalSignatures::ROBOT_APPROACH);
	signals.robotGraspObj1 = signalClient.getIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ1);
	signals.robotGraspObj2 = signalClient.getIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ2);
	signals.robotGraspObj3 = signalClient.getIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ3);
	signals.robotPlaceObj1 = signalClient.getIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ1);
	signals.robotPlaceObj2 = signalClient.getIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ2);
	signals.robotPlaceObj3 = signalClient.getIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ3);
	signals.humanGraspObj1 = signalClient.getIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ1);
	signals.humanGraspObj2 = signalClient.getIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ2);
	signals.humanGraspObj3 = signalClient.getIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ3);
	signals.humanPlaceObj1 = signalClient.getIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ1);
	signals.humanPlaceObj2 = signalClient.getIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ2);
	signals.humanPlaceObj3 = signalClient.getIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ3);
	signals.canRestart = signalClient.getIntegerSignal(SignalSignatures::CAN_RESTART);
	signals.restart = signalClient.getIntegerSignal(SignalSignatures::RESTART);
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
	signalClient.setIntegerSignal(SignalSignatures::ROBOT_APPROACH, 0);
	signalClient.setIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ1, 0);
	signalClient.setIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ2, 0);
	signalClient.setIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ3, 0);
	signalClient.setIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ1, 0);
	signalClient.setIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ2, 0);
	signalClient.setIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ3, 0);
	signalClient.setIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ1, 0);
	signalClient.setIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ2, 0);
	signalClient.setIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ3, 0);
	signalClient.setIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ1, 0);
	signalClient.setIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ2, 0);
	signalClient.setIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ3, 0);
	signalClient.setIntegerSignal(SignalSignatures::CAN_RESTART, 0);
	signalClient.setIntegerSignal(SignalSignatures::RESTART, 0);
}

void CoppeliasimHandler::printSignals() const
{
	std::cout << "Signals:" << std::endl;
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
	std::endl(std::cout);
}
