#include "experiment.h"


Experiment::Experiment(std::string name, RobotArchitecture architecture, int commsFreq, double deltaT)
		: coppeliasimHandler(), architecture(architecture), dnfcomposerHandler({ architecture,std::move(name), deltaT }), commsFrequency(commsFreq)
{
	experimentRunning = false;
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
	ExperimentMonitoring::monitor_log(LogLevel::INFO, "All objects have been placed in the container.");
	// For now let's keep alive for a few seconds after the task is done.
	Sleep(10000);
}

void Experiment::updateHandPosition() const
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

void Experiment::updateAvailableObjects() const
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

void Experiment::updateTargetObject()
{
	static int previousTargetObject = 0;
	dnfcomposerSignals.targetObject = dnfcomposerHandler.getTargetObject();
	if((dnfcomposerSignals.targetObject != previousTargetObject) && dnfcomposerSignals.targetObject!=0)
	{
		ExperimentMonitoring::monitor_log(LogLevel::INFO, "Robot is targeting object " + std::to_string(dnfcomposerSignals.targetObject) + ".");
		previousTargetObject = dnfcomposerSignals.targetObject;
	}
}

void Experiment::checkHumanState() const
{
	static bool object1GraspedLogged = false;
	static bool object2GraspedLogged = false;
	static bool object3GraspedLogged = false;

	static bool object1PlacedLogged = false;
	static bool object2PlacedLogged = false;
	static bool object3PlacedLogged = false;

	static int graspedObject = 0;

	if(coppeliasimSignals.humanGraspObj1)
		{
		if (!object1GraspedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 1 has been grasped by the human.");
			object1GraspedLogged = true; // Prevents further logging for object 1
			graspedObject = 1;
		}
	}
	if (coppeliasimSignals.humanGraspObj2)
	{
		if (!object2GraspedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 2 has been grasped by the human.");
			object2GraspedLogged = true; // Prevents further logging for object 2
			graspedObject = 2;
		}
	}
	if (coppeliasimSignals.humanGraspObj3)
	{
		if (!object3GraspedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 3 has been grasped by the human.");
			object3GraspedLogged = true; // Prevents further logging for object 3
			graspedObject = 3;
		}
	}

	if (coppeliasimSignals.humanPlaceObj1)
	{
		if (!object1PlacedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 1 has been placed by the human.");
			object1PlacedLogged = true; // Prevents further logging for object 1
		}
	}
	if (coppeliasimSignals.humanPlaceObj2)
	{
		if (!object2PlacedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 2 has been placed by the human.");
			object2PlacedLogged = true; // Prevents further logging for object 2
		}
	}
	if (coppeliasimSignals.humanPlaceObj3)
	{
		if (!object3PlacedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 3 has been placed by the human.");
			object3PlacedLogged = true; // Prevents further logging for object 3
		}
	}
}



void Experiment::checkRobotState() const
{
	static bool object1GraspedLogged = false;
	static bool object2GraspedLogged = false;
	static bool object3GraspedLogged = false;

	static bool object1PlacedLogged = false;
	static bool object2PlacedLogged = false;
	static bool object3PlacedLogged = false;

	static int graspedObject = 0;

	if (coppeliasimSignals.objectGrasped)
	{
		if (dnfcomposerSignals.targetObject == 1 && !object1GraspedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 1 has been grasped by the robot.");
			object1GraspedLogged = true; // Prevents further logging for object 1
			graspedObject = 1;
		}
		else if (dnfcomposerSignals.targetObject == 2 && !object2GraspedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 2 has been grasped by the robot.");
			object2GraspedLogged = true; // Prevents further logging for object 2
			graspedObject = 2;
		}
		else if (dnfcomposerSignals.targetObject == 3 && !object3GraspedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 3 has been grasped by the robot.");
			object3GraspedLogged = true; // Prevents further logging for object 3
			graspedObject = 3;
		}
	}

	if (coppeliasimSignals.objectPlaced)
	{
		if (graspedObject == 1 && !object1PlacedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 1 has been placed by the robot.");
			object1PlacedLogged = true; // Prevents further logging for object 1
		}
		if (graspedObject == 2 && !object2PlacedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 2 has been placed by the robot.");
			object2PlacedLogged = true; // Prevents further logging for object 2
		}
		if (graspedObject == 3 && !object3PlacedLogged)
		{
			ExperimentMonitoring::monitor_log(LogLevel::INFO, "Object 3 has been placed by the robot.");
			object3PlacedLogged = true; // Prevents further logging for object 3
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
		updateHandPosition();
		updateAvailableObjects();
		if (experimentRunning)
			updateTargetObject();
		if(experimentRunning)
			checkRobotState();
		coppeliasimHandler.setSignals(dnfcomposerSignals);
		Sleep(10);
	} while (true);
}

bool Experiment::areObjectsPresent() const
{
	const bool isPresent = coppeliasimSignals.object1 != 0 || coppeliasimSignals.object2 != 0 || coppeliasimSignals.object3 != 0;
	return isPresent;
}