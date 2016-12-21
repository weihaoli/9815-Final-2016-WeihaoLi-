#pragma once
#include"CommonDirectory.h"
#include"streamingservice.hpp"
#include"pricingservice.hpp"
#include"products.hpp"

class Algostream {
private:
	PriceStream<Bond> _PriceStream;
	double bit_price;
	double offer_price;
	double bit_quant;
	double offer_quant;
	Bond _Product;
public:
	Algostream() {};
	Algostream(Price<Bond>&_inprice);
	PriceStream<Bond> GetPriceStream() const{
		return _PriceStream;
	}

};

Algostream::Algostream(Price<Bond>&_inprice ) {

	_Product = _inprice.GetProduct();
	bit_price = _inprice.GetMid() - _inprice.GetBidOfferSpread() / 2;
	offer_price = _inprice.GetMid() + _inprice.GetBidOfferSpread() / 2;
	bit_quant = 1000000;
	offer_quant = 1000000;
	PriceStreamOrder bit_order(bit_price, bit_quant, 0, BID);
	PriceStreamOrder offer_order(offer_price, offer_quant, 0, OFFER);
	_PriceStream = PriceStream<Bond>(_Product, bit_order, offer_order);
}


class AlgostreamService :public Service<string, Algostream> {
private:
	Algostream _nothing;
	vector<ServiceListener<Algostream>*> Listeners;
public:
	void StreamAlgoOrder(Price<Bond>& _price);


	
	
	
	void OnMessage(Algostream &data) override {};
	void AddListener(ServiceListener<Algostream>*listener) override {Listeners.push_back(listener);};
	virtual const vector< ServiceListener<Algostream>*>& GetListeners() const override { return Listeners; };
	Algostream& GetData(string key) override {
		return _nothing;
	}
};


void AlgostreamService::StreamAlgoOrder(Price<Bond>& _price) {
	
	Algostream _Streamout(_price);
	for (ServiceListener<Algostream>* lstn : Listeners)
		lstn->ProcessAdd(_Streamout);
}


// Pricing to Algostream Listener
class PricingToAlgostream_Listener :public ServiceListener<Price<Bond>> {
private:
	AlgostreamService* _AlgostreamService;
public:
	PricingToAlgostream_Listener(AlgostreamService &in_service) : _AlgostreamService(&in_service) {

	}
	void ProcessAdd(Price<Bond>&) override;
};
void PricingToAlgostream_Listener::ProcessAdd(Price<Bond>& in_Price) {
	
	_AlgostreamService->StreamAlgoOrder(in_Price);
}






