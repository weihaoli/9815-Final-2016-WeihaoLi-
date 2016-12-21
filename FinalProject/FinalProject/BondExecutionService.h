#pragma once
#include"CommonDirectory.h"
#include"executionservice.hpp"
#include"BondAlgoExecutionService.h"

class BondExecutionService_Connector : public Connector<ExecutionOrder<Bond>> {
private:
	string _thefile;
public:
	BondExecutionService_Connector(string _filename) :_thefile(_filename) {

	}
	// Publish data to the Connector
	virtual void Publish( ExecutionOrder<Bond> &data)  override;

	

};
void BondExecutionService_Connector::Publish( ExecutionOrder<Bond> &data)  {
	ofstream out_file(_thefile, ios::app);
	PricingSide _side = data.GetPricingSide();
	string b_a;
	if (_side == BID)
		b_a = "BID";
	else
		b_a = "OFFER";
	string O_type;
	switch (data.GetOrderType())
	{
	case FOK:O_type = "FOK"; 
		break;
	case IOC:O_type = "IOC";
		break;
	case MARKET:O_type = "MARKET";
		break;
	case LIMIT:O_type = "LIMIT";
		break;
	case STOP:O_type = "STOP";
		break;
	default: O_type = "OTHER";
		break;
	}

	out_file <<"  Order ID  "<<data.GetOrderId()<<"  Order Type "<<O_type<< "  " << data.GetProduct().GetProductId() <<"   "<< b_a<<"     "
	<< data.GetPrice() << "    visible  " << data.GetVisibleQuantity()
		<< "  Hidden " << data.GetHiddenQuantity() << "  Parent  " << data.GetParentOrderId() << "\n";

	
}







class BondExecutionService :public ExecutionService<Bond> {
public:
	BondExecutionService_Connector* _OutConnector;
	vector<ServiceListener<ExecutionOrder<Bond>>*> Listeners;
	ExecutionOrder<Bond> _nothing;
public:


	BondExecutionService(BondExecutionService_Connector& _InConnector) :_OutConnector(&_InConnector) {};

	void GenerateOrder(AlgoExecutionOrder _order);
	// Execute an order on a market
	void ExecuteOrder(ExecutionOrder<Bond>& order, Market market) override;



	void OnMessage(ExecutionOrder<Bond> &data) override {};
	void AddListener(ServiceListener<ExecutionOrder<Bond>>*listener) override { Listeners.push_back(listener); };
	const vector<ServiceListener<ExecutionOrder<Bond>>*>& GetListeners() const override { return Listeners; };
	ExecutionOrder<Bond>& GetData(string key) override {

		return _nothing;
	};

};

void BondExecutionService::GenerateOrder(AlgoExecutionOrder _Algoorder) {
	ExecutionOrder<Bond> in_put= _Algoorder.GetExecutionOrder();
	ExecuteOrder(in_put, CME);
	//	ExecutionOrder(const T &_product, PricingSide _side, const string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId, bool _isChildOrder);

}

void BondExecutionService::ExecuteOrder( ExecutionOrder<Bond>& order, Market market) {
	_OutConnector->Publish(order);
	for (ServiceListener<ExecutionOrder<Bond>>* listener : Listeners)
		(*listener).ProcessAdd(order);
}





// BondStream To AlgoStream Listener
class AlgoToBondExecute_Listener :public ServiceListener<AlgoExecutionOrder> {
private:
	BondExecutionService* _BondExecuteService;
public:
	AlgoToBondExecute_Listener(BondExecutionService &in_service) : _BondExecuteService(&in_service) {

	}
	void ProcessAdd(AlgoExecutionOrder&) override;
};
void AlgoToBondExecute_Listener::ProcessAdd(AlgoExecutionOrder& in_AlgoExecution) {
	
	_BondExecuteService->GenerateOrder(in_AlgoExecution);
}
