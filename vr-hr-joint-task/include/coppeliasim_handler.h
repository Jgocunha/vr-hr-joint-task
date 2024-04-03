#pragma once

#include <thread>

#include <tools/logger.h>
#include "coppeliasim_structs.h"
#include "misc.h"


class CoppeliasimHandler
{
private:
	coppeliasim_cpp::CoppeliaSimClient client;
	std::thread coppeliasimThread;
	Signals signals;
	Object hand;
public:
	CoppeliasimHandler();
	~CoppeliasimHandler();
	void init();
	void run();
	void close();
	bool isConnected() const;

	Pose getHandPose() const;
	void setSignals(const OutgoingSignals& signals);
	IncomingSignals getSignals() const;
	void resetSignals();
private:
	void readSignals();
	void writeSignals() const;
	void printSignals() const;
	void readObjectData();
};


