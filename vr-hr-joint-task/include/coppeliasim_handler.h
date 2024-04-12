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
	static constexpr const char* ROBOT_APPROACH = "robotApproaching";
	static constexpr const char* ROBOT_GRASP = "robotGrasping";
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
	static constexpr const char* CAN_RESTART = "canBeRestarted";
	static constexpr const char* RESTART = "restart";

	bool simStarted;
	bool object1;
	bool object2;
	bool object3;
	bool robotApproaching;
	bool robotGrasping;
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
	bool canRestart;
	bool restart;

	IncomingSignals()
		: simStarted(false)
		, object1(false)
		, object2(false)
		, object3(false)
		, robotApproaching(false)
		, robotGrasping(false)
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
		, canRestart(false)
		, restart(false)
	{}
};

struct OutgoingSignals
{
	static constexpr const char* START_SIM = "startSim";
	static constexpr const char* TARGET_OBJECT = "targetObject";

	bool startSim;
	int targetObject;

	OutgoingSignals()
		: startSim(false)
		, targetObject(0)
	{}
};

class CoppeliasimHandler
{
private:
	coppeliasim_cpp::CoppeliaSimClient incomingSignalsClient;
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
