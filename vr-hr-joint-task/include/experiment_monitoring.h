#pragma once

#include <fstream>
#include <filesystem>

enum class LogLevel
{
    INFO,
    WARNING,
   // ERROR
};


class ExperimentMonitoring
{
private:
    static std::ofstream logFile;
    static std::ofstream logFileHuman;
    static std::string sessionDir;

public:
    static void initialize();
    static void monitor_log(LogLevel level, const std::string& msg);
    static void log_human_pose(const std::string& msg);
    static void finalize();
};