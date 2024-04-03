#include "experiment.h"


Experiment::Experiment(std::string name, int commsFreq, double deltaT)
		: coppeliasimHandler(), dnfcomposerHandler({ std::move(name), deltaT }), commsFrequency(commsFreq)
{
}

Experiment::~Experiment()
{
	close();
}

void Experiment::init()
{
	coppeliasimHandler.init();
	dnfcomposerHandler.init();
}

void Experiment::run()
{
	main();
}

void Experiment::close()
{
	coppeliasimHandler.close();
	dnfcomposerHandler.close();
	signalsThread.join();
}

void Experiment::main()
{
	waitForConnection();
	Sleep(commsFrequency);
	signalsThread = std::thread(&Experiment::updateSignals, this);
	waitForSimulationStart();
	waitForObjectsToBeCreated();
	keepAliveWhileTaskIsRunning();
}

void Experiment::waitForConnection() const
{
	while (!coppeliasimHandler.isConnected())
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for connection with CoppeliaSim...\n");
		Sleep(commsFrequency);
	}
}

void Experiment::waitForSimulationStart()
{
	dnfcomposerSignals.startSim = true;
	bool hasSimStarted = coppeliasimSignals.simStarted;
	while (!hasSimStarted)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for CoppeliaSim to start...\n");
		hasSimStarted = coppeliasimSignals.simStarted;
		Sleep(commsFrequency);
	}
}

void Experiment::waitForObjectsToBeCreated() const
{
	bool areObjectsPresentInScene = areObjectsPresent();
	while (!areObjectsPresentInScene)
	{
		log(dnf_composer::tools::logger::LogLevel::INFO, "Waiting for objects to be created...\n");
		areObjectsPresentInScene = areObjectsPresent();
		Sleep(commsFrequency);
	}

	log(dnf_composer::tools::logger::LogLevel::INFO, "Pick and place will now start...\n");
}

void Experiment::keepAliveWhileTaskIsRunning() const
{
	while(areObjectsPresent())
	{
		Sleep(commsFrequency);
	}
	// For now let's keep alive for a few seconds after the task is done.
	Sleep(10000);
}

void Experiment::updateHandPosition() const
{
	static Position handPrevious = handPosition;
	static const Position objPosition1 = { 0.60023, -0.02697, 0.70607 };
	static const Position objPosition2 = { 0.60042, -0.15003, 0.70607 };
	static const Position objPosition3 = { 0.60344, -0.27474, 0.70607 };
	static constexpr double tau = 0.1;
	static constexpr double sigma = 0.1;

	using Clock = std::chrono::high_resolution_clock;
	auto lastTime = Clock::now();

	const auto currentTime = Clock::now();
	const std::chrono::duration<double> elapsed = currentTime - lastTime;
	const double deltaTime = elapsed.count();

	// Check if elapsed time is too small
	if (deltaTime < std::numeric_limits<double>::epsilon())
		return;

	double likelihood_1 = calculateLikelihoodOfHumanAction(handPosition, handPrevious, objPosition1, deltaTime, tau, sigma);
	double likelihood_2 = calculateLikelihoodOfHumanAction(handPosition, handPrevious, objPosition2, deltaTime, tau, sigma);
	double likelihood_3 = calculateLikelihoodOfHumanAction(handPosition, handPrevious, objPosition3, deltaTime, tau, sigma);

	if(!coppeliasimSignals.object1)
		likelihood_1 = 0.0;
	if (!coppeliasimSignals.object2)
		likelihood_2 = 0.0;
	if (!coppeliasimSignals.object3)
		likelihood_3 = 0.0;

	dnfcomposerHandler.setHandStimulus(likelihood_1, likelihood_2, likelihood_3);

	handPrevious = handPosition;
	lastTime = currentTime;
}

void Experiment::updateAvailableObjects() const
{
	// Object 1
	if (!coppeliasimSignals.object1)
		dnfcomposerHandler.removeTargetObject(1);
	else
		dnfcomposerHandler.addTargetObject(1);

	// Object 2
	if (!coppeliasimSignals.object2)
		dnfcomposerHandler.removeTargetObject(2);
	else
		dnfcomposerHandler.addTargetObject(2);

	// Object 3
	if (!coppeliasimSignals.object3)
		dnfcomposerHandler.removeTargetObject(3);
	else 
		dnfcomposerHandler.addTargetObject(3);
}

void Experiment::updateTargetObject()
{
	dnfcomposerSignals.targetObject = dnfcomposerHandler.getTargetObject();
}

void Experiment::updateSignals()
{
	do
	{
		coppeliasimSignals = coppeliasimHandler.getSignals();
		handPosition = coppeliasimHandler.getHandPose().position;
		updateHandPosition();
		updateAvailableObjects();
		updateTargetObject();
		coppeliasimHandler.setSignals(dnfcomposerSignals);
		Sleep(10);
	} while (true);
}

bool Experiment::areObjectsPresent() const
{
	const bool isPresent = coppeliasimSignals.object1 != 0 || coppeliasimSignals.object2 != 0 || coppeliasimSignals.object3 != 0;
	return isPresent;
}