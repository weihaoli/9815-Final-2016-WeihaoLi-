/**
 * historicaldataservice.hpp
 * historicaldataservice.hpp
 *
 * @author Breman Thuraisingham
 * Defines the data types and Service for historical data.
 *
 * @author Breman Thuraisingham
 */
#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP
template<typename T>
class HistoricalData_Connector : public Connector<T> {
private:
	string _thefile;
public:
	HistoricalData_Connector(string _filename) :_thefile(_filename) {

	}
	// Publish data to the Connector
	virtual void Publish(T &data)  override {
		ofstream out_file(_thefile, ios::app);
		 out_file << data;

	
	};



};





/**
 * Service for processing and persisting historical data to a persistent store.
 * Keyed on some persistent key.
 * Type T is the data type to persist.
 */
template<typename T>
class HistoricalDataService : Service<string,T>
{
private:
	T _nothing;
	vector<ServiceListener<T>*> Listeners;
	HistoricalData_Connector<T>* _OutConnector;
	
public:
	HistoricalDataService(HistoricalData_Connector<T>& _inConnector) :_OutConnector(&_inConnector) {};
  // Persist data to a store
  void PersistData(string persistKey, T& data);


  void OnMessage(T &data) override {};
  void AddListener(ServiceListener<T>*listener) override { Listeners.push_back(listener); };
  virtual const vector<ServiceListener<T>*>& GetListeners() const override { return Listeners; };
  T& GetData(string key) override {

	  return _nothing;
  };




};
template<typename T>
void HistoricalDataService<T>::PersistData(string persistKey,  T& data)
{
	_OutConnector->Publish(data);

}




// Listener
template<typename T>
class HistoricalData_Listener :public ServiceListener<T> {
private:
	HistoricalDataService<T> * _HisDataService;
public:
	HistoricalData_Listener(HistoricalDataService<T> &in_service) : _HisDataService(&in_service) {
}


	void ProcessAdd(T&) override;
};
template<typename T>
void HistoricalData_Listener<T>::ProcessAdd(T& in_info) {
	
	_HisDataService->PersistData("ok",in_info);
}

#endif
