#pragma once

#include "OrderBook.h"
#include "OrderBookEntry.h"
#include "Wallet.h"
#include "LogBook.h"


class MerkelBot
{
    public:
        /**Constructs orderbook*/
        MerkelBot(OrderBook& _orderBook);
        /** Call to start bot*/
        void init();

    private:
        void marketStats();
        void getBid();
        void submitBid(std::string product, double price, double amount);
        void getAsk();
        void submitAsk(std::string product, double price, double amount);
        void gotoNextTimeframe();
        void printWallet();

        OrderBook& orderBook;
        Wallet wallet;
        std::string currentTime;
        std::string previousTime;
        std::string username;
        LogBook logBook;

       

};


