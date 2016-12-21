#pragma once
#include"CommonDirectory.h"
#include"tradebookingservice.hpp"

class BondTradeBookingService : public TradeBookingService<Bond>
{
private:

	map<string, Trade<Bond>> TradingBook;
	vector<ServiceListener<Trade<Bond>>*> Listeners;
public:

	BondTradeBookingService();
	~BondTradeBookingService() {};

	// Book the trade
	void BookTrade(Trade<Bond> &trade);



	// Get trade on our service given TradeID
	Trade<Bond>& GetData(string key) override{ return TradingBook[key]; };
	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Trade<Bond> &data) override;

	// Add a listener to the Service 
	virtual void AddListener(ServiceListener<Trade<Bond>> *listener) ;
	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Trade<Bond>>* >& GetListeners() const;

};

//constructor
BondTradeBookingService::BondTradeBookingService() {};




//Book Trade
void BondTradeBookingService::BookTrade(Trade<Bond> &trade)
{

	TradingBook.insert(pair<string, Trade<Bond>>(trade.GetTradeId(), trade));
	for (ServiceListener<Trade<Bond>>* listener : Listeners)
		(*listener).ProcessAdd(trade);

}


void BondTradeBookingService::OnMessage(Trade<Bond> &data)
{
	BookTrade(data);
}

void BondTradeBookingService::AddListener(ServiceListener<Trade<Bond>> *_listener)
{
	Listeners.push_back(_listener);
}

const vector< ServiceListener<Trade<Bond>>* >& BondTradeBookingService::GetListeners() const
{
	return Listeners;
}














class BondTradingConnector : public Connector<Trade<Bond>> {
private:
	ifstream in_file;
	BondTradeBookingService* _Service;
	
	
public:
	BondTradingConnector() {};
	BondTradingConnector(string filename, BondTradeBookingService& input_Service, BondProductService& BPS) :_Service(&input_Service), in_file(filename) {
		
		std::string line, par;
		
		//parameters for trade
		long bid_price; long quantity;
		string cusip; string trade_id; string book;
		Side _side;
		
		Bond New_Bond;
		//read the data by line and flow them into BookTrading Service
		
		while (std::getline(in_file, line))
		{
			
			stringstream lineStream(line);
			//	 read a parameter of a trade
			getline(lineStream, par, ',');
			 cusip = par;
			getline(lineStream, par, ',');
			trade_id = par;
			getline(lineStream, par, ',');
		    book = par;
			getline(lineStream, par, ',');
			bid_price = String_To_Price(par);
			getline(lineStream, par, ',');
			quantity = stoi(par);
			getline(lineStream, par, ',');
			
			if (par == "BUY") _side = BUY;
			else _side = SELL;
		
			

			Trade<Bond> new_Trade(BPS.GetData(cusip), trade_id, book, quantity, _side);
			
			(*_Service).OnMessage(new_Trade);
	}
};
	
	//read the next line of the txt, and parse the paramater to make it a trade, and then call on_message to call the 
	//corresponding service

	
	//add product in to the map
	

};











