#pragma once

#include <fstream>
#include <filesystem>

enum class LogLevel
{
    CONTROL,
    ROBOT,
    HUMAN,
};

class EventLogger
{
    static std::ofstream logFile;
    static std::ofstream humanHandPoseFile;
    static std::string sessionDirectory;
public:
    static void initialize();
    static void log(LogLevel level, const std::string& message);
    static void logHumanHandPose(const std::string& message);
    static void finalize();
};
