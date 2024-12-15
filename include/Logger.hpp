#pragma once

#define DEBUG 1
#define INFO 2
#define WARNING 3
#define ERROR 4
#define FATAL 5

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

// Singleton Logger
// Dupa instantierea clasei Logger, trebuie folosite metodele enableConsoleOutput si enableFileOutput pentru a activa iesirea in consola si/sau in fisier

class Logger 
{
    private:
        static Logger*  instance;

        bool            consoleOutputEnabled;
        bool            fileOutputEnabled;

        Logger(){};
        Logger(const Logger& obj) = delete;
        Logger(const Logger&& obj) = delete;
        ~Logger(){};

        std::ofstream   logFile;
        std::string     getLevel(int level) {return level == DEBUG ? "DEBUG" : level == INFO ? "INFO" : level == WARNING ? "WARNING" : level == ERROR ? "ERROR" : "FATAL";}
        std::string     getTime() {return std::to_string(time(0));}
        void            writeLog(std::string message);

    public:
        static Logger*  getInstance();
        void            log(int level, std::string message, std::string file);

        void            enableConsoleOutput(bool enable = true);
        void            enableFileOutput(const std::string filename, bool enable = true);
};