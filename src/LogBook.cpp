#include "LogBook.h"

std::ofstream LogBook::logFile;

LogBook::LogBook()
{
    startLog();
}

void LogBook::startLog()
{
    logFile.open("logBook.txt");
}

void LogBook::stopLog()
{
    logFile.close();
}

void LogBook::write(std::string note)
{
    logFile << note << std::endl;
}