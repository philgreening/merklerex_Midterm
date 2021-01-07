#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>
#include "logBook.h"

class OrderBook
{
    public:
    /** construct, reading a csv data file */
        OrderBook(std::string filename);
    /** return vector of all know products in the dataset*/
        std::vector<std::string> getKnownProducts();
    /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product, 
                                              std::string timestamp);

        // std::vector<std::string> getTimeStamp(std::string timestamp);
        // std::vector<std::string> getTimeStamp1();


        /** returns the earliest time in the orderbook*/
        std::string getEarliestTime();
        /** returns the next time after the 
         * sent time in the orderbook  
         * If there is no next timestamp, wraps around to the start
         * */
        std::string getNextTime(std::string timestamp);

        std::string getPrevTime(std::string timestamp);

    /** inserts new order into orderbook */    
        void insertOrder(OrderBookEntry& order);
    /** matching engine to work match orders*/
        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp, std::string username);

    /** find and returns the highest price of currency from a vector of orders */
        static double getHighPrice(std::vector<OrderBookEntry>& orders);
    /** find and returns the lowest price of currency from a vector of orders */
        static double getLowPrice(std::vector<OrderBookEntry>& orders);
    /** works out and returns average price of currency from a vector of orders */
        static double getAveragePrice(std::vector<OrderBookEntry>& orders);
    /** works out previous timestamp average amount of currency from a vector of orders */
         static double getPrevAveragePrice(std::vector<OrderBookEntry>& orders);
    /** works out average amount of currency from a vector of orders */
        static double getAverageAmount(std::vector<OrderBookEntry>& orders);

        //static double getEMA(std::vector<OrderBookEntry>& orders);
        
    private:
        std::vector<OrderBookEntry> orders;
        //double emaArray = orders[0].price;
        //LogBook logBook;



};
