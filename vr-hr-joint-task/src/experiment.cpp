#include "experiment.h"


Experiment::Experiment(std::string name, RobotArchitecture architecture, int commsFreq, double deltaT)
		: coppeliasimHandler(), architecture(architecture), dnfcomposerHandler({ architecture,std::move(name), deltaT }), commsFrequency(commsFreq)
{
	experimentRunning = false;
	handPosition = {0.0, 0.0, 0.0};
}

Experiment::~Experiment()
{
	close();
}

void Experiment::init()
{
	coppeliasimHandler.init();
	dnfcomposerHandler.init();
	ExperimentMonitoring::initialize();
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
	ExperimentMonitoring::finalize();
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
	ExperimentMonitoring::monitor_log(LogLevel::INFO, "Connection with CoppeliaSim established.");
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
	ExperimentMonitoring::monitor_log(LogLevel::INFO, "Simulation started.");
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
	ExperimentMonitoring::monitor_log(LogLevel::INFO, "Objects created.");
	log(dnf_composer::tools::logger::LogLevel::INFO, "Pick and place will now start...\n");
}

void Experiment::keepAliveWhileTaskIsRunning()
{
	experimentRunning = true;
	while(areObjectsPresent())
	{
		Sleep(commsFrequency);
	}
	// For now let's keep alive for a few seconds after the task is done.
	Sleep(10000);
	experimentRunning = false;
	ExperimentMonitoring::monitor_log(LogLevel::INFO, "All objects have been placed in the container.");
}

void Experiment::updateHumanHandPosition() const
{
	switch (architecture)
	{
		case RobotArchitecture::ACTION_LIKELIHOOD:
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


				if (likelihood_1 == 0)
				{
					std::cout << "Likelihood were zero: " << likelihood_1 << std::endl;
					handPrevious = handPosition;
					lastTime = currentTime;
					return;
				}
				/*std::cout << "Likelihood 1: " << likelihood_1 << std::endl;
				std::cout << "Likelihood 2: " << likelihood_2 << std::endl;
				std::cout << "Likelihood 3: " << likelihood_3 << std::endl;*/

				if (!coppeliasimSignals.object1)
					likelihood_1 = 0.0;
				if (!coppeliasimSignals.object2)
					likelihood_2 = 0.0;
				if (!coppeliasimSignals.object3)
					likelihood_3 = 0.0;

				dnfcomposerHandler.setHandStimulus(likelihood_1, likelihood_2, likelihood_3);

				handPrevious = handPosition;
				lastTime = currentTime;
			}
			break;
		case RobotArchitecture::HAND_MOTION:
			dnfcomposerHandler.setHandStimulus(coppeliasimSignals.hand_y, coppeliasimSignals.hand_proximity);
			break;
	}
	
}

void Experiment::updateAvailableObjectsInWorkspace() const
{
	static bool previousObject1 = coppeliasimSignals.object1;
	static bool previousObject2 = coppeliasimSignals.object2;
	static bool previousObject3 = coppeliasimSignals.object3;

	// Object 1
	if(previousObject1 != coppeliasimSignals.object1)
	{
		if (coppeliasimSignals.object1)
		{
			dnfcomposerHandler.addTargetObject(1);
		}
		else
		{
			dnfcomposerHandler.removeTargetObject(1);
		}
		previousObject1 = coppeliasimSignals.object1;
	}

	// Object 2
	if(previousObject2 != coppeliasimSignals.object2)
	{
		if (coppeliasimSignals.object2)
		{
			dnfcomposerHandler.addTargetObject(2);
		}
		else
		{
			dnfcomposerHandler.removeTargetObject(2);
		}
		previousObject2 = coppeliasimSignals.object2;
	}

	// Object 3
	if (previousObject3 != coppeliasimSignals.object3)
	{
		if (coppeliasimSignals.object3)
		{
			dnfcomposerHandler.addTargetObject(3);
		}
		else
		{
			dnfcomposerHandler.removeTargetObject(3);
		}
		previousObject3 = coppeliasimSignals.object3;
	}
}

void Experiment::updateRobotTargetObject()
{
	static int previousTargetObject = 0;
	dnfcomposerSignals.targetObject = dnfcomposerHandler.getTargetObject();
	if((dnfcomposerSignals.targetObject != previousTargetObject) && coppeliasimSignals.robotApproaching)
	{
		if(dnfcomposerSignals.targetObject == 0)
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Robot is not targeting any object.");
		else
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Robot is targeting object " + std::to_string(dnfcomposerSignals.targetObject) + ".");
			previousTargetObject = dnfcomposerSignals.targetObject;
		}
	}
}

void Experiment::updateExperimentMonitoringLogs()
{
	// Robot grasps
	{
		static bool robotGraspObj1Logged = false;
		static bool robotGraspObj2Logged = false;
		static bool robotGraspObj3Logged = false;
		if (coppeliasimSignals.robotGraspObj1 && !robotGraspObj1Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Robot has grasped object 1.");
			coppeliasimSignals.robotGraspObj1 = false;
			robotGraspObj1Logged = true;
		}
		if (coppeliasimSignals.robotGraspObj2 && !robotGraspObj2Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Robot has grasped object 2.");
			coppeliasimSignals.robotGraspObj2 = false;
			robotGraspObj2Logged = true;
		}
		if (coppeliasimSignals.robotGraspObj3 && !robotGraspObj3Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Robot has grasped object 3.");
			coppeliasimSignals.robotGraspObj3 = false;
			robotGraspObj3Logged = true;
		}
	}
	
	// Robot places
	{
		static bool robotPlaceObj1Logged = false;
		static bool robotPlaceObj2Logged = false;
		static bool robotPlaceObj3Logged = false;
		if (coppeliasimSignals.robotPlaceObj1 && !robotPlaceObj1Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Robot has placed object 1.");
			coppeliasimSignals.robotPlaceObj1 = false;
			robotPlaceObj1Logged = true;
		}
		if (coppeliasimSignals.robotPlaceObj2 && !robotPlaceObj2Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Robot has placed object 2.");
			coppeliasimSignals.robotPlaceObj2 = false;
			robotPlaceObj2Logged = true;
		}
		if (coppeliasimSignals.robotPlaceObj3 && !robotPlaceObj3Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Robot has placed object 3.");
			coppeliasimSignals.robotPlaceObj3 = false;
			robotPlaceObj3Logged = true;
		}
	}

	// Human grasps
	{
		static bool humanGraspObj1Logged = false;
		static bool humanGraspObj2Logged = false;
		static bool humanGraspObj3Logged = false;
		if (coppeliasimSignals.humanGraspObj1 && !humanGraspObj1Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Human has grasped object 1.");
			coppeliasimSignals.humanGraspObj1 = false;
			humanGraspObj1Logged = true;
		}
		if (coppeliasimSignals.humanGraspObj2 && !humanGraspObj2Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Human has grasped object 2.");
			coppeliasimSignals.humanGraspObj2 = false;
			humanGraspObj2Logged = true;
		}
		if (coppeliasimSignals.humanGraspObj3 && !humanGraspObj3Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Human has grasped object 3.");
			coppeliasimSignals.humanGraspObj3 = false;
			humanGraspObj3Logged = true;
		}
	}

	// Human places
	{
		static bool humanPlaceObj1Logged = false;
		static bool humanPlaceObj2Logged = false;
		static bool humanPlaceObj3Logged = false;
		if (coppeliasimSignals.humanPlaceObj1 && !humanPlaceObj1Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Human has placed object 1.");
			coppeliasimSignals.humanPlaceObj1 = false;
			humanPlaceObj1Logged = true;
		}
		if (coppeliasimSignals.humanPlaceObj2 && !humanPlaceObj2Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Human has placed object 2.");
			coppeliasimSignals.humanPlaceObj2 = false;
			humanPlaceObj2Logged = true;
		}
		if (coppeliasimSignals.humanPlaceObj3 && !humanPlaceObj3Logged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Human has placed object 3.");
			coppeliasimSignals.humanPlaceObj3 = false;
			humanPlaceObj3Logged = true;
		}
	}
	
}

void Experiment::updateSignals()
{
	do
	{
		coppeliasimSignals = coppeliasimHandler.getSignals();
		const Pose pose = coppeliasimHandler.getHandPose();
		handPosition = pose.position;
		std::string msg = std::to_string(handPosition.x) + ", " + std::to_string(handPosition.y) + ", " + std::to_string(handPosition.z) +
			", " + std::to_string(pose.orientation.alpha) + ", " + std::to_string(pose.orientation.beta) + ", " + std::to_string(pose.orientation.gamma);
		ExperimentMonitoring::log_human_pose(msg);
		if (experimentRunning)
		{
			updateAvailableObjectsInWorkspace();
			updateHumanHandPosition();
			updateRobotTargetObject();
			updateExperimentMonitoringLogs();
		}
		coppeliasimHandler.setSignals(dnfcomposerSignals);
		Sleep(10);
	} while (true);
}

bool Experiment::areObjectsPresent() const
{
	const bool isPresent = coppeliasimSignals.object1 != 0 || coppeliasimSignals.object2 != 0 || coppeliasimSignals.object3 != 0;
	return isPresent;
}