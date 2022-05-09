#include "CSVReader.h"
#include <iostream>
#include <fstream>

CSVReader::CSVReader()
{
}

std::vector<OrderBookEntry> CSVReader::readCSV(const std::string& csvFilename)
{
	std::vector<OrderBookEntry> entries;

	std::ifstream csvFile{ csvFilename };
	std::string line;
	if (csvFile.is_open())
	{
		while (std::getline(csvFile, line))
		{
			try
			{
				OrderBookEntry obe = stringsToOBE(tokenise(line, ','));
				entries.push_back(obe);
			}
			catch (const std::exception &e)
			{
				std::cout << "CSVReader::readCSV bad data" << std::endl;
			}
		}
	}
	else
	{
		std::cout << "CSV file is not open" << std::endl;
	}

	std::cout << "CSVReader::readCSV read " << entries.size() << " entries" << std::endl;
	return entries;
}

/// <summary>
/// Read a CSV line from a file and tokenise each line by using a character separator as a delimeter between future tokens.
/// </summary>
/// <param name="csvLine">A line from a CSV file.</param>
/// <param name="separator">: a character like "space".</param>
/// <returns>: A vector of strings, each string being a word on its own.</returns>
std::vector<std::string> CSVReader::tokenise(const std::string& csvLine, char separator)
{
	std::vector<std::string> tokens;
	std::string temp;
	temp.reserve(50);// reserve capacity of the string we will surely need

	for (std::size_t i = 0, size = csvLine.length(); i < size; i++)
	{
		if (csvLine[i] != separator) // if this character is not a separator, add it to the temp string
		{
			temp += csvLine[i];
		}
		else
		{
			// if separator is found and temp has characters stored in it
			if (!temp.empty())
			{
				tokens.push_back(temp); // push that temp string to the vector of tokens
				temp.clear(); //empty the temp variable
				temp.reserve(50); //reserve character capacity again
			}
		}
	}
	if (!temp.empty())
		tokens.push_back(temp);

	return tokens;
}

OrderBookEntry CSVReader::stringsToOBE(const std::vector<std::string>& tokens)
{
	float price;
	double amount;

	if (tokens.size() != 5) // bad
	{
		std::cout << "Bad line " << std::endl;
		throw std::exception{};
	}
	// we have 5 tokens
	try
	{
		price = std::stof(tokens[3]);
		amount = std::stod(tokens[4]);
	}
	catch (const std::exception &e)
	{
		std::cout << "CSVReader::stringsToOBE Bad float! " << tokens[3] << std::endl;
		std::cout << "CSVReader::stringsToOBE Bad float! " << tokens[4] << std::endl;
		throw;
	}

	OrderBookEntry obe{ price,
					   amount,
					   tokens[0],
					   tokens[1],
					   OrderBookEntry::stringToOrderBookType(tokens[2]) };

	return obe;
}

OrderBookEntry CSVReader::stringsToOBE(const std::string& priceString,
	const std::string& amountString,
	const std::string& timestamp,
	const std::string& product,
	OrderBookType orderType)
{
	float price;
	double amount;
	try
	{
		price = std::stod(priceString);
		amount = std::stod(amountString);
	}
	catch (const std::exception &e)
	{
		std::cout << "CSVReader::stringsToOBE Bad float! " << priceString << std::endl;
		std::cout << "CSVReader::stringsToOBE Bad float! " << amountString << std::endl;
		throw;
	}
	OrderBookEntry obe{ price,
					   amount,
					   timestamp,
					   product,
					   orderType };

	return obe;
}
