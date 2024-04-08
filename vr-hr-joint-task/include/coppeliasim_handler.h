#pragma once

#include <thread>
#include <client.h>
#include <mutex>

#include "misc.h"


struct HumanHand
{
	int objectHandle;
	Position position;

	HumanHand(int objectHandle = 0, const Position& position = {0,0,0})
	: objectHandle(objectHandle), position(position)
	{}
};


struct SignalSignatures
{
	static constexpr const char* START_SIM = "startSim";
	static constexpr const char* SIM_STARTED = "simStarted";

	static constexpr const char* TARGET_OBJECT = "targetObject";

	static constexpr const char* OBJECT1_EXISTS = "object1";
	static constexpr const char* OBJECT2_EXISTS = "object2";
	static constexpr const char* OBJECT3_EXISTS = "object3";
};

struct Signals
{
	bool simStarted;
	bool startSim;
	bool object1;
	bool object2;
	bool object3;
	int targetObject;

	Signals()
		: simStarted(false),
	startSim(false),
	object1(false),
	object2(false),
	object3(false),
	targetObject(0)
	{}
};


class CoppeliasimHandler
{
private:
	coppeliasim_cpp::CoppeliaSimClient signalClient;
	coppeliasim_cpp::CoppeliaSimClient handClient;
	std::thread signalThread;
	std::thread handThread;
	HumanHand hand;
	Signals signals;
public:
	CoppeliasimHandler();
	~CoppeliasimHandler();

	void init();
	void setSignals(bool startSim, int targetObject);
	Signals getSignals() const;
	Position getHandPosition() const;
	void end();

	bool isConnected() const;
private:
	void readAndWriteSignals();
	void readHandPosition();
	void readSignals();
	void writeSignals() const;
	void resetSignals() const;
	void printSignals() const;
};
