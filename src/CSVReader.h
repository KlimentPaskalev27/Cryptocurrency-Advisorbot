#pragma once

#include "OrderBookEntry.h"
#include <vector>
#include <string>


class CSVReader
{
public:
	CSVReader();

	static std::vector<OrderBookEntry> readCSV(const std::string& csvFile);
	static std::vector<std::string> tokenise(const std::string& csvLine, char separator);

	static OrderBookEntry stringsToOBE(const std::string& price,
		const std::string& amount,
		const std::string& timestamp,
		const std::string& product,
		OrderBookType OrderBookType);

private:
	static OrderBookEntry stringsToOBE(const std::vector<std::string>& strings);

};