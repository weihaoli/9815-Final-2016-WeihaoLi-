/**
 * positionservice.hpp
 * Defines the data types and Service for positions.
 *
 * @author Breman Thuraisingham
 */
#ifndef POSITION_SERVICE_HPP
#define POSITION_SERVICE_HPP

#include <string>
#include <map>
#include "soa.hpp"
#include "tradebookingservice.hpp"
#include<numeric>

using namespace std;

/**
 * Position class in a particular book.
 * Type T is the product type.
 */
template<typename T>
class Position
{

public:
	Position() {};
  // ctor for a position
  Position(const T &_product);

  // Get the product
  const T& GetProduct() const;

  // Get the position quantity
  long GetPosition(string &book);
  //
  friend ostream& operator << (ostream& os,  Position& t) {
	  os << "  ID: " << t.GetProduct().GetProductId(); 
	  for (auto p : t.GetPpositions())
		 os << " Book " << p.first << "  amount " << p.second;
     os << " Total "<< t.GetAggregatePosition()<<"\n";
	  return os;
  }



  // Get the aggregate position
  long GetAggregatePosition();
  void SetPosition(string &book, long amount);
  void SetProduct(T in_product);

  
  map<string, long>& GetPpositions() { return positions;};
private:
  T product;
  //book name ,amount
  map<string,long> positions;

};
template<typename T>
void Position<T>::SetPosition( string &book, long amount)
{

		positions[book] += amount;
}

template<typename T>
void Position<T>::SetProduct(T in_product)
{

	product = in_product;
}

/**
 * Position Service to manage positions across multiple books and secruties.
 * Keyed on product identifier.
 * Type T is the product type.
 */



template<typename T>
class PositionService : public Service<string,Position <T> >
{

public:
	

	// Get all listeners on the Service.
	
  // Add a trade to the service
  virtual void AddTrade(const Trade<T> &trade) = 0;

};

//position constructor
template<typename T>
Position<T>::Position(const T &_product) :product(_product)
{
}

template<typename T>
const T& Position<T>::GetProduct() const
{
  return product;
}

template<typename T>
long Position<T>::GetPosition(string &book)
{
  return positions[book];
}





template<typename T>
long Position<T>::GetAggregatePosition()
{
  // No-op implementation - should be filled out for implementations
	long sum_position = 0;
	for (auto m : positions) {
		sum_position += m.second;

	}

	return sum_position;
}

#endif
