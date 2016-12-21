#pragma once
#include"CommonDirectory.h"
#include"positionservice.hpp"
#include"productservice.h"





class BondPositionService:public PositionService<Bond> {
private:
	//map<cusip, position>
	map<string, Position<Bond>> Position_Map;
	vector<ServiceListener<Position<Bond>>*> Listeners;
public:
	
	// Get position of a specific bond( map<book, quantity>) on our service given a cusip
	Position<Bond>& GetData(string cusip);

	void AddTrade(const Trade<Bond> &trade) override;
	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Position<Bond> &data) override ;

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	 void AddListener(ServiceListener<Position<Bond>> *listener) override {
		
		 Listeners.push_back(listener);

	};

	// Get all listeners on the Service.
	 virtual const vector< ServiceListener<Position<Bond>>* >& GetListeners() const override{ return Listeners; };

};

Position<Bond>& BondPositionService::GetData(string key) { return Position_Map[key]; };
void BondPositionService::OnMessage(Position<Bond> &data) {
	

}







void BondPositionService::AddTrade(const Trade<Bond> &trade) {
	string cusip = trade.GetProduct().GetProductId();
	string book = trade.GetBook();
	double amount = 0;
	if (trade.GetSide() == BUY) amount = 1;
	else amount = -1;
	amount *= trade.GetQuantity();
	Position_Map[cusip].SetPosition(book, amount);
	Position_Map[cusip].SetProduct(trade.GetProduct());
	for (ServiceListener<Position<Bond>>* listener : Listeners) {
		
		(*listener).ProcessAdd(Position_Map[cusip]);
	}
	}

// BondTradebookingToPossition Listener
class TradeBookingToPosition_Listener :public ServiceListener<Trade<Bond>> {
private:
	BondPositionService* PositionService;
public:
	TradeBookingToPosition_Listener(BondPositionService &in_service): PositionService(&in_service){

	}
	void ProcessAdd(Trade<Bond>&) override;
};
void TradeBookingToPosition_Listener::ProcessAdd(Trade<Bond>& in_Trade) {
	PositionService->AddTrade(in_Trade);
}
