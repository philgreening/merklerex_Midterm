#include "MerkelBot.h"
#include <iostream>

MerkelBot::MerkelBot(OrderBook& _orderBook)
: orderBook(_orderBook)
{
    username = "bot";
}

void MerkelBot::init()
{
    //R1A retrieves order book
    currentTime = orderBook.getEarliestTime();
    wallet.insertCurrency("BTC", 10);
    auto count = 0;
   
    while(currentTime <  orderBook.getNextTime(currentTime))
    {
        predictFuture();
        getAsk();
        getBid();
        gotoNextTimeframe();
        printWallet();
        count ++;
        std::cout << count << std::endl;
    }

}

void MerkelBot::predictFuture()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        //TODO: prediction algorithm
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::vector<OrderBookEntry> entries1 = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);

        // std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Average ask price: " << OrderBook::getAveragePrice(entries) << std::endl;
        std::cout << "Average ask amount: " << OrderBook::getAverageAmount(entries) << std::endl;
        std::cout << "Average bid price: " << OrderBook::getAveragePrice(entries1) << std::endl;
        std::cout << "Average bid amount: " << OrderBook::getAverageAmount(entries1) << std::endl;
        //std::cout << "ema: " << OrderBook::getEMA(entries1) << std::endl;


        // std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        // std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    }
}

void MerkelBot::getBid()
{

    for (std::string const& p : orderBook.getKnownProducts())
    {
        //TODO: prediction algorithm
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        double bidPrice = OrderBook::getAveragePrice(entries);
        double bidAmount = OrderBook::getAverageAmount(entries);

        submitBid(p, bidPrice, bidAmount);
    }
}

void MerkelBot::submitBid(std::string product, double price, double amount)
{
    OrderBookType orderType = OrderBookType::bid;
    try {
        OrderBookEntry obe{price,
                       amount,
                       currentTime, 
                       product,
                       orderType,
                       username};

        if (wallet.canFulfillOrder(obe))
        {
            std::cout << "Wallet has sufficient funds to accomodate bid." << std::endl;
            orderBook.insertOrder(obe);
        }
        else {
            std::cout << "Wallet has insufficient funds to accomodate bid." << std::endl;
        }
    }catch (const std::exception& e)
    {
        std::cout << "Received invalid bid order." << std::endl;
    }   
}

void MerkelBot::getAsk()
{

    for (std::string const& p : orderBook.getKnownProducts())
    {
        //TODO: prediction algorithm
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);
        double askPrice = OrderBook::getAveragePrice(entries);
        double askAmount = OrderBook::getAverageAmount(entries);

        submitAsk(p, askPrice, askAmount);
    }
}

void MerkelBot::submitAsk(std::string product, double price, double amount)
{
    OrderBookType orderType = OrderBookType::ask;
    try {
        OrderBookEntry obe{price,
                       amount,
                       currentTime, 
                       product,
                       orderType,
                       username};

        if (wallet.canFulfillOrder(obe))
        {
            std::cout << "Wallet has sufficient funds to accomodate bid." << std::endl;
            orderBook.insertOrder(obe);
        }
        else {
            std::cout << "Wallet has insufficient funds to accomodate bid." << std::endl;
        }
    }catch (const std::exception& e)
    {
        std::cout << "Received invalid bid order." << std::endl;
    }   
}

void MerkelBot::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime, username);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "bot")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
    }

    currentTime = orderBook.getNextTime(currentTime);
    std::cout << currentTime << std::endl;
}

void MerkelBot::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}
 
