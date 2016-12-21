#include"BondTradeBooking.h"
#include"CommonDirectory.h"
#include"BondPosition.h"
#include"productservice.h"
#include"BondRiskService.h"
#include"BondPricingService.h"
#include"BondMarketdataService.h"
#include"executionservice.hpp"
#include"BondStreamingService.h"
#include"BondExecutionService.h"
#include"BondInquiryservice.h"
#include"historicaldataservice.hpp"
int main() {


	/**********************************************************************************/
	//				      Initialize Product    									  //
	/**********************************************************************************/



	
	//Bonds 
	string cusip1 = "912828M72"; date maturity2Y(2017, Dec, 31); Bond B2Y(cusip1, CUSIP, "T", 2, maturity2Y);
	string cusip2 = "912828N22"; date maturity3Y(2018, Dec, 31); Bond B3Y(cusip2, CUSIP, "T", 3, maturity3Y);
	string cusip3 = "912828M98"; date maturity5Y(2020, Dec, 31); Bond B5Y(cusip3, CUSIP, "T", 5, maturity5Y);
	string cusip4 = "912828M80"; date maturity7Y(2022, Dec, 31); Bond B7Y(cusip4, CUSIP, "T", 7, maturity7Y);
	string cusip5 = "912828M56"; date maturity10Y(2025, Dec, 31); Bond N10Y(cusip5, CUSIP, "T", 10, maturity10Y);
	string cusip6 = "912810RP5"; date maturity30Y(2045, Dec, 31); Bond N30Y(cusip6, CUSIP, "T", 30, maturity30Y);
	
	
	//grouped by busket
	vector<Bond> front_end{ B2Y, B3Y };
	BucketedSector<Bond> front_end_sector(front_end, "FrontEnd");
	vector<Bond> belly{ B5Y, B7Y, N10Y };
	BucketedSector<Bond> belly_sector(belly, "Belly");
	vector<Bond> long_end{ N30Y };
	BucketedSector<Bond> long_end_sector(long_end, "LongEnd");
	
	
	// add a product service
	BondProductService Bond_Product;
	Bond_Product.Add(B2Y);
	Bond_Product.Add(B3Y);
	Bond_Product.Add(B5Y);
	Bond_Product.Add(B7Y);
	Bond_Product.Add(N10Y);
	Bond_Product.Add(N30Y);
	
	
	
	/**********************************************************************************/
	//				      Initialize Service    									  //
	/**********************************************************************************/


	BondTradeBookingService TradeBookingService;
	BondPositionService PositionService;
	BondRiskService RiskService;
	BondPricingService PricingService;
	BondMarketdataService MarketdataService;
	AlgostreamService _AlgoStreamingService;
	AlgoExecutionService _AlgoExecutionService;
	//creat IniquiryService and it's connector
	BondInquiryService _InquiryService;
	
	//the two only Service need to out_put file
	//Execept Historycal Service, Which I will test it later,after testing all these
	
	//Create a Streaming connector and lINKED it to StreamingService
	BondStreamingService_Connector StreamingService_Connector("Out_File_Streaming.txt");
	BondStreamingService _StreamingService(StreamingService_Connector);
	
	//Create a Execution connector and lINKED it to ExecutionService
	BondExecutionService_Connector _ExecutionService_Connector("Out_File_Execution.txt");
	BondExecutionService _ExecutionService(_ExecutionService_Connector);
	//Create a Inquiry connector and lINKED it to Inquiry Service
	Inquiry_Connector _Inquiry_Connector(_InquiryService, Bond_Product);
	
	
	//Create a History connector and lINKED it to HistorydataService
	//We just test Risk Here for Inquiries Service here for orther Service just change the
	//templatet type To PV01<Bond>,Position<Bond>,etc
	HistoricalData_Connector<Inquiry<Bond>> HistoricalSevice_Connector("Out_File_History.txt");
	HistoricalDataService<Inquiry<Bond>> HistoricalService(HistoricalSevice_Connector);
	


	
	/**********************************************************************************/
	//				      Use Listener to Connect Service							  //
	/**********************************************************************************/
	
	
	
	//Listener : TradeBooking and  Position
	TradeBookingToPosition_Listener Listener_P2TB(PositionService);
	TradeBookingService.AddListener(&Listener_P2TB);

	
	//Listener: Position and Risk
	PositionToRisk_Listener Listener_R2P(RiskService);
	PositionService.AddListener(&Listener_R2P);

	//Listener: Pricing and AlgoStream
	PricingToAlgostream_Listener Listener_Price2AL(_AlgoStreamingService);
	PricingService.AddListener(&Listener_Price2AL);
	
	//Listener: AlgoStreaming and Streaming
	BondstreamToAlgoStream_Listener Listener_Bond2Al(_StreamingService);
	_AlgoStreamingService.AddListener(&Listener_Bond2Al);

	//Listener: Marketdata and AlgoExecution
	MarketToAlgoExecution_Listener Market2AL(_AlgoExecutionService);
	MarketdataService.AddListener(&Market2AL);

	//Listener: AlgoExecution and Execution
	AlgoToBondExecute_Listener Listener_Al2Ex(_ExecutionService);
	_AlgoExecutionService.AddListener(&Listener_Al2Ex);

	//Listener: Position and Historydata
	HistoricalData_Listener<Inquiry<Bond>> HistoricalService_Listener(HistoricalService);
	_InquiryService.AddListener(&HistoricalService_Listener);


	/**********************************************************************************/
	//				      Read              Data   									  //
	/**********************************************************************************/
	
	////Initialized the Connector and ReadDta 
	BondTradingConnector BT_Connector("trades.txt", TradeBookingService, Bond_Product);
	////Create and initialized Pricing Connector
	BondPricingConnector BP_Connector(PricingService, Bond_Product);
	BP_Connector.Read_File("price.txt");
	////Create a connector and read data for market data Service
	BondMarketdataConnector BM_Conector(MarketdataService, Bond_Product);
	BM_Conector.Read_File("market_data.txt");
	// test Inquiry Service
	_Inquiry_Connector.Subscribe("inquiries.txt");









	/**********************************************************************************/
	//				      Run And Testing     									      //
	/**********************************************************************************/













	////test the TradeBookingService
	std::cout << "\n**************************\n";
	std::cout << "\n     test Trade Booking   \n";
	std::cout << "\n**************************\n";
	std::cout <<"\n Product Information :" <<TradeBookingService.GetData("T4").GetProduct();
	std::cout <<"\n              Side   :" <<TradeBookingService.GetData("T4").GetSide();
	std::cout <<"\n          Quantity   :" <<TradeBookingService.GetData("T4").GetQuantity();
	std::cout <<"\n          Trade ID   :" <<TradeBookingService.GetData("T4").GetTradeId();
	std::cout <<"\n         Trading Book:" <<TradeBookingService.GetData("T4").GetBook();
	
	
	//test the Possition Service
	std::cout << "\n\n\n\n**************************\n";
	std::cout << "\n     test  Position Service   \n";
	std::cout << "\n**************************\n";
	string ID = "TRSY3";
	std::cout << "\n TestBond 912828M72\n\n";
	std::cout << "\n    Product Information : " << PositionService.GetData("912828M72").GetProduct();
	std::cout << "\n     agregated position : "<< PositionService.GetData("912828M72").GetAggregatePosition();
	std::cout << "\n position in book TRSY3 : " << PositionService.GetData("912828M72").GetPosition(ID);
	
	////test the risk service

	std::cout << "\n\n\n\n**************************\n";
	std::cout << "\n     test Risk Service   \n";
	std::cout << "\n**************************\n";


	std::cout << "\n Look at quantity Associated with  Bond 912828M72: " << RiskService.GetData("912828M72").GetQuantity();
	std::cout << "\n             risk Associated with  Bond 912828M72: " << RiskService.GetData("912828M72").GetPV01();
	std::cout << "\n                    PV01   assiociated with belly: " << RiskService.GetBucketedRisk(belly_sector).GetPV01();
	std::cout << "\n               Quantity bonds are in belly sector: " << RiskService.GetBucketedRisk(belly_sector).GetQuantity();



	std::cout << "\n\n\n\n**************************\n";
	std::cout << "\n     test Prcing Service   \n";
	std::cout << "\n**************************\n";


	std::cout << "\n       Latest mid Price of  Bond 912828M72: " << PricingService.GetData("912828M72").GetMid();
	std::cout << "\n          Latest spread of  Bond 912828M72: " << PricingService.GetData("912828M72").GetBidOfferSpread();
	


	std::cout << "\n\n\n\n**************************\n";
	std::cout << "\n     test Prcing Service   \n";
	std::cout << "\n**************************\n";

	//test the Marketdata Service
	std::cout << "\n\n\n\n**************************\n";
	std::cout << "\n     test  MarketData   \n";
	std::cout << "\n**************************\n";

	
	OrderBook<Bond> order_book_2Y = MarketdataService.AggregateDepth("912828M72");
	cout << "The latest orderbook of 912828M72\n";
	cout << "\n  Best bid price  : " << order_book_2Y.GetBidStack()[0].GetPrice();
	cout << "\n   with quantity  : " << order_book_2Y.GetBidStack()[0].GetQuantity();
	cout << "\n Best offer price : "<< order_book_2Y.GetOfferStack()[0].GetPrice();
	cout << "\n   with quantity  :" << order_book_2Y.GetOfferStack()[0].GetQuantity()<< "\n\n";
	

	//test the Inquiry Service
	std::cout << "\n\n\n\n**************************\n";
	std::cout << "\n    test  Inquiry Service\n";
	std::cout << "\n**************************\n";
	string Iquiry_id = "000008";
	std::cout<<"Test the #8 Receive Bond Inquiry :"<<_InquiryService.GetData(Iquiry_id);


	
	std::cout << "\n\n\n\n*********************************************************\n";
	std::cout << "\n    To Check Execution/Streaming/History Service See correponding 'Out_File XXX.txt. ";
	std::cout << "\n****************************************************************\n";
	
	std::cin.get();


	


}