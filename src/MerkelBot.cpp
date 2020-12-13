#include "MerkelBot.h"
#include <iostream>

MerkelBot::MerkelBot()
{

}

void MerkelBot::init()
{
    //R1A retrieves order book
    currentTime = orderBook.getEarliestTime();
    predictFuture();
}

void MerkelBot::predictFuture()
{
     for (std::string const& p : orderBook.getKnownProducts())
    {
        //TODO: prediction algorithm
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    }
}