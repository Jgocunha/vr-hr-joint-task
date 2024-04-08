
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
		hand.handle = client.getObjectHandle("RightController");
		client.startSimulation();

		while (isConnected())
		{
			readSignals();
			readObjectData();
			writeSignals();
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

bool CoppeliasimHandler::isConnected() const
{
	return client.isConnected();
}

Pose CoppeliasimHandler::getHandPose() const
{
	return Pose{ {hand.pose.position.x, hand.pose.position.y, hand.pose.position.z},
				 {hand.pose.orientation.alpha, hand.pose.orientation.beta, hand.pose.orientation.gamma} };
}

void CoppeliasimHandler::setSignals(const OutgoingSignals& signals_out)
{
	signals.outgoing = signals_out;
}

IncomingSignals CoppeliasimHandler::getSignals() const
{
	return signals.incoming;
}


void CoppeliasimHandler::resetSignals()
{
	signals.clear();
}

void CoppeliasimHandler::readSignals()
{
	signals.incoming.simStarted = client.getIntegerSignal(SignalSignatures::SIM_STARTED);
	signals.incoming.object1 = client.getIntegerSignal(SignalSignatures::OBJECT1_EXISTS);
	signals.incoming.object2 = client.getIntegerSignal(SignalSignatures::OBJECT2_EXISTS);
	signals.incoming.object3 = client.getIntegerSignal(SignalSignatures::OBJECT3_EXISTS);
	signals.incoming.hand_y = client.getFloatSignal(SignalSignatures::HAND_Y);
	signals.incoming.hand_proximity = client.getFloatSignal(SignalSignatures::HAND_PROXIMITY);
	signals.incoming.robotApproaching = client.getIntegerSignal(SignalSignatures::ROBOT_APPROACH);

	signals.incoming.robotGraspObj1 = client.getIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ1);
	signals.incoming.robotGraspObj2 = client.getIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ2);
	signals.incoming.robotGraspObj3 = client.getIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ3);
	signals.incoming.robotPlaceObj1 = client.getIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ1);
	signals.incoming.robotPlaceObj2 = client.getIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ2);
	signals.incoming.robotPlaceObj3 = client.getIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ3);

	signals.incoming.humanGraspObj1 = client.getIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ1);
	signals.incoming.humanGraspObj2 = client.getIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ2);
	signals.incoming.humanGraspObj3 = client.getIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ3);

	signals.incoming.humanPlaceObj1 = client.getIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ1);
	signals.incoming.humanPlaceObj2 = client.getIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ2);
	signals.incoming.humanPlaceObj3 = client.getIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ3);
}

void CoppeliasimHandler::writeSignals() const
{
	client.setIntegerSignal(SignalSignatures::START_SIM, signals.outgoing.startSim);
	client.setIntegerSignal(SignalSignatures::TARGET_OBJECT, signals.outgoing.targetObject);

	//client.setIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ1, signals.incoming.robotGraspObj1);
	//client.setIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ2, signals.incoming.robotGraspObj2);
	//client.setIntegerSignal(SignalSignatures::ROBOT_GRASP_OBJ3, signals.incoming.robotGraspObj3);

	//client.setIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ1, signals.incoming.robotPlaceObj1);
	//client.setIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ2, signals.incoming.robotPlaceObj2);
	//client.setIntegerSignal(SignalSignatures::ROBOT_PLACE_OBJ3, signals.incoming.robotPlaceObj3);

	//client.setIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ1, signals.incoming.humanGraspObj1);
	//client.setIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ2, signals.incoming.humanGraspObj2);
	//client.setIntegerSignal(SignalSignatures::HUMAN_GRASP_OBJ3, signals.incoming.humanGraspObj3);
	//										  									 
	//client.setIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ1, signals.incoming.humanPlaceObj1);
	//client.setIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ2, signals.incoming.humanPlaceObj2);
	//client.setIntegerSignal(SignalSignatures::HUMAN_PLACE_OBJ3, signals.incoming.humanPlaceObj3);
}

void CoppeliasimHandler::printSignals() const
{
	signals.print();
}

void CoppeliasimHandler::readObjectData()
{
	hand.pose = client.getObjectPose(hand.handle);
}
