#pragma once

#include "OrderBook.h"
#include "OrderBookEntry.h"
#include "Wallet.h"
#include "LogBook.h"


class MerkelBot
{
    public:
        MerkelBot(OrderBook& _orderBook);
        void init();

    private:
        void predictFuture();
        void getBid();
        void submitBid(std::string product, double price, double amount);
        void getAsk();
        void submitAsk(std::string product, double price, double amount);
        void gotoNextTimeframe();
        void printWallet();
        void calculateEma();


        //OrderBookEntry orderBookEntry;
        //OrderBook orderBook{"20200601.csv"};
        //OrderBook orderBook{"20200317.csv"};
        OrderBook& orderBook;
        Wallet wallet;
        std::string currentTime;
        std::string username;
        LogBook logBook;

       

};


