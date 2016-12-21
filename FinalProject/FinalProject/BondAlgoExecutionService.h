#pragma once
#pragma once
#include"CommonDirectory.h"
#include"executionservice.hpp"

using namespace std;




/**
* An execution order that can be placed on an exchange.
* Type T is the product type.
*/
/**
* class AlgoExecution
*/
class AlgoExecutionOrder {
private:
	ExecutionOrder<Bond> _executionOrder;
public:
	// ctor
	AlgoExecutionOrder() {};
	AlgoExecutionOrder( OrderBook<Bond> o); // use algorithm to transform book message to ExecutionOrder classes

											// getter
	ExecutionOrder<Bond> GetExecutionOrder() const;

	
};

AlgoExecutionOrder::AlgoExecutionOrder( OrderBook<Bond> o) {
	auto product = o.GetProduct();
	// some strategy
	int id = o.GetBidStack()[1].GetPrice()*11111;
	string orderID = "O" + to_string(id);
	PricingSide side = ((id / 2) ? BID : OFFER);
	

	double price = 0;
	long  visualQuantity = 0;
	long hiddenQuantity = 0;
	if (side == BID) {
		price = o.GetBidStack()[0].GetPrice();
		visualQuantity = o.GetBidStack()[0].GetQuantity();
		hiddenQuantity = 0;
	}
	else {
		price = o.GetOfferStack()[0].GetPrice();
		visualQuantity = o.GetOfferStack()[0].GetQuantity();
		hiddenQuantity = 0;

	}

	
	string parentID = orderID + "P";
	bool isChild = id / 2;


	ExecutionOrder<Bond> New_Execution(product, side, orderID, OrderType(id%5), price, visualQuantity, hiddenQuantity, parentID, isChild);
	_executionOrder = New_Execution;
}


ExecutionOrder<Bond> AlgoExecutionOrder::GetExecutionOrder() const {



	return _executionOrder;


};






class AlgoExecutionService:public Service<string,AlgoExecutionOrder>
{

private:
	AlgoExecutionOrder _nothing;
	vector<ServiceListener<AlgoExecutionOrder>*> Listeners;
public:
	void ExecutionAlgoOrder(OrderBook<Bond>& _orderBook);
	
	
	//general functions for this class
	AlgoExecutionOrder& GetData(string cusip) override { return _nothing; };
	void OnMessage(AlgoExecutionOrder &data) override {};
	void AddListener(ServiceListener<AlgoExecutionOrder> *listener) override {
		Listeners.push_back(listener);};
	virtual const vector< ServiceListener<AlgoExecutionOrder>* >& GetListeners() const override { return Listeners; };
};


void AlgoExecutionService::ExecutionAlgoOrder(OrderBook<Bond>& _orderBook) 
{


	AlgoExecutionOrder _Executeout(_orderBook);
	
	for (ServiceListener<AlgoExecutionOrder>* lstn : Listeners)
		lstn->ProcessAdd(_Executeout);
}




// MaketToAlgoService
class MarketToAlgoExecution_Listener :public ServiceListener<OrderBook<Bond>> {
private:
	AlgoExecutionService* _AlgoexecuteService;
public:
	 MarketToAlgoExecution_Listener(AlgoExecutionService &in_service) : _AlgoexecuteService(&in_service) {

	}
	void ProcessAdd(OrderBook<Bond>&) override;
};
void MarketToAlgoExecution_Listener::ProcessAdd(OrderBook<Bond>& in_orderbook) {
	
	_AlgoexecuteService->ExecutionAlgoOrder(in_orderbook);
	
}



