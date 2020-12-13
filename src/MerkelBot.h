#pragma once

#include "OrderBook.h"


class MerkelBot
{
    public:
        MerkelBot();
        void init();

    private:
        void loadOrderBook();
        void predictFuture();

        OrderBook orderBook{"20200601.csv"};
        std::string currentTime;
};


