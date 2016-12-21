#pragma once
#include"CommonDirectory.h"
#include"streamingservice.hpp"
#include"pricingservice.hpp"
#include"products.hpp"
#include"BondAlgoStreamingService.h"

class BondStreamingService_Connector : public Connector<PriceStream <Bond>> {
private:
	string _thefile;
public: 
	BondStreamingService_Connector(string _filename):_thefile(_filename) {

	}
	// Publish data to the Connector
	virtual void Publish(PriceStream<Bond> &data)  override;



};
void BondStreamingService_Connector ::Publish( PriceStream<Bond> &data)  {
	
	ofstream out_file(_thefile,ios::app);
	out_file << "  " << data.GetProduct().GetProductId() << "  BID    " << data.GetBidOrder().GetPrice() << "    " << data.GetBidOrder().GetVisibleQuantity()
		<< "  OFFER    " << data.GetOfferOrder().GetPrice() << "    " << data.GetOfferOrder().GetVisibleQuantity()<<"\n";

}




class BondStreamingService :public StreamingService<Bond> {
private:
	vector<ServiceListener<PriceStream <Bond>>*> Listeners;
	BondStreamingService_Connector* _OutConnector;
	PriceStream <Bond> _nothing;
public:
	BondStreamingService() {};
	BondStreamingService(BondStreamingService_Connector& _InConnector) :_OutConnector(&_InConnector) {};


	// send price to Market
	void PublishPrice( PriceStream<Bond>& priceStream) override;
	void AlgoToPirceStream(const Algostream& _instream);



	void OnMessage(PriceStream<Bond> &data) override {};
	void AddListener(ServiceListener<PriceStream<Bond>>*listener) override { Listeners.push_back(listener); };
	virtual const vector<ServiceListener<PriceStream <Bond>>*>& GetListeners() const override { return Listeners; };
	PriceStream <Bond>& GetData(string key) override{

		return _nothing;
	};
};

void BondStreamingService::AlgoToPirceStream(const Algostream& _instream) {
	PriceStream<Bond>in_put = _instream.GetPriceStream();
	PublishPrice(in_put);

}



void BondStreamingService::PublishPrice( PriceStream<Bond>& priceStream) {

	_OutConnector->Publish(priceStream);
}


// BondStream To AlgoStream Listener
class BondstreamToAlgoStream_Listener :public ServiceListener<Algostream> {
private:
	BondStreamingService* _BondstreamService;
public:
	BondstreamToAlgoStream_Listener(BondStreamingService &in_service) : _BondstreamService(&in_service) {

	}
	void ProcessAdd(Algostream&) override;
};
void BondstreamToAlgoStream_Listener::ProcessAdd(Algostream& in_AlgoStream) {
	_BondstreamService->AlgoToPirceStream(in_AlgoStream);
}






