#pragma once
#include"CommonDirectory.h"
#include"riskservice.hpp"
// Bond Risk Service

class BondRiskService : public RiskService<Bond> {

private:
	map<string, PV01<Bond>> PVOVS;
	map<string, double> PV01_value;//assuming PV01 value for bonds
	vector<ServiceListener<PV01<Bond>>*> Listeners;
public:
	// Initialzation : initialize the Bondservice with given PV01value and bonds
	BondRiskService();

	
	// Add a position that the service will risk
	virtual void AddPosition(Position<Bond> &) override;

	// Get the bucketed risk for the bucket sector
	 PV01<BucketedSector<Bond>> GetBucketedRisk(const BucketedSector<Bond> &) override;
	

	// return the pv01 of specific bond
	virtual PV01<Bond>& GetData(string cusip) override;

	// This is useless
	virtual void OnMessage(PV01<Bond> &data) override{};

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<PV01<Bond>> *) ;

	const vector< ServiceListener<PV01<Bond>>* >& GetListeners() const override {
		return Listeners;
	}
};

//for simplisity let's just set up PV01 in initialization
BondRiskService::BondRiskService() {
	PV01_value.insert(pair<string, double>("912828M72", 0.91));
	PV01_value.insert(pair<string, double>("912828N22", 0.92));
	PV01_value.insert(pair<string, double>("912828M98", 0.93));
	PV01_value.insert(pair<string, double>("912828M80", 0.94));
	PV01_value.insert(pair<string, double>("912828M56", 0.95));
	PV01_value.insert(pair<string, double>("912810RP5", 0.96));
	

}

void BondRiskService::AddPosition(Position<Bond> & position) {
	string product_id = position.GetProduct().GetProductId(); 
	double new_quantity = position.GetAggregatePosition();
	double new_pv01 = new_quantity*PV01_value[product_id];
	PVOVS[product_id] = PV01<Bond>(position.GetProduct(), new_pv01, new_quantity);
}




  PV01<BucketedSector<Bond>> BondRiskService::GetBucketedRisk(const BucketedSector<Bond> & sector)  {
	//first we need to load all product 
	vector<string> cusips;
	for (Bond p: sector.GetProducts()) {
		cusips.push_back(p.GetProductId());
	}
	double sum_pv01 = 0;
	double sum_quant = 0;
	for (string _c : cusips) {
		sum_pv01 += PVOVS[_c].GetPV01()* PVOVS[_c].GetQuantity();
		sum_quant += PVOVS[_c].GetQuantity();
	}
	PV01<BucketedSector<Bond>> Bucket_PV01( sector, sum_pv01, sum_quant);

	
	return  Bucket_PV01;

}





// return the pv01 of specific bond
PV01<Bond>& BondRiskService::GetData(string cusip) {

	return PVOVS[cusip];

};



// Add a listener to the Service for callbacks on add, remove, and update events
// for data to the Service.
void BondRiskService::AddListener(ServiceListener<PV01<Bond>> * listener) {

	Listeners.push_back(listener);

};



// RiskToPosition Listener
class PositionToRisk_Listener :public ServiceListener<Position<Bond>> {
private:
	BondRiskService* RiskService;
public:
	PositionToRisk_Listener(BondRiskService &in_service) : RiskService(&in_service) {

	}
	void ProcessAdd(Position<Bond>&) override;
};
void PositionToRisk_Listener ::ProcessAdd(Position<Bond>& in_Trade) {
	RiskService->AddPosition(in_Trade);
}