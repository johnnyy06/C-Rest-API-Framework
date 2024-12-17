#include "Logger.hpp"



Logger *Logger::instance = nullptr;

std::string Logger::getTime()
{
    // returneaza timpul curent sub forma de string
    std::time_t now = std::time(nullptr);
    std::tm *time = std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(time, "[%Y-%m-%d %H:%M:%S]");

    return oss.str();
}

void Logger::writeLog(std::string message)
{
    if (consoleOutputEnabled)
    {
        std::cout << message << std::endl;
    }

    if (fileOutputEnabled)
    {
        logFile << message << std::endl;
    }
}

Logger *Logger::getInstance()
{
    if (instance == nullptr)
    {
        instance = new Logger();
    }

    return instance;
}

// file se refera la fisierul in care s-a apelat functia log
void Logger::log(int level, std::string message, std::string file)
{
    // foloseste writeLog pentru a scrie mesajul in consola si/sau in fisier
    writeLog("[" + getLevel(level) + "]" + " " + getTime() + " " + message + " " + "[" + file + "]");
}

void Logger::enableConsoleOutput(bool enable)
{
    consoleOutputEnabled = enable;
}

void Logger::enableFileOutput(const std::string filename, bool enable)
{
    fileOutputEnabled = enable;
    if (enable)
    {
        logFile.open(filename, std::ios::app);
    }
    else
    {
        logFile.close();
    }
}
