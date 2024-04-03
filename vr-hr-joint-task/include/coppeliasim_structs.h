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
	static constexpr const char* OBJECT_GRASPED = "objectGrasped";
	static constexpr const char* OBJECT_PLACED = "objectPlaced";
};

struct IncomingSignals
{
	bool simStarted;
	bool object1;
	bool object2;
	bool object3;
	bool objectGrasped;
	bool objectPlaced;

	IncomingSignals()
		: simStarted(false)
		, object1(false)
		, object2(false)
		, object3(false)
		,objectGrasped(false)
		,objectPlaced(false)
	{
	}

	void print() const
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Incoming signals:");
		log(dnf_composer::tools::logger::LogLevel::INFO, "Sim started: " + std::to_string(simStarted));
		log(dnf_composer::tools::logger::LogLevel::INFO, "Object 1: " + std::to_string(object1));
		log(dnf_composer::tools::logger::LogLevel::INFO, "Object 2: " + std::to_string(object2));
		log(dnf_composer::tools::logger::LogLevel::INFO, "Object 3: " + std::to_string(object3));
		log(dnf_composer::tools::logger::LogLevel::INFO, "Object grasped: " + std::to_string(objectGrasped));
		log(dnf_composer::tools::logger::LogLevel::INFO, "Object placed: " + std::to_string(objectPlaced));
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
		log(dnf_composer::tools::logger::LogLevel::INFO, "Start sim: " + std::to_string(startSim));
		log(dnf_composer::tools::logger::LogLevel::INFO, "Target object: " + std::to_string(targetObject));
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