#include "MerkelBot.h"
#include <iostream>
#include <cmath>

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
        logBook.write( "=====Current time: " + currentTime + " =====" "\n" "Timeframe number: "+ std::to_string(count) + "\n");
        logBook.write("*****Market Stats*****");
        predictFuture();
        logBook.write("\n" "*****Getting bids and Asks*****");
        getAsk();
        getBid();
        logBook.write("\n" "*****Matching orders*****");
        gotoNextTimeframe();
        printWallet();
        count ++;

    }
    logBook.stopLog();
}

void MerkelBot::predictFuture()
{
    previousTime = orderBook.getPrevTime(currentTime);
    logBook.write("\n" + previousTime + "\n");

    for (std::string const& p : orderBook.getKnownProducts())
    {
        //TODO: prediction algorithm
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::vector<OrderBookEntry> entries1 = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);
        std::vector<OrderBookEntry> prevEntriesBid = orderBook.getOrders(OrderBookType::bid, 
                                                                p, previousTime);
        std::vector<OrderBookEntry> prevEntriesAsk = orderBook.getOrders(OrderBookType::ask, 
                                                                p, previousTime);                                                        

        logBook.write("Asks seen: " + std::to_string(entries.size()));
        logBook.write("Bids seen: " + std::to_string(entries1.size()));
     
        logBook.write("\n" + p + " Average ask price: "+ std::to_string(OrderBook::getAveragePrice(entries)));
        logBook.write( p + " Average previous ask price: "+ std::to_string(OrderBook::getPrevAveragePrice(prevEntriesAsk)));

        logBook.write( p + " Average bid price: "+ std::to_string(OrderBook::getAveragePrice(entries1)));
        logBook.write( p + " Average previous bid price: "+ std::to_string(OrderBook::getPrevAveragePrice(prevEntriesBid)));
        
        double previousBidPrice = OrderBook::getAveragePrice(prevEntriesBid);
        double bidPrice = OrderBook::getAveragePrice(entries1);
        double previousAskPrice = OrderBook::getAveragePrice(prevEntriesAsk);
        double askPrice = OrderBook::getAveragePrice(entries);
        double percent = (previousBidPrice - bidPrice) / previousBidPrice * 100 ;
        percent = std::round(percent);
        if (bidPrice < previousBidPrice)
        {
             logBook.write( "*****" "Average bid price gone down - buy at: " + std::to_string(bidPrice) + " " + std::to_string(percent) + "%" "\n" );
        }
        else if (askPrice > previousAskPrice)
        {
            logBook.write("*****" "Average ask price gone up - sell at: "   + std::to_string(askPrice) + " " + std::to_string(percent) + "%" + "\n");
        }
    }
}

void MerkelBot::getBid()
{

    for (std::string const& p : orderBook.getKnownProducts())
    {

        logBook.write("MerkelBot:: get bid: Product: " + p );
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);
        std::vector<OrderBookEntry> prevBidEntries = orderBook.getOrders(OrderBookType::bid, 
                                                                    p, previousTime);                                                           
        double bidPrice = OrderBook::getAveragePrice(entries);
        double prevBidAvePrice = OrderBook::getAveragePrice(prevBidEntries);
        double BidAvePrice = OrderBook::getAveragePrice(bidEntries);

        double bidAmount = OrderBook::getAverageAmount(entries);

        if (BidAvePrice < prevBidAvePrice)
        {
            submitBid(p, bidPrice, bidAmount);
        }
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
            logBook.write("Bid:: Wallet has insufficient funds." );
        }
    }catch (const std::exception& e)
    {
        logBook.write("MerkelBot::submitBid Bad Input." );
    }   
}

void MerkelBot::getAsk()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        logBook.write("MerkelBot:: get ask: Product: " + p );
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);
        std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::vector<OrderBookEntry> prevAskEntries = orderBook.getOrders(OrderBookType::ask, 
                                                                    p, previousTime);    
                            
        double askPrice = OrderBook::getAveragePrice(entries);
        double askAmount = OrderBook::getAverageAmount(entries);

        double prevAskAvePrice = OrderBook::getAveragePrice(prevAskEntries);
        double askAvePrice = OrderBook::getAveragePrice(askEntries);

        if (askAvePrice > prevAskAvePrice)
        {
            submitAsk(p, askPrice, askAmount);
        }
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
    logBook.write("\n" "Going to next time frame: " + currentTime + "\n");
}

void MerkelBot::printWallet()
{
    std::cout << wallet.toString() << std::endl;
    logBook.write("Wallet contains: " "\n" + wallet.toString());
}
 
