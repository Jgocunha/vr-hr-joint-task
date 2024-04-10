#include "event_logger.h"

std::ofstream EventLogger::logFile;
std::ofstream EventLogger::humanHandPoseFile;
std::string EventLogger::sessionDirectory;

void EventLogger::initialize()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%y-%m-%d_%Hh%Mm%Ss");
    sessionDirectory = std::string(OUTPUT_DIRECTORY) + "/session" + ss.str();

    std::filesystem::create_directories(sessionDirectory);

    logFile.open(sessionDirectory + "/logs.txt", std::ofstream::out | std::ofstream::app);
    humanHandPoseFile.open(sessionDirectory + "/logs_human.txt", std::ofstream::out | std::ofstream::app);

    log(LogLevel::CONTROL, "Session started at " + ss.str());
}

void EventLogger::log(LogLevel level, const std::string& msg)
{
	if (!logFile.is_open()) return;

	const auto now = std::chrono::system_clock::now();
	const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

	std::stringstream timeSS, logSS;
	timeSS << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
	std::string levelStr;
	switch (level) {
	case LogLevel::CONTROL: levelStr = "CONTROL"; break;
	case LogLevel::ROBOT: levelStr = "ROBOT"; break;
	case LogLevel::HUMAN: levelStr = "HUMAN"; break;
	}

	logSS << timeSS.str() << " " << levelStr << " " << msg << std::endl;

	logFile << logSS.str();
	logFile.flush(); // Ensure that each message is immediately written to the file
}

void EventLogger::logHumanHandPose(const std::string& msg)
{
	if (!humanHandPoseFile.is_open()) return;

	const auto now = std::chrono::system_clock::now();
	const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

	std::stringstream timeSS;
	timeSS << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");

	const std::string logMsg = timeSS.str() + " " + msg + "\n";

	humanHandPoseFile << logMsg;
	humanHandPoseFile.flush(); // Ensure that each message is immediately written to the file
}

void EventLogger::finalize()
{
	if (logFile.is_open())
		logFile.close();
	if (humanHandPoseFile.is_open())
		humanHandPoseFile.close();
}