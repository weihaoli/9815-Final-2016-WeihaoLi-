#pragma once
#include"CommonDirectory.h"
#include"inquiryservice.hpp"

class BondInquiryService : public InquiryService<Bond> {
private:
	map<string, Inquiry<Bond>> Inquiry_Map;
	vector<ServiceListener<Inquiry<Bond>>*> Listeners;
public:
	// Send a quote back to the client
	void SendQuote(const string &inquiryId, double price)override {};

	// Reject an inquiry from the client
	void RejectInquiry(const string &inquiryId) override {};






	void OnMessage(Inquiry<Bond> &data) override;
	Inquiry<Bond>& GetData(string I_id) {
		return Inquiry_Map[I_id];
	}
	void AddListener(ServiceListener<Inquiry<Bond>> *listener) override {
		Listeners.push_back(listener);
	};
	virtual const vector< ServiceListener<Inquiry<Bond>>*>& GetListeners() const override { return Listeners; };


};

void BondInquiryService::OnMessage(Inquiry<Bond> &data) {
	Inquiry_Map[data.GetInquiryId()] = data;
	for (ServiceListener<Inquiry<Bond>>* listener : Listeners)
		listener->ProcessAdd(data);


}







class Inquiry_Connector : public Connector<Inquiry<Bond>>{
private:
	BondInquiryService* _InquiryService;
	BondProductService* _product;
public:
	Inquiry_Connector(BondInquiryService& _inbi, BondProductService& _inpd) :_InquiryService(&_inbi), _product(&_inpd) {};


	void Subscribe(string _filename);
	virtual void Publish( Inquiry<Bond> &data)  override;

};


void Inquiry_Connector::Subscribe(string _filename) {
	ifstream in_file(_filename);
	string line, par;
	string inquiryid, productid, state;
	Side _side;
	long quant;
	double price;

	while (std::getline(in_file, line))
	{

		stringstream lineStream(line);
		//	 read a parameter of a trade
		getline(lineStream, par, ',');
		inquiryid = par;

		getline(lineStream, par, ',');
		productid = par;

		getline(lineStream, par, ',');
		_side = (par == "BUY") ? BUY : SELL;

		getline(lineStream, par, ',');
		quant = stoi(par);

		getline(lineStream, par, ',');
		price = String_To_Price(par);

		getline(lineStream, par, ',');

		Inquiry<Bond> _inquiry(inquiryid, _product->GetData(productid), _side, quant, price, RECEIVED);
		_InquiryService->SendQuote(_inquiry.GetProduct().GetProductId(), 100);
		_inquiry.GetPrice() = 100.0;
		_inquiry.GetState() = QUOTED;
		Publish(_inquiry);
	}

}



void Inquiry_Connector::Publish(Inquiry<Bond> &data)  {
	double charge;
	charge = (data.GetSide() == BUY) ? -1.0 / 128.0 : 1.0 / 128.0;
	Side _side1 = (data.GetSide() == BUY) ? SELL : BUY;
	data.GetPrice()+= charge;
	data.GetSide() = _side1;
	data.GetState() = DONE;
	_InquiryService->OnMessage(data);

}