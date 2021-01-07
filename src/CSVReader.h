#pragma once

#include "OrderBookEntry.h"
#include <vector>
#include <string>


class CSVReader
{
    public:
    /**Generic constructor*/
     CSVReader();
    /**Reads a csv file and returns a vector of OrderBookEntry strings */
     static std::vector<OrderBookEntry> readCSV(std::string csvFile);
    /**Splits lines in CSV file into tokens by seperator and returns vector of strings */
     static std::vector<std::string> tokenise(std::string csvLine, char separator);
    /**Converts given strings and OrderBookType and returns an OrderBookEntry*/
     static OrderBookEntry stringsToOBE(std::string price, 
                                        std::string amount, 
                                        std::string timestamp, 
                                        std::string product, 
                                        OrderBookType OrderBookType);

    private:
     static OrderBookEntry stringsToOBE(std::vector<std::string> strings);
     
};