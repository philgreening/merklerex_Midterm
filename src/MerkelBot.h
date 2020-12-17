#pragma once

#include "OrderBook.h"
#include "OrderBookEntry.h"
#include "Wallet.h"


class MerkelBot
{
    public:
        MerkelBot();
        void init();

    private:
        void predictFuture();
        void submitBid();
        void submitAsk();
        void gotoNextTimeframe();
        void printWallet();
        void calculateEma();


        //OrderBookEntry orderBookEntry;
        OrderBook orderBook{"20200601.csv"};
        Wallet wallet;
        std::string currentTime;
       

};


