#include "coppeliasim_handler.h"
#include "dnfcomposer_handler.h"
#include "misc.h"
#include <algorithm> 

class Experiment
{
private:
	CoppeliasimHandler coppeliasimHandler;
	DNFComposerHandler dnfcomposerHandler;
	int commsFrequency;
	std::thread signalsThread;
	bool taskFinished;
public:
	Experiment(std::string name, int commsFreq, double deltaT);
	~Experiment();

	void init();
	void run();
	void close();
private:
	void main();
	void waitForConnection() const;
	void waitForSimulationStart();
	void waitForObjectsToBeCreated() const;
	void pickAndPlaceObjects();

	void updateHandPosition() const;
	void updateAvailableObjects();
	void updateTargetObject();
	void updateSignals();

	bool areObjectsPresent() const;
};
