#include "MerkelBot.h"
#include <iostream>

MerkelBot::MerkelBot()
{

}

void MerkelBot::init()
{
    //R1A retrieves order book
    currentTime = orderBook.getEarliestTime();
    wallet.insertCurrency("BTC", 10);
    auto count = 0;
   

     for (std::string const& t : orderBook.getTimeStamp())
     //for(auto e : orderBook.getOrders())
     //for (OrderBookEntry& e : orderBook)
    //for (int i = 0 ; i <= currentTime.size(); i++)  
    {
        predictFuture();
        submitAsk();
        submitBid();
        gotoNextTimeframe();
        printWallet();
        count ++;
    }
    std::cout << count << std::endl;

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

// void MerkelBot::calculateEma()
// {

// }

void MerkelBot::submitBid()
{

    for (std::string const& p : orderBook.getKnownProducts())
    {
        //TODO: prediction algorithm
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        

        std::string bid = std::to_string(OrderBook::getAveragePrice(entries));
        std::string amount = std::to_string(OrderBook::getAverageAmount(entries));
        std::string input = p + ',' + bid + ','+ amount;
        std::cout << "Bid input: "<< input << std::endl;
        std::vector<std::string> tokens = CSVReader::tokenise(input, ',');


    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;

    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
        // std::cout << "Bids seen: " << entries.size() << std::endl;
        // std::cout << "Max bid: " << OrderBook::getHighPrice(entries) << std::endl;
        // std::cout << "Min bid: " << OrderBook::getLowPrice(entries) << std::endl;
        std::cout << currentTime << std::endl;

    }
   
    // std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    // std::string input;
    // std::getline(std::cin, input);

    
}

void MerkelBot::submitAsk()
{

    for (std::string const& p : orderBook.getKnownProducts())
    {
        //TODO: prediction algorithm
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::vector<OrderBookEntry> entries1 = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);                              
        

        std::string bid = std::to_string(OrderBook::getAveragePrice(entries1));
        std::string amount = std::to_string(OrderBook::getAverageAmount(entries1));
        std::string input = p + ',' + bid + ','+ amount;
        std::cout << "Ask input: "<< input << std::endl;

        std::vector<std::string> tokens = CSVReader::tokenise(input, ',');


    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;

    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
        // std::cout << "Asks seen: " << entries.size() << std::endl;
        // std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        // std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
        std::cout << currentTime << std::endl;

    }
   
    // std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    // std::string input;
    // std::getline(std::cin, input);    
}

void MerkelBot::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
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
 
