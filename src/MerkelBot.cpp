#include "MerkelBot.h"
#include <iostream>
#include <cmath>
#include <chrono>

MerkelBot::MerkelBot(OrderBook& _orderBook)
: orderBook(_orderBook)
{
    username = "bot";
}

void MerkelBot::init()
{
    auto start = std::chrono::steady_clock::now();

    currentTime = orderBook.getEarliestTime();
    wallet.insertCurrency("BTC", 10);
    //count of timestamps
    auto count = 0;
    printWallet();
   
    //bot runs functions in order until there are no more timestamps
    while(currentTime <  orderBook.getNextTime(currentTime))
    {
        logBook.write( "=====Current time: "
                     + currentTime 
                     + " =====" "\n" "Timeframe number: "
                     + std::to_string(count) + "\n");
                     
        logBook.write("*****Market Stats*****");
        marketStats();
        logBook.write("\n" "*****Getting bids and Asks*****");
        getAsk();
        getBid();
        logBook.write("\n" "*****Matching orders*****");
        gotoNextTimeframe();
        printWallet();
        count ++;
    }
    logBook.stopLog();
    auto end = std::chrono::steady_clock::now();
    std::cout << "Elapsed time in nanoseconds : "<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
        << " ns" << std::endl;

}

void MerkelBot::marketStats()
{
    //gets previous timestamp
    previousTime = orderBook.getPrevTime(currentTime);

    for (std::string const& p : orderBook.getKnownProducts())
    {
        //vector of asks for current timestamp
        std::vector<OrderBookEntry> entriesAsk = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        //vector of bids for current timestamp                                                       
        std::vector<OrderBookEntry> entriesBid = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);
        //vector of bids for previous timestamp                                                      
        std::vector<OrderBookEntry> prevEntriesBid = orderBook.getOrders(OrderBookType::bid, 
                                                                p, previousTime);
        //vector of asks for previous timestamp                                                      
        std::vector<OrderBookEntry> prevEntriesAsk = orderBook.getOrders(OrderBookType::ask, 
                                                                p, previousTime);                                                        
     
        logBook.write("\n" + p + " Average ask price: " + std::to_string(OrderBook::getAveragePrice(entriesAsk)));
        logBook.write( p + " Average previous ask price: " + std::to_string(OrderBook::getAveragePrice(prevEntriesAsk)));

        logBook.write( p + " Average bid price: " + std::to_string(OrderBook::getAveragePrice(entriesBid)));
        logBook.write( p + " Average previous bid price: " + std::to_string(OrderBook::getAveragePrice(prevEntriesBid)));
        
        double previousBidPrice = OrderBook::getAveragePrice(prevEntriesBid);
        double bidPrice = OrderBook::getAveragePrice(entriesBid);
        double previousAskPrice = OrderBook::getAveragePrice(prevEntriesAsk);
        double askPrice = OrderBook::getAveragePrice(entriesAsk);

        //calculate price difference
        double bidChange = bidPrice - previousBidPrice;
        double askChange = askPrice - previousAskPrice;

        
        if (bidPrice < previousBidPrice)
        {
             logBook.write( "*****" "Average bid price has decreased by "
                          + std::to_string(bidChange) +  " - buy at: " 
                          + std::to_string(bidPrice) + "\n" );
        }
        else if (askPrice > previousAskPrice)
        {
            logBook.write("*****" "Average ask price has increased by " 
                         + std::to_string(askChange)+ " - sell at: " 
                         + std::to_string(askPrice) + "\n");
        }
    }
}

void MerkelBot::getBid()
{

    for (std::string const& p : orderBook.getKnownProducts())
    {
        logBook.write("MerkelBot:: get bid: Product: " + p );

        //vector of asks to set ask price
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        //vector bids for current timestamp
        std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);
        //vector bids for previous timestamp
        std::vector<OrderBookEntry> prevBidEntries = orderBook.getOrders(OrderBookType::bid, 
                                                                p, previousTime);                                                           
        double bidPrice = OrderBook::getAveragePrice(entries);
        double bidAmount = OrderBook::getAverageAmount(entries);

        double prevBidAvePrice = OrderBook::getAveragePrice(prevBidEntries);
        double BidAvePrice = OrderBook::getAveragePrice(bidEntries);
        //if average bid price has decreased submit bid
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
            logBook.write("Bid:: Wallet looks good ." );
            
            orderBook.insertOrder(obe);

            logBook.write("Place bid: "  + product + " bid price: " 
                         + std::to_string(price) 
                         + " bid amount: " 
                         + std::to_string(amount));
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
        //vector of bids to set ask price
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);
        //vector of asks for current timestamp
        std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        //vector of asks for previous timestamp
        std::vector<OrderBookEntry> prevAskEntries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, previousTime);    
                            
        double askPrice = OrderBook::getAveragePrice(entries);
        double askAmount = OrderBook::getAverageAmount(entries);

        double prevAskAvePrice = OrderBook::getAveragePrice(prevAskEntries);
        double askAvePrice = OrderBook::getAveragePrice(askEntries);
        //if ask price has increased submit ask
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
            logBook.write("Ask:: Wallet looks good ." );
            orderBook.insertOrder(obe);
            logBook.write("Place ask: "  + product 
                         + " ask price: " 
                         + std::to_string(price) 
                         + " ask amount: " 
                         + std::to_string(amount));
        }
        else {
            logBook.write("Ask:: Wallet has insufficient funds." );
        }
    }catch (const std::exception& e)
    {
        logBook.write("MerkelBot::submitAsk Bad Input." );
    }   
}

void MerkelBot::gotoNextTimeframe()
{
    for (std::string p : orderBook.getKnownProducts())
    {
        logBook.write("Matching: " + p);
        
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime, username);

        logBook.write("Sales: " + std::to_string(sales.size()) + "\n");

        for (OrderBookEntry& sale : sales)
        {
            logBook.write("Sale price: " 
                         + std::to_string(sale.price) 
                         + " amount: " 
                         + std::to_string(sale.amount)+ "\n"); 

            if (sale.username == "bot" && sales.size() < 2)
            {
                // update the wallet
                wallet.processSale(sale);
            }
            else
            {
                //if number of sales is > 2 withdraw order;
                sales.pop_back();
                logBook.write("*****Sale withdrawn " + p 
                             + " price: " + std::to_string(sale.price) 
                             + "/" + " amount: " +  std::to_string(sale.amount) 
                             +  "\n"  + "number of sales remaining: " 
                             + std::to_string(sales.size())); 
            }
        }
    }

    currentTime = orderBook.getNextTime(currentTime);
    std::cout << "\n" + currentTime  << std::endl;
    logBook.write("\n" "Going to next time frame: " + currentTime + "\n");
}

void MerkelBot::printWallet()
{
    std::cout << wallet.toString() << std::endl;
    logBook.write("Wallet contains: " "\n" + wallet.toString());
}
 
