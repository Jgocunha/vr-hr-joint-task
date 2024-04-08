#include "experiment_monitoring.h"

// Static members initialization
std::ofstream ExperimentMonitoring::logFile;
std::ofstream ExperimentMonitoring::logFileHuman;
std::string ExperimentMonitoring::sessionDir;

void ExperimentMonitoring::initialize()
{
    // Create a session directory
    const auto now = std::chrono::system_clock::now();
    const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%y-%m-%d_%Hh%Mm%Ss");
    sessionDir = std::string(OUTPUT_DIRECTORY) + "/session" + ss.str();

    std::filesystem::create_directories(sessionDir);

    // Open a log file in the session directory
    logFile.open(sessionDir + "/logs.txt", std::ofstream::out | std::ofstream::app);
	logFileHuman.open(sessionDir + "/logs_human.txt", std::ofstream::out | std::ofstream::app);

	// Log the start of the session
	experimentLog(LogLevel::CONTROL, "Session started at " + ss.str());
}

void ExperimentMonitoring::experimentLog(LogLevel level, const std::string& msg)
{
    if (!logFile.is_open()) return;

   // Get current time
   const auto now = std::chrono::system_clock::now();
   const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

   // Convert time to string and LogLevel to string
   std::stringstream timeSS, logSS;
   timeSS << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
   std::string levelStr;
   switch (level) {
   case LogLevel::CONTROL: levelStr = "CONTROL"; break;
   case LogLevel::ROBOT: levelStr = "ROBOT"; break;
   case LogLevel::HUMAN: levelStr = "HUMAN"; break;
   }

   // Format the log message
   logSS << timeSS.str() << " " << levelStr << " " << msg << std::endl;

   // Write to log file
   logFile << logSS.str();
   logFile.flush(); // Ensure that each message is immediately written to the file
}

void ExperimentMonitoring::humanPoseLog(const std::string& msg)
{
	if (!logFileHuman.is_open()) return;

	// Get current time
	const auto now = std::chrono::system_clock::now();
	const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

	// Convert time to string
	std::stringstream timeSS;
	timeSS << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");

	// Format the log message
	const std::string logMsg = timeSS.str() + " " + msg + "\n";

	// Write to log file
	logFileHuman << logMsg;
	logFileHuman.flush(); // Ensure that each message is immediately written to the file
}

void ExperimentMonitoring::finalize()
{
	if (logFile.is_open()) 
		logFile.close();
    if (logFileHuman.is_open())
		logFileHuman.close();
}
