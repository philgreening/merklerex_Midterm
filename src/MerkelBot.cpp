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
    //logBook.startLog();
    printWallet();
   
    while(currentTime <  orderBook.getNextTime(currentTime))
    {
        logBook.write(std::to_string(count) + " ===== " + "Current time: "+ currentTime + " =====");
        predictFuture();
        getAsk();
        getBid();
        gotoNextTimeframe();
        printWallet();
        count ++;
        //std::cout << count << std::endl;
        
    }
    logBook.stopLog();
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
        logBook.write( p + " Average ask price: "+ std::to_string(OrderBook::getAveragePrice(entries)));
        std::cout << "Average ask amount: " << OrderBook::getAverageAmount(entries) << std::endl;
        std::cout << "Average bid price: " << OrderBook::getAveragePrice(entries1) << std::endl;
        logBook.write( p + " Average bid price: "+ std::to_string(OrderBook::getAveragePrice(entries1)));
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
        //std::cout << "Product: " << p << std::endl;
        logBook.write(" MerkelBot:: get bid: Product: " + p );
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        double bidPrice = OrderBook::getAveragePrice(entries);
        double bidAmount = OrderBook::getAverageAmount(entries);

        submitBid(p, bidPrice, bidAmount);
        //logBook.write("Bid submitted: " + p + " Price: " + std::to_string(bidPrice)  + " Amount: " + std::to_string(1));
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
            logBook.write("Bid:: Wallet looks good ." );
            orderBook.insertOrder(obe);
            logBook.write("Place bid: "  + product + " bid price: " + std::to_string(price) + " bid amount: " + std::to_string(amount));
        }
        else {
            //std::cout << "Wallet has insufficient funds to accomodate bid." << std::endl;
            logBook.write("Bid:: Wallet has insufficient funds." );

        }
    }catch (const std::exception& e)
    {
        //std::cout << "MerkelBot::submitBid Bad Input." << std::endl;
        logBook.write("MerkelBot::submitBid Bad Input." );

    }   
}

void MerkelBot::getAsk()
{

    for (std::string const& p : orderBook.getKnownProducts())
    {
        //TODO: prediction algorithm
        //std::cout << "Product: " << p << std::endl;
        logBook.write(" MerkelBot:: get ask: Product: " + p );
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);
        double askPrice = OrderBook::getAveragePrice(entries);
        double askAmount = OrderBook::getAverageAmount(entries);

        submitAsk(p, askPrice, askAmount);
        //logBook.write("Ask submitted: " + p + " Price: " + std::to_string(askPrice)  + " Amount: " + std::to_string(1));

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
            //std::cout << "Wallet has sufficient funds to accomodate bid." << std::endl;
            logBook.write("Ask:: Wallet looks good ." );
            orderBook.insertOrder(obe);
            logBook.write("Place ask: "  + product + " ask price: " + std::to_string(price) + " ask amount: " + std::to_string(amount));

        }
        else {
            //std::cout << "Wallet has insufficient funds to accomodate bid." << std::endl;
            logBook.write("Ask:: Wallet has insufficient funds." );
        }
    }catch (const std::exception& e)
    {
        //std::cout << "Received invalid bid order." << std::endl;
        logBook.write("MerkelBot::submitAsk Bad Input." );
    }   
}

void MerkelBot::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        logBook.write("Matching: " + p);
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime, username);
        std::cout << "Sales: " << sales.size() << std::endl;
        logBook.write("Sales: " + std::to_string(sales.size()));
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl;
            logBook.write("Sale price: " + std::to_string(sale.price) + " amount: " + std::to_string(sale.amount)); 
            if (sale.username == "bot")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
    }

    currentTime = orderBook.getNextTime(currentTime);
    std::cout << currentTime << std::endl;
    logBook.write("\n" "Going to next time frame: " + currentTime);
}

void MerkelBot::printWallet()
{
    std::cout << wallet.toString() << std::endl;
    logBook.write("Wallet contains: " "\n" + wallet.toString());
}
 
