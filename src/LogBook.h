#pragma once
#include <string>
#include <fstream>

class LogBook
{
    public:

    LogBook();
    static void startLog();
    static void stopLog();
    void write(std::string note);

    private:
    static std::ofstream logFile;

};

