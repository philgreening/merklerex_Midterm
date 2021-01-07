#pragma once
#include <fstream>
#include <string>

class LogBook
{
    public:
    /** Generic constructor */
    LogBook();    
    /**Stops logging */
    static void stopLog();
    /**Function to write string to file */
    void write(std::string note);

    private:
    static void startLog();
    static std::ofstream logFile;

};

