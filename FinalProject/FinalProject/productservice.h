#pragma once
/**
* productservice.hpp defines Bond and IRSwap ProductServices
*/

#include <iostream>
#include <map>
#include "products.hpp"
#include "soa.hpp"

/**
* Bond Product Service to own reference data over a set of bond securities.
* Key is the productId string, value is a Bond.
*/
class BondProductService 
{

public:
	// BondProductService ctor
	BondProductService();

	// Return the bond data for a particular bond product identifier
	Bond& GetData(string productId);

	// Add a bond to the service (convenience method)
	void Add(Bond &bond);

private:
	map<string, Bond> bondMap; // cache of bond products

};



BondProductService::BondProductService()
{
	bondMap = map<string, Bond>();
}

Bond& BondProductService::GetData(string productId)
{
	return bondMap[productId];
}

void BondProductService::Add(Bond &bond)
{
	bondMap.insert(pair<string, Bond>(bond.GetProductId(), bond));
}
