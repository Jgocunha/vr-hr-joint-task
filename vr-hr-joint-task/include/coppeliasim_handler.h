#pragma once

#include <thread>

#include <tools/logger.h>

#include "../lib/coppeliasim-cpp-client/include/client.h"

namespace coppeliasim
{
	struct Signals
	{
	};

	class CoppeliasimHandler
	{
	private:
		std::thread coppeliasimThread;
		CoppeliaSimClient client;
		Signals signals;
		bool wereSignalsChanged = false;
	public:
		CoppeliasimHandler();

		void init();
		void step();
		void close();

		void setSignals(Signals signals);
		Signals getSignals();

		void resetSignals();
	private:
		void writeSignals();
		void readSignals();
	};
}


