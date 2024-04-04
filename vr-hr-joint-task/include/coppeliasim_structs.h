#pragma once

#include "client.h"


struct SignalSignatures
{
	static constexpr const char* START_SIM = "startSim";
	static constexpr const char* SIM_STARTED = "simStarted";
	static constexpr const char* TARGET_OBJECT = "targetObject";
	static constexpr const char* OBJECT1_EXISTS = "object1";
	static constexpr const char* OBJECT2_EXISTS = "object2";
	static constexpr const char* OBJECT3_EXISTS = "object3";
	static constexpr const char* HAND_Y = "hand_y";
	static constexpr const char* HAND_PROXIMITY = "hand_proximity";

	static constexpr const char* ROBOT_APPROACH = "robotApproaching";

	static constexpr const char* ROBOT_GRASP_OBJ1 = "robotGraspObj1";
	static constexpr const char* ROBOT_GRASP_OBJ2 = "robotGraspObj2";
	static constexpr const char* ROBOT_GRASP_OBJ3 = "robotGraspObj3";
	
	static constexpr const char* ROBOT_PLACE_OBJ1 = "robotPlaceObj1";
	static constexpr const char* ROBOT_PLACE_OBJ2 = "robotPlaceObj2";
	static constexpr const char* ROBOT_PLACE_OBJ3 = "robotPlaceObj3";
	
	static constexpr const char* HUMAN_GRASP_OBJ1 = "humanGraspObj1";
	static constexpr const char* HUMAN_GRASP_OBJ2 = "humanGraspObj2";
	static constexpr const char* HUMAN_GRASP_OBJ3 = "humanGraspObj3";
	
	static constexpr const char* HUMAN_PLACE_OBJ1 = "humanPlaceObj1";
	static constexpr const char* HUMAN_PLACE_OBJ2 = "humanPlaceObj2";
	static constexpr const char* HUMAN_PLACE_OBJ3 = "humanPlaceObj3";
};

struct IncomingSignals
{
	bool simStarted;
	bool object1;
	bool object2;
	bool object3;
	float hand_y;
	float hand_proximity;

	bool robotApproaching;

	bool robotGraspObj1;
	bool robotGraspObj2;
	bool robotGraspObj3;
	
	bool robotPlaceObj1;
	bool robotPlaceObj2;
	bool robotPlaceObj3;
	
	bool humanGraspObj1;
	bool humanGraspObj2;
	bool humanGraspObj3;
	
	bool humanPlaceObj1;
	bool humanPlaceObj2;
	bool humanPlaceObj3;

	IncomingSignals()
		: simStarted(false)
		, object1(false)
		, object2(false)
		, object3(false)
		,hand_y(0.0f)
		,hand_proximity(0.0f)

		,robotApproaching(false)

		, robotGraspObj1(false)
		, robotGraspObj2(false)
		, robotGraspObj3(false)
		
		, robotPlaceObj1(false)
		, robotPlaceObj2(false)
		, robotPlaceObj3(false)
		
		, humanGraspObj1(false)
		, humanGraspObj2(false)
		, humanGraspObj3(false)
		
		, humanPlaceObj1(false)
		, humanPlaceObj2(false)
		, humanPlaceObj3(false)
	{
	}

	void print() const
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Incoming signals:");
		log(dnf_composer::tools::logger::LogLevel::INFO, "Sim started: " + std::to_string(simStarted) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Object 1: " + std::to_string(object1) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Object 2: " + std::to_string(object2) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Object 3: " + std::to_string(object3) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Hand y: " + std::to_string(hand_y) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Hand proximity: " + std::to_string(hand_proximity) + '\n');

		log(dnf_composer::tools::logger::LogLevel::INFO, "Robot approaching: " + std::to_string(robotApproaching) + '\n');
		
		log(dnf_composer::tools::logger::LogLevel::INFO, "Robot grasp obj1: " + std::to_string(robotGraspObj1) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Robot grasp obj2: " + std::to_string(robotGraspObj2) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Robot grasp obj3: " + std::to_string(robotGraspObj3) + '\n');

		log(dnf_composer::tools::logger::LogLevel::INFO, "Robot place obj1: " + std::to_string(robotPlaceObj1) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Robot place obj2: " + std::to_string(robotPlaceObj2) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Robot place obj3: " + std::to_string(robotPlaceObj3) + '\n');
		
		log(dnf_composer::tools::logger::LogLevel::INFO, "Human grasp obj1: " + std::to_string(humanGraspObj1) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Human grasp obj2: " + std::to_string(humanGraspObj2)+ '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Human grasp obj3: " + std::to_string(humanGraspObj3)+ '\n');
		
		log(dnf_composer::tools::logger::LogLevel::INFO, "Human place obj1: " + std::to_string(humanPlaceObj1)+ '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Human place obj2: " + std::to_string(humanPlaceObj2)+ '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Human place obj3: " + std::to_string(humanPlaceObj3)+ '\n');
	}
};

struct OutgoingSignals
{
	bool startSim;
	int targetObject;

	OutgoingSignals()
		: startSim(false)
		, targetObject(0)
	{
	}

	void print() const
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Outgoing signals:");
		log(dnf_composer::tools::logger::LogLevel::INFO, "Start sim: " + std::to_string(startSim) + '\n');
		log(dnf_composer::tools::logger::LogLevel::INFO, "Target object: " + std::to_string(targetObject) + '\n');
	}
};

struct Signals
{
	IncomingSignals incoming;
	OutgoingSignals outgoing;

	Signals()
		: incoming()
		, outgoing()
	{
	}

	void print() const
	{
		incoming.print();
		outgoing.print();
	}

	void clear()
	{
		incoming = IncomingSignals();
		outgoing = OutgoingSignals();
	}
};

struct Object
{
	int handle;
	coppeliasim_cpp::Pose pose;

	Object()
		: handle(-1)
	{}

	Object(int handle)
		: handle(handle)
	{}
};