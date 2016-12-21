#pragma once
#include"CommonDirectory.h"
#include"pricingservice.hpp"

class BondPricingService:public PricingService<Bond>{

private:
	map<string, Price<Bond>> Price_List;
	vector<ServiceListener<Price<Bond>> *> Listeners;
public:

	// Get data on our service given a key
	virtual Price<Bond>& GetData(string cusip) override;

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Price<Bond> &data) override;

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Price<Bond>> *) override;

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Price<Bond>>* >& GetListeners() const override;

};

Price<Bond>& BondPricingService::GetData(string cusip) {
	return Price_List[cusip];
}

void BondPricingService::OnMessage(Price<Bond> &data) {
	if (Price_List.find(data.GetProduct().GetProductId()) == Price_List.end())
		Price_List.insert(pair<string, Price<Bond>>(data.GetProduct().GetProductId(), data));
	Price_List[data.GetProduct().GetProductId()].GetMid() = data.GetMid();
	Price_List[data.GetProduct().GetProductId()].GetBidOfferSpread() = data.GetBidOfferSpread();
	for (ServiceListener<Price<Bond>>* listener : Listeners)
		listener->ProcessAdd(data);

}

void BondPricingService::AddListener(ServiceListener<Price<Bond>> * listener) {
	Listeners.push_back(listener);
}

const vector< ServiceListener<Price<Bond>>* >& BondPricingService::GetListeners() const {
	return Listeners;
}


class BondPricingConnector : public Connector<Price<Bond>> {
private:
	
	BondPricingService* _Pricing_Service;
	BondProductService* _Product_Service;

public:
	BondPricingConnector() {};
	BondPricingConnector(BondPricingService& input_Service, BondProductService& BPS) :_Pricing_Service(&input_Service), _Product_Service(&BPS) {

		}

	void Read_File(string); 

	void Publish( Price<Bond>&) override {};

	};


void BondPricingConnector::Read_File(string filename){
		ifstream in_file(filename);


		std::string line, par;

		//parameters for trade
		double mid; double spread; 
		string cusip; 

		
		//read the data by line and flow them into BookTrading Service

		while (std::getline(in_file, line))
		{

			stringstream lineStream(line);
			//	 read a parameter of a trade
			getline(lineStream, par, ',');
			cusip = par;
			
			getline(lineStream, par, ',');
		    mid = String_To_Price(par);
			
			getline(lineStream, par, ',');
			spread = String_To_Price(par);
			



			Price<Bond> new_Price(_Product_Service->GetData(cusip), mid,spread);

			(*_Pricing_Service).OnMessage(new_Price);





		}


	//read the next line of the txt, and parse the paramater to make it a trade, and then call on_message to call the 
	//corresponding service

	

	//add product in to the map


};


