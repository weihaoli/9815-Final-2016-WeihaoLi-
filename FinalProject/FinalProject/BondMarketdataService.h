#pragma once
#include"CommonDirectory.h"
#include"marketdataservice.hpp"


class BondMarketdataService :public MarketDataService<Bond> {
private:
	map<string, OrderBook<Bond>> OrderBooks_Map;
	vector<ServiceListener<OrderBook<Bond>>*> Listeners;
public:
	// Get the best bid/offer order
	virtual const BidOffer GetBestBidOffer(const string &productId) override;
	// Get data on our service given a key
	virtual OrderBook<Bond>& GetData(string cusip) override;

	//const OrderBook<Bond>& AggregateDepth(const string &productId) override;

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(OrderBook<Bond> &data) override;

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<OrderBook<Bond>> *) override;

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<OrderBook<Bond>>* >& GetListeners() const ;
};


const BidOffer BondMarketdataService::GetBestBidOffer(const string &cusip) {
	Order best_bid = (OrderBooks_Map[cusip].GetBidStack()).at(0);
	Order best_offer = (OrderBooks_Map[cusip].GetOfferStack()).at(0);
	BidOffer best_bid_offer(best_bid, best_offer);
	return best_bid_offer;

}

OrderBook<Bond>&  BondMarketdataService::GetData(string cusip) {
	return OrderBooks_Map[cusip];
}





void BondMarketdataService::OnMessage(OrderBook<Bond> &data) {
	
	OrderBooks_Map[data.GetProduct().GetProductId()] = data;
	for (ServiceListener<OrderBook<Bond>>*listener : Listeners)
		listener->ProcessAdd(data);
}



// Add a listener to the Service for callbacks on add, remove, and update events
// for data to the Service.
void BondMarketdataService::AddListener(ServiceListener<OrderBook<Bond>> *listener) {

	Listeners.push_back(listener);

};

const vector< ServiceListener<OrderBook<Bond>>* >& BondMarketdataService::GetListeners() const{
	return Listeners;

}




class BondMarketdataConnector : public Connector<OrderBook<Bond>> {
private:

	BondMarketdataService* _Marketdata_Service;
	BondProductService* _Product_Service;

public:
	BondMarketdataConnector() {};
	BondMarketdataConnector(BondMarketdataService& input_Service, BondProductService& BPS) :_Marketdata_Service(&input_Service), _Product_Service(&BPS) {

	}

	void Read_File(string);

	

};


void BondMarketdataConnector::Read_File(string filename) {
	ifstream in_file(filename);


	std::string line, par;

	//parameters for trade
	vector<Order> _bidstack,_offerstack;
	double _bidprice, _offerprice;
	double _bidquantity, _offerquantity;
//	PricingSide _side;
	string cusip;
	
	Bond new_Bond;
	//read the data by line and flow them into BookTrading Service
	double count = 0;
	while (std::getline(in_file, line))
	{
		
		
		stringstream lineStream(line);
		
		//prepare the paramater needed for a oderbook
		getline(lineStream, par, ',');
			
			 cusip = par;
			_bidstack.clear();
			_offerstack.clear();
		
		//construct a bid order with data 1 - 15
		for(int i = 0;i<5;i++){
			
			getline(lineStream, par, ',');
			_bidprice = String_To_Price(par);
			
			getline(lineStream, par, ',');
		
			_bidquantity = stod(par);
			
			getline(lineStream, par, ',');
			

			Order bid_orderOrder(_bidprice, _bidquantity, BID);
			_bidstack.push_back(bid_orderOrder);
			
		}
		for (int i = 0; i<5; i++) {
			
				getline(lineStream, par, ',');
			
			    _offerprice = String_To_Price(par);
				getline(lineStream, par, ',');
				
				_offerquantity = stod(par);
				getline(lineStream, par, ',');
				

				Order offer_orderOrder(_offerprice, _offerquantity, OFFER);
				_offerstack.push_back(offer_orderOrder);
		}
			OrderBook<Bond> new_Orderbook(_Product_Service->GetData(cusip), _bidstack, _offerstack);

			(*_Marketdata_Service).OnMessage(new_Orderbook);
		}
}




