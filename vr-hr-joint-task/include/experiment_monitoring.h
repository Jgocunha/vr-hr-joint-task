#pragma once

#include <fstream>
#include <filesystem>

enum class LogLevel
{
    CONTROL,
    ROBOT,
    HUMAN,
};


class ExperimentMonitoring
{
private:
    static std::ofstream logFile;
    static std::ofstream logFileHuman;
    static std::string sessionDir;

public:
    static void initialize();
    static void experimentLog(LogLevel level, const std::string& msg);
    static void humanPoseLog(const std::string& msg);
    static void finalize();
};