#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "MerkelBot.h"


class MerkelMain
{
    public:
        /**Generic constructor */
        MerkelMain();
        /** Call this to start the sim */
        void init();
    
    private: 
        void printMenu();
        void printStartMenu();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void gotoNextTimeframe();
        int getUserOption();
        int getStartOption();
        void processUserOption(int userOption);
        void processStartOption(int userOption);
        void initUserMode();



        OrderBook orderBook{"20200317.csv"};
	    //OrderBook orderBook{"20200601.csv"};
        Wallet wallet;
        std::string currentTime;
        std::string username;
        MerkelBot botApp{orderBook};
};
