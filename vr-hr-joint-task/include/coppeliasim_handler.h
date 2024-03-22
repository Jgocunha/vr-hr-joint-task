#pragma once

#include <thread>

#include <tools/logger.h>

#include "client.h"


struct SignalSignatures
{
	static constexpr const char* START_SIM = "startSim";
	static constexpr const char* SIM_STARTED = "simStarted";

	static constexpr const char* TARGET_OBJECT = "targetObject";

	static constexpr const char* OBJECT1_EXISTS = "object1";
	static constexpr const char* OBJECT2_EXISTS = "object2";
	static constexpr const char* OBJECT3_EXISTS = "object3";

	static constexpr const char* OBJECT_GRASPED = "objectGrasped";

	static constexpr const char* OBJECT_PLACED = "objectPlaced";

	static constexpr const char* HAND_Y = "hand_y";
	static constexpr const char* HAND_PROXIMITY = "hand_proximity";

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

	float hand_y = 0.00f;
	float hand_proximity = 0.00f;
};

class CoppeliasimHandler
{
private:
	std::thread coppeliasimThread;
	coppeliasim_cpp::CoppeliaSimClient client;
	Signals signals;
	bool wereSignalsChanged = false;
	int handHandle = 0;
public:
	CoppeliasimHandler();
	~CoppeliasimHandler();

	void init();
	void run();
	void close();

	bool hasSignalMajorityValue(const std::string& signalName, int requiredValue, int sampleSize) const;

	void setSignal(const std::string& signalName, const int signalValue);
	Signals getSignals() const;
	bool isConnected() const;

	void resetSignals() const;
private:
	void readSignals();
};


