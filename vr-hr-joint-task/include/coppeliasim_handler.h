#pragma once

#include <thread>
#include <client.h>

#include "misc.h"


struct HumanHand
{
	int objectHandle;
	Pose pose;

	HumanHand(int objectHandle = 0, const Pose& pose = { {0,0,0},
		{0,0,0} })
	: objectHandle(objectHandle), pose(pose)
	{}
};

struct IncomingSignals
{
	static constexpr const char* SIM_STARTED = "simStarted";
	static constexpr const char* OBJECT1_EXISTS = "object1";
	static constexpr const char* OBJECT2_EXISTS = "object2";
	static constexpr const char* OBJECT3_EXISTS = "object3";
	static constexpr const char* OBJECT4_EXISTS = "object4";
	static constexpr const char* OBJECT5_EXISTS = "object5";
	static constexpr const char* OBJECT6_EXISTS = "object6";
	static constexpr const char* OBJECT7_EXISTS = "object7";
	static constexpr const char* OBJECT8_EXISTS = "object8";
	static constexpr const char* ROBOT_APPROACH = "robotApproaching";
	static constexpr const char* ROBOT_GRASP = "robotGrasping";
	static constexpr const char* ROBOT_GRASP_OBJ1 = "robotGraspObj1";
	static constexpr const char* ROBOT_GRASP_OBJ2 = "robotGraspObj2";
	static constexpr const char* ROBOT_GRASP_OBJ3 = "robotGraspObj3";
	static constexpr const char* ROBOT_GRASP_OBJ4 = "robotGraspObj4";
	static constexpr const char* ROBOT_GRASP_OBJ5 = "robotGraspObj5";
	static constexpr const char* ROBOT_GRASP_OBJ6 = "robotGraspObj6";
	static constexpr const char* ROBOT_GRASP_OBJ7 = "robotGraspObj7";
	static constexpr const char* ROBOT_GRASP_OBJ8 = "robotGraspObj8";

	static constexpr const char* ROBOT_PLACE_OBJ1 = "robotPlaceObj1";
	static constexpr const char* ROBOT_PLACE_OBJ2 = "robotPlaceObj2";
	static constexpr const char* ROBOT_PLACE_OBJ3 = "robotPlaceObj3";
	static constexpr const char* ROBOT_PLACE_OBJ4 = "robotPlaceObj4";
	static constexpr const char* ROBOT_PLACE_OBJ5 = "robotPlaceObj5";
	static constexpr const char* ROBOT_PLACE_OBJ6 = "robotPlaceObj6";
	static constexpr const char* ROBOT_PLACE_OBJ7 = "robotPlaceObj7";
	static constexpr const char* ROBOT_PLACE_OBJ8 = "robotPlaceObj8";

	static constexpr const char* HUMAN_GRASP_OBJ1 = "humanGraspObj1";
	static constexpr const char* HUMAN_GRASP_OBJ2 = "humanGraspObj2";
	static constexpr const char* HUMAN_GRASP_OBJ3 = "humanGraspObj3";
	static constexpr const char* HUMAN_GRASP_OBJ4 = "humanGraspObj4";
	static constexpr const char* HUMAN_GRASP_OBJ5 = "humanGraspObj5";
	static constexpr const char* HUMAN_GRASP_OBJ6 = "humanGraspObj6";
	static constexpr const char* HUMAN_GRASP_OBJ7 = "humanGraspObj7";
	static constexpr const char* HUMAN_GRASP_OBJ8 = "humanGraspObj8";

	static constexpr const char* HUMAN_PLACE_OBJ1 = "humanPlaceObj1";
	static constexpr const char* HUMAN_PLACE_OBJ2 = "humanPlaceObj2";
	static constexpr const char* HUMAN_PLACE_OBJ3 = "humanPlaceObj3";
	static constexpr const char* HUMAN_PLACE_OBJ4 = "humanPlaceObj4";
	static constexpr const char* HUMAN_PLACE_OBJ5 = "humanPlaceObj5";
	static constexpr const char* HUMAN_PLACE_OBJ6 = "humanPlaceObj6";
	static constexpr const char* HUMAN_PLACE_OBJ7 = "humanPlaceObj7";
	static constexpr const char* HUMAN_PLACE_OBJ8 = "humanPlaceObj8";

	static constexpr const char* CAN_RESTART = "canBeRestarted";
	static constexpr const char* RESTART = "restart";
	static constexpr const char* COLLISION = "collisionCounter";
	static constexpr const char* HUMAN_IDLE_TIME = "humanIdleTime";
	static constexpr const char* ROBOT_IDLE_TIME = "robotIdleTime";
	static constexpr const char* REPLANNING_COUNT = "replanningCount";
	static constexpr const char* COMMON_GRASP = "commonGrasp";

	bool simStarted;
	bool object1;
	bool object2;
	bool object3;
	bool object4;
	bool object5;
	bool object6;
	bool object7;
	bool object8;
	bool robotApproaching;
	bool robotGrasping;
	bool robotGraspObj1;
	bool robotGraspObj2;
	bool robotGraspObj3;
	bool robotGraspObj4;
	bool robotGraspObj5;
	bool robotGraspObj6;
	bool robotGraspObj7;
	bool robotGraspObj8;

	bool robotPlaceObj1;
	bool robotPlaceObj2;
	bool robotPlaceObj3;
	bool robotPlaceObj4;
	bool robotPlaceObj5;
	bool robotPlaceObj6;
	bool robotPlaceObj7;
	bool robotPlaceObj8;

	bool humanGraspObj1;
	bool humanGraspObj2;
	bool humanGraspObj3;
	bool humanGraspObj4;
	bool humanGraspObj5;
	bool humanGraspObj6;
	bool humanGraspObj7;
	bool humanGraspObj8;

	bool humanPlaceObj1;
	bool humanPlaceObj2;
	bool humanPlaceObj3;
	bool humanPlaceObj4;
	bool humanPlaceObj5;
	bool humanPlaceObj6;
	bool humanPlaceObj7;
	bool humanPlaceObj8;
	bool canRestart;
	bool restart;
	int collisionCounter;
	int humanIdleTime;
	int robotIdleTime;
	int replanningCount;
	int commonGrasp;

	IncomingSignals()
		: simStarted(false)
		, object1(false)
		, object2(false)
		, object3(false)
		, object4(false)
		, object5(false)
		, object6(false)
		, object7(false)
		, object8(false)
		, robotApproaching(false)
		, robotGrasping(false)
		, robotGraspObj1(false)
		, robotGraspObj2(false)
		, robotGraspObj3(false)
		, robotGraspObj4(false)
		, robotGraspObj5(false)
		, robotGraspObj6(false)
		, robotGraspObj7(false)
		, robotGraspObj8(false)

		, robotPlaceObj1(false)
		, robotPlaceObj2(false)
		, robotPlaceObj3(false)
		, robotPlaceObj4(false)
		, robotPlaceObj5(false)
		, robotPlaceObj6(false)
		, robotPlaceObj7(false)
		, robotPlaceObj8(false)

		, humanGraspObj1(false)
		, humanGraspObj2(false)
		, humanGraspObj3(false)
		, humanGraspObj4(false)
		, humanGraspObj5(false)
		, humanGraspObj6(false)
		, humanGraspObj7(false)
		, humanGraspObj8(false)

		, humanPlaceObj1(false)
		, humanPlaceObj2(false)
		, humanPlaceObj3(false)
		, humanPlaceObj4(false)
		, humanPlaceObj5(false)
		, humanPlaceObj6(false)
		, humanPlaceObj7(false)
		, humanPlaceObj8(false)
		
		, canRestart(false)
		, restart(false)
		, collisionCounter(0)
		, humanIdleTime(0)
		, robotIdleTime(0)
		, replanningCount(0)
		, commonGrasp(0)
	{}
};

struct OutgoingSignals
{
	static constexpr const char* START_SIM = "startSim";
	static constexpr const char* TARGET_OBJECT = "targetObject";
	static constexpr const char* ARCHITECTURE_TYPE = "archType";

	bool startSim;
	int targetObject;
	int archType;

	OutgoingSignals()
		: startSim(false)
		, targetObject(0)
		, archType(0)
	{}
};

class CoppeliasimHandler
{
private:
	coppeliasim_cpp::CoppeliaSimClient  incomingSignalsClient;
	coppeliasim_cpp::CoppeliaSimClient outgoingSignalsClient;
	coppeliasim_cpp::CoppeliaSimClient handClient;
	std::thread incomingSignalsThread;
	std::thread outgoingSignalsThread;
	std::thread handThread;
	IncomingSignals incomingSignals;
	OutgoingSignals outgoingSignals;
	HumanHand hand;
public:
	CoppeliasimHandler();
	~CoppeliasimHandler();

	void init();
	void setSignals(const OutgoingSignals& signals);
	IncomingSignals getSignals() const;
	Pose getHandPose() const;
	void end();

	bool isConnected() const;
	void resetSignals() const;
private:
	void incomingSignalsLoop();
	void outgoingSignalsLoop();
	void readHandPosition();
	void readSignals();
	void writeSignals() const;
	void printSignals() const;
};
