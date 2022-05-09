#pragma once

#include <string>
#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"

class Advisorbot
{
public:

	Advisorbot();
	void init();

private:

	OrderBook orderBook{ "20200601.csv" };
	std::string currentTime;

	std::string getUserOption();
	void processUserOption(const std::string& userOption);
	std::vector<std::string> commandParser(const std::string input, char separator);

	void printMenu();
	void printHelpCmd(const std::vector<std::string>& tokens);
	void printProducts();

	void printMin(const std::vector<std::string>& tokens);
	void printMax(const std::vector<std::string>& tokens);
	void printAvg(const std::vector<std::string>& tokens);
	void compareProducts(const std::vector<std::string>& tokens);

	void gotoNextTimeframe();
	void reset();

	void predict(const std::vector<std::string>& tokens);
	float timestampProdPrices(std::vector<OrderBookEntry> orders);
	float stepPriceChange(const std::vector<OrderBookEntry>& orders);

	bool inputValidator(const std::vector<std::string>& tokens);
	std::vector<std::string> getKnownCommands();
};