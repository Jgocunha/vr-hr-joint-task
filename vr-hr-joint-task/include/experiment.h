#include "coppeliasim_handler.h"
#include "dnfcomposer_handler.h"
#include "experiment_monitoring.h"
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
	bool experimentRunning;
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
	void keepAliveWhileTaskIsRunning();

	void updateHumanHandPosition() const;
	void updateAvailableObjectsInWorkspace() const;
	void updateRobotTargetObject();
	void updateExperimentMonitoringLogs();
	void updateSignals();

	bool areObjectsPresent() const;
};
