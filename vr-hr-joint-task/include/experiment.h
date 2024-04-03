#include "coppeliasim_handler.h"
#include "dnfcomposer_handler.h"
#include "misc.h"
#include <algorithm>
#include <chrono>



class Experiment
{
private:
	CoppeliasimHandler coppeliasimHandler;
	DNFComposerHandler dnfcomposerHandler;
	int commsFrequency;
	std::thread signalsThread;
	IncomingSignals coppeliasimSignals;
	OutgoingSignals dnfcomposerSignals;
	Position handPosition;
	RobotArchitecture architecture;
public:
	Experiment(std::string name, RobotArchitecture architecture, int commsFreq, double deltaT);
	~Experiment();

	void init();
	void run();
	void close();
private:
	void main();
	void waitForConnection() const;
	void waitForSimulationStart();
	void waitForObjectsToBeCreated() const;
	void keepAliveWhileTaskIsRunning() const;

	void updateHandPosition() const;
	void updateAvailableObjects() const;
	void updateTargetObject();
	void updateSignals();

	bool areObjectsPresent() const;
};
