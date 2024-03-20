#pragma once

#include <thread>

#include <tools/logger.h>

#include "client.h"


struct SignalSignatures
{
	static constexpr const char* START_SIM = "startSim";
	static constexpr const char* SIM_STARTED = "simStarted";

	static constexpr const char* OBJECTS_CREATED = "objectsCreated";
	static constexpr const char* TARGET_OBJECT = "targetObject";

	static constexpr const char* OBJECT1_EXISTS = "object1";
	static constexpr const char* OBJECT2_EXISTS = "object2";
	static constexpr const char* OBJECT3_EXISTS = "object3";

	static constexpr const char* OBJECT_GRASPED = "objectGrasped";

	static constexpr const char* OBJECT_PLACED = "objectPlaced";

	//// There is a better way to do this using pack and unpack
	//// But changes to coppeliasim-cpp-client need to be made.
	//static constexpr const char* HAND_X = "hand_x";
	//static constexpr const char* HAND_Y = "hand_y";
	//static constexpr const char* HAND_Z = "hand_z";

};

struct Signals
{
	bool startSim = true;
	bool simStarted = false;

	bool objectsCreated = false;
	int targetObject = 0;

	bool object1 = false;
	bool object2 = false;
	bool object3 = false;

	bool objectGrasped = false;
	bool objectPlaced = false;
};

struct SimulationData
{
	coppeliasim_cpp::Position handPosition;
	coppeliasim_cpp::Position object1Position;
	coppeliasim_cpp::Position object2Position;
	coppeliasim_cpp::Position object3Position;
};

class CoppeliasimHandler
{
private:
	std::thread coppeliasimThread;
	coppeliasim_cpp::CoppeliaSimClient client;
	Signals signals;
	SimulationData data;
	bool wereSignalsChanged = false;
public:
	CoppeliasimHandler();
	~CoppeliasimHandler();

	void init();
	void run();
	void close();

	void setSignal(const std::string& signalName, const int signalValue);
	void setSignals(const Signals& sign);
	Signals getSignals() const;
	SimulationData getSimulationData() const;
	bool isConnected() const;

	void resetSignals() const;
private:
	void writeSignals();
	void readSignals();
	void updateData();
};


