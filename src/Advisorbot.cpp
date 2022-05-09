#include "Advisorbot.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <stdio.h>
#include <string.h>
#include <map>
#include <algorithm>

Advisorbot::Advisorbot()
{
}

void Advisorbot::init()
{
	std::string input;
	currentTime = orderBook.getEarliestTime();
	printMenu();
	while (true)
	{
		std::cout << "Reminder: use the 'help' and 'help command' commands to check usage and details." << std::endl;
		input = getUserOption();
		processUserOption(input);
	}
}

/// <summary>
/// Takes console input and prints to the user what they selected. It checks for input validity and then returns an integer, which is lated used in processUserOption().
/// </summary>
/// <returns>integer between 1 and 7</returns>
std::string Advisorbot::getUserOption()
{
	std::string userOption;
	std::string line;

	std::cout << " " << std::endl;
	std::cout << "Type in one of the commands from the menu." << std::endl;
	std::cout << "help, prod, min, max, avg, compare, predict, time, step, reset" << std::endl;
	std::cout << " " << std::endl;

	std::getline(std::cin, line);

	try {
		userOption = line;
	}
	catch (const std::exception& e)
	{
		std::cout << "Error with input. Exception is caught." << std::endl;
	}

	std::cout << "======================================================" << std::endl;
	std::cout << "Processing your inputs: '" << userOption << "'" << std::endl;
	std::cout << "======================================================" << std::endl;

	return userOption;
}

/// <summary>
/// Takes the input of the user and parses it into a number of tokens using a character separator. Creates a vector of those string tokens.
/// </summary>
/// <param name="input">: string input, something like "help avg" or "help prod".</param>
/// <param name="separator">: A character like blank space.</param>
/// /// <returns>: a vector of token strings.</returns>
std::vector<std::string> Advisorbot::commandParser(std::string input, char separator)
{
	std::vector<std::string> tokens;
	std::string temp;
	//reserve capacity for the temp string that we will need. Commands don't tend to go above 20 chars usually.
	temp.reserve(20);

	// check if blank space is used consecutive times, trim them down and rewrite command string with single spaces as separator
	int spaceCounter = 0;
	for (int i = 0; i == input.length(); i++)
	{
		//for each identified space, add to counter
		if ( input[i] == separator)
		{
			spaceCounter++;
		}

		// if there are more than one spaces, save this range of characters
		if (spaceCounter > 1)
		{
			input = input.substr(0, i);
		}

		//checks if there are same consecutive characters. Used to trim down several whitespaces into one.
		std::string::iterator new_end = std::unique(input.begin(), input.end());
		input.erase(new_end, input.end());

		//reset
		spaceCounter = 0;
	}
	// result from above will be a string that will only have single spaces as separator between potential command and parameters

	// by using char separator, push to a vector each word a token
	for (std::size_t i = 0, size = input.length(); i < size; i++)
	{
		//if this char is not a separator, add it to n-th token string
		if (input[i] != separator)
		{
			temp += input[i];
		}
		else // if we reach a separator, push token of chars created until now to a vector of tokens
		{
			if (!temp.empty())
			{
				tokens.push_back(temp);
				temp.clear(); //clear the string
				temp.reserve(20); //again reserve some capacity
			}
		}
	}
	if (!temp.empty())
		tokens.push_back(temp);

	return tokens;
}

/// <summary>
/// Takes user input string as parameter from commandParser() and then based on user input runs a member function.
/// </summary>
/// <param name="userOption">: string which matches a list of expected commands, otherwise inform for bad input.</param>
void Advisorbot::processUserOption(const std::string& userOption)
{
	std::cout << " " << std::endl;

	//tokenise the user input
	std::vector<std::string> tokens = commandParser(userOption, ' ');

	if (userOption.find("help") != std::string::npos && tokens[0] == "help")
	{
		if (tokens.size() == 1 && userOption == "help")
		{
			std::cout << "'help' prints out the menu of commands. Showing menu below:" << std::endl;
			printMenu();
		}
		else {
			printHelpCmd(tokens);
		}
	}
	else if (userOption == "prod")
	{
		printProducts();
	}
	else if (userOption.find("min") != std::string::npos)
	{
		printMin(commandParser(userOption, ' ') );
	}
	else if (userOption.find("max") != std::string::npos)
	{
		printMax(tokens);
	}
	else if (userOption.find("avg") != std::string::npos)
	{
		printAvg(tokens);
	}
	else if (userOption.find("compare") != std::string::npos)
	{
		compareProducts(tokens) ;
	}
	else if (userOption.find("predict") != std::string::npos)
	{
		predict(tokens);
	}
	else if (userOption == "step")
	{
		gotoNextTimeframe();
	}
	else if (userOption == "time")
	{
		std::cout << "Current time is now:  " << currentTime << std::endl;
	}
	else if (userOption == "reset")
	{
		reset();
	}
	else // bad input
	{
		std::cout << "Command Input (first word) must be from given commands" << std::endl;
	}
	std::cout << " " << std::endl;
}


/// <summary>
/// Shows the menu. Lists all available commands.
/// </summary>
void Advisorbot::printMenu()
{
	std::cout << " " << std::endl;
	std::cout << "======================================================" << std::endl;
	std::cout << "Use one the following commands + parameters." << std::endl;
	std::cout << "======================================================" << std::endl;
	std::cout << "help " << std::endl;
	std::cout << "help <command> " << std::endl;
	std::cout << "prod " << std::endl;
	std::cout << "min <product> <ask/bid>" << std::endl;
	std::cout << "max <product> <ask/bid>" << std::endl;
	std::cout << "avg <product> <ask/bid> <steps>" << std::endl;
	std::cout << "compare <product_1> <product_2> <ask/bid>" << std::endl;
	std::cout << "predict <product> <ask/bid>" << std::endl;
	std::cout << "time " << std::endl;
	std::cout << "step " << std::endl;
	std::cout << "reset " << std::endl;
	std::cout << "======================================================" << std::endl;
	std::cout << "Current time is: " << currentTime << std::endl;
	std::cout << "======================================================" << std::endl;
	std::cout << "To get more information on each command and its parameters, type in 'help (and the name of the command)'." << std::endl;
	std::cout << "Example: help prod" << std::endl;
	std::cout << "======================================================" << std::endl;
	std::cout << " " << std::endl;
}

/// <summary>
/// A command which will show extra details on how to use a specified command as the second input.
/// </summary>
/// <param name="tokens"></param>
void Advisorbot::printHelpCmd(const std::vector<std::string>& tokens)
{
	std::cout << " " << std::endl;

	if (tokens[1] == "help")
	{
		if (tokens.size() > 2)
		{
			if (tokens[2].find("command") != std::string::npos)
			{
				std::cout << "The 'help cmd' command will show additional help information for the specified command." << std::endl;
				std::cout << "Input parameters: COMMAND NAME" << std::endl;
				std::cout << "Example: help avg" << std::endl;
			}
		}
		else {
			std::cout << "The 'help' command will list all the available commands." << std::endl;
			std::cout << "Input parameters: NONE" << std::endl;
			std::cout << "Example: help " << std::endl;
		}
	}
	else if (tokens[1] == "prod")
	{
		std::cout << "The 'prod' command will list all available products. " << std::endl;
		std::cout << "Input parameters: NONE" << std::endl;
		std::cout << "Example: prod" << std::endl;
	}
	else if (tokens[1] == "min")
	{
		std::cout << "The 'min' command will show the lowest ask/bid price of a product in this timestep. " << std::endl;
		std::cout << "Input parameters:  PRODUCT NAME , TYPE (ask, bid)" << std::endl;
		std::cout << "Example: min ETH/USDT ask" << std::endl;
	}
	else if (tokens[1] == "max")
	{
		std::cout << "The 'max' command will show the highest ask/bid price of a product in this timestep. " << std::endl;
		std::cout << "Input parameters:  PRODUCT NAME , TYPE (ask, bid)" << std::endl;
		std::cout << "Example: max DOGE/BTC bid" << std::endl;
	}
	else if (tokens[1] == "avg")
	{
		std::cout << "The 'avg' command will show the average ask/bid price of a product in the last N steps, N specified by user. " << std::endl;
		std::cout << "Input parameters: PRODUCT NAME , TYPE (ask, bid) , STEPS (number)" << std::endl;
		std::cout << "Example: avg DOGE/BTC bid 3" << std::endl;
	}
	else if (tokens[1] == "compare")
	{
		std::cout << "The 'compare' command will show the average ask/bid price difference between two products" << std::endl;
		std::cout << "which the user selects from data up until the current time step." << std::endl;
		std::cout << "Input parameters: PRODUCT 1 NAME , PRODUCT 2 NAME , TYPE (ask, bid) " << std::endl;
		std::cout << "Example: compare ETH/USDT BTC/USDT ask" << std::endl;
	}
	else if (tokens[1] == "predict")
	{
		std::cout << "The 'predict' command will show the possible next ask/bid price of a product selected by the user. " << std::endl;
		std::cout << "It iterates through all the prices for the entire past time of a single product" << std::endl;
		std::cout << "and by calculating the average change in price, it estimates what the next will be." << std::endl;
		std::cout << "Input parameters:  PRODUCT NAME , TYPE (ask, bid) " << std::endl;
		std::cout << "Example: predict ETH/USDT bid" << std::endl;
	}
	else if (tokens[1] == "time")
	{
		std::cout << "The 'time' command will show the current timestamp / timestep. " << std::endl;
		std::cout << "Input parameters: NONE" << std::endl;
		std::cout << "Example: time" << std::endl;
	}
	else if (tokens[1] == "step")
	{
		std::cout << "The 'step' command will switch to the next timestamp / timestep. " << std::endl;
		std::cout << "Input parameters: NONE" << std::endl;
		std::cout << "Example: step" << std::endl;
	}
	else if (tokens[1] == "reset")
	{
		std::cout << "The 'reset' command will reset the timestamp / timestep to the initial from the start. " << std::endl;
		std::cout << "Input parameters: NONE" << std::endl;
		std::cout << "Example: reset" << std::endl;
	}
	else 
	{
		std::cout << "Invalid input.Please use a valid command." << std::endl;
	}
	std::cout << " " << std::endl;
}

/// <summary>
/// Prints all the products from the tokenised .csv file.
/// </summary>
void Advisorbot::printProducts()
{
	std::cout << " " << std::endl;
	std::cout << "======================================================" << std::endl;
	std::cout << "Printing products: " << std::endl;
	std::cout << "======================================================" << std::endl;
	
	for (std::string const& p : orderBook.getKnownProducts())
	{
		std::cout << p << std::endl;
	}

	std::cout << "======================================================" << std::endl;
	std::cout << " " << std::endl;
}

/// <summary>
/// Moves the simulation to the next timestamp. Switches from one time step into the next. Time steps are determined by the CSV rows' data after it is tokenised when creating an orderBook object.
/// </summary>
void Advisorbot::gotoNextTimeframe()
{
	std::cout << "Going to next time frame. " << std::endl;
	currentTime = orderBook.getNextTime(currentTime);
	std::cout << "Current time is now:  " << currentTime  << std::endl;
}


/// <summary>
/// Prints out the lowest price of a specified product and ask/bid type in current timestep.
/// </summary>
/// <param name="tokens"></param>
void Advisorbot::printMin(const std::vector<std::string>& tokens)
{
	if (Advisorbot::inputValidator(tokens))
	{
		if (tokens.size() == 3)
		{
			std::string type = tokens[2];
			std::string product = tokens[1];
			std::string minToString;
			std::vector<OrderBookEntry> entries;
			float min;

			if (type == "ask")
			{
				entries = orderBook.getOrders(OrderBookType::ask, product, currentTime);
				min = OrderBook::getLowPrice(entries);
				minToString = std::to_string(min);
			}
			else if (type == "bid")
			{
				entries = orderBook.getOrders(OrderBookType::bid, product, currentTime);
				min = OrderBook::getLowPrice(entries);
				minToString = std::to_string(min);
			}
			else {
				std::cout << "Bad input for bid/ask type. Please select one of both." << std::endl;
			}
			std::cout << "Lowest " << type << " price for " << product << " is " << minToString << std::endl;
		} 
		else {
			std::cout << "You've missed a part of the command, it must contain 3 words. Example: 'min ETH/USDT ask'" << std::endl;
		}
	}
	else {
		std::cout << "Bad input. Try again." << std::endl;
	}
}

/// <summary>
/// Prints out the highest price of specified product and ask/bid type in current timestep.
/// </summary>
/// <param name="tokens"></param>
void Advisorbot::printMax(const std::vector<std::string>& tokens)
{
	if (Advisorbot::inputValidator(tokens))
	{
		if (tokens.size() == 3)
		{
			std::string type = tokens[2];
			std::string product = tokens[1];
			std::string maxToString;
			std::vector<OrderBookEntry> entries;
			float max;

			if (type == "ask")
			{
				entries = orderBook.getOrders(OrderBookType::ask, product, currentTime);
				max = OrderBook::getHighPrice(entries);
				maxToString = std::to_string(max);
			}
			else if (type == "bid")
			{
				entries = orderBook.getOrders(OrderBookType::bid, product, currentTime);
				max = OrderBook::getHighPrice(entries);
				maxToString = std::to_string(max);
			}
			else {
				std::cout << "Bad input for bid/ask type. Please select one of both." << std::endl;
			}
			std::cout << "Highest " << type << " price for " << product << " is " << maxToString << std::endl;
		}
		else {
			std::cout << "You've missed a part of the command, it must contain 3 words. Example: 'min ETH/USDT ask'" << std::endl;
		}
	}
	else {
		std::cout << "Bad input. Try again." << std::endl;
	}
}

/// <summary>
/// Prints the average price of given product of given bid/ask type from the last N time steps. N being the number of steps the user has input.
/// </summary>
/// <param name="tokens"></param>
/// <param name="steps"></param>
void Advisorbot::printAvg(const std::vector<std::string>& tokens)
{
	if (Advisorbot::inputValidator(tokens))
	{
		std::string type = tokens[2];
		std::string product = tokens[1];
		std::string trackTime = orderBook.getEarliestTime();
		std::vector<OrderBookEntry> orders;
		float timestampPrices; // the prices of a product in one specific timestamp
		float avg = 0;
		//Initialize both below at 1 to cover the current time step
		int stepsToCurrent = 1; // counts the time steps taken by the user until now. 
		int stepToStartAt = 1; // the time step at which trackTime will begin until it reaches currentTime
		int steps;

		// if no steps are specified with command, then default is only for this time step check average price
		if (tokens.size() == 4)
		{
			steps = stoi(tokens[3]);
		}
		else {
			steps = 1;
			std::cout << "Advisorbot could not see any steps as an input so it sets current time step as default." << std::endl;
		}

		while (trackTime != currentTime)
		{
			trackTime = orderBook.getNextTime(trackTime);
			stepsToCurrent++;
		}

		// input validation
		if (steps > stepsToCurrent)
		{
			std::cout << "The number of steps you've input is larger than the steps taken in the simulation. Please choose a number up to " << stepsToCurrent << std::endl;
			return;
		}
		stepToStartAt = stepsToCurrent - steps;

		//reset time tracker and move it to step to start counting from for avrg
		trackTime = orderBook.getEarliestTime();
		for (int i  = 0; i < stepToStartAt; i++)
		{
			trackTime = orderBook.getNextTime(trackTime);
			//std::cout << "Moving trackTime to step: " << i << std::endl;
		}

		if (type == "ask")
		{
			if (trackTime == currentTime)
			{
				orders = orderBook.getOrders(OrderBookType::ask, product, trackTime);
				timestampPrices = timestampProdPrices(orders);
				avg = avg + timestampPrices;
			}
			else {
				while (trackTime != currentTime)
				{
					orders = orderBook.getOrders(OrderBookType::ask, product, trackTime);
					timestampPrices = timestampProdPrices(orders);
					avg = avg + timestampPrices;
					trackTime = orderBook.getNextTime(trackTime);
				}
			}
			avg = avg / steps;
			std::cout << "Average ASK price of " << product << " in the last " << steps << " timesteps is " << avg << std::endl;
		}
		else if (type == "bid")
		{
			if (trackTime == currentTime)
			{
				orders = orderBook.getOrders(OrderBookType::bid, product, trackTime);
				timestampPrices = timestampProdPrices(orders);
				avg = avg + timestampPrices;
			}
			else {
				while (trackTime != currentTime)
				{
					orders = orderBook.getOrders(OrderBookType::bid, product, trackTime);
					timestampPrices = timestampProdPrices(orders);
					avg = avg + timestampPrices;
					trackTime = orderBook.getNextTime(trackTime);
				}
			}
			avg = avg / steps;
			std::cout << "Average BID price of " << product << " in the last " << steps << " timesteps is " << avg << std::endl;
		}
		else {
			std::cout << "Bad input for bid/ask type. Please select one of both." << std::endl;
		}
	}
	else {
		std::cout << "Bad input. Try again." << std::endl;
	}
}

/// <summary>
/// Used by the printAvg() function for the 'avg' command. Takes all the orders passed into it and sums their prices up.
/// </summary>
/// <param name="orders"></param>
/// <returns></returns>
float Advisorbot::timestampProdPrices(std::vector<OrderBookEntry> orders)
{
	float avg = 0;

	for (OrderBookEntry& order : orders)
	{
		avg = avg + order.price;
	}
	avg = avg / orders.size();
	return avg;
}

void Advisorbot::compareProducts(const std::vector<std::string>& tokens)
{
	if (Advisorbot::inputValidator(tokens))
	{
		std::string product_1 = tokens[1];
		std::string product_2 = tokens[2];
		std::string type = tokens[3];
		std::string trackTime = orderBook.getEarliestTime();
		std::vector<OrderBookEntry> orders_1;
		std::vector<OrderBookEntry> orders_2;
		float timestampPrices_1; // the prices of a product in one specific timestamp
		float timestampPrices_2;
		float diff = 0;
		int stepsToCurrent; // counts the time steps taken by the user until now

		// stepsToCurrent begins at 1 because current timestamp is considered
		for (stepsToCurrent = 1; trackTime != currentTime; stepsToCurrent++)
		{
			trackTime = orderBook.getNextTime(trackTime);
		}
	

		//reset time tracker and move it to step to start counting from for avrg
		trackTime = orderBook.getEarliestTime();


		if (type == "ask")
		{
			if (trackTime == currentTime)
			{
				orders_1 = orderBook.getOrders(OrderBookType::ask, product_1, trackTime);
				orders_2 = orderBook.getOrders(OrderBookType::ask, product_2, trackTime);
				timestampPrices_1 = timestampProdPrices(orders_1);
				timestampPrices_2 = timestampProdPrices(orders_2);
				diff = diff + (timestampPrices_1 - timestampPrices_2);
			}
			else {
				while (trackTime != currentTime)
				{
					orders_1 = orderBook.getOrders(OrderBookType::ask, product_1, trackTime);
					orders_2 = orderBook.getOrders(OrderBookType::ask, product_2, trackTime);
					timestampPrices_1 = timestampProdPrices(orders_1);
					timestampPrices_2 = timestampProdPrices(orders_2);
					diff = diff + (timestampPrices_1 - timestampPrices_2);
					trackTime = orderBook.getNextTime(trackTime);
				}
			}
			diff = diff / stepsToCurrent;
			std::cout << "Average ASK price difference between " << product_1 << " and " << product_2 << " is " << diff << std::endl;
		}
		else if (type == "bid")
		{
			if (trackTime == currentTime)
			{
				orders_1 = orderBook.getOrders(OrderBookType::bid, product_1, trackTime);
				orders_2 = orderBook.getOrders(OrderBookType::bid, product_2, trackTime);
				timestampPrices_1 = timestampProdPrices(orders_1);
				timestampPrices_2 = timestampProdPrices(orders_2);
				diff = timestampPrices_1 - timestampPrices_2;
			}
			else {
				while (trackTime != currentTime)
				{
					orders_1 = orderBook.getOrders(OrderBookType::bid, product_1, trackTime);
					orders_2 = orderBook.getOrders(OrderBookType::bid, product_2, trackTime);
					timestampPrices_1 = timestampProdPrices(orders_1);
					timestampPrices_2 = timestampProdPrices(orders_2);
					diff = timestampPrices_1 - timestampPrices_2;
					trackTime = orderBook.getNextTime(trackTime);
				}
			}
			diff = diff / stepsToCurrent;
			std::cout << "Average BID price difference between " << product_1 << " and " << product_2 << " is " << diff << std::endl;
		}
		else {
			std::cout << "Bad input for bid/ask type. Please select one of both." << std::endl;
		}
	}
	else {
		std::cout << "Bad input. Try again." << std::endl;
	}
}

/// <summary>
/// Resets the time step to the first one.
/// </summary>
void Advisorbot::reset()
{
	currentTime = orderBook.getEarliestTime();
	std::cout << "Time is now reset back to: " << currentTime << std::endl;
}

/// <summary>
/// Predicts the next ask/bid value of a product by estimating the average change in price up until this point.
/// </summary>
/// <param name="tokens"></param>
void Advisorbot::predict(const std::vector<std::string>& tokens)
{
	if (Advisorbot::inputValidator(tokens))
	{
		std::string type = tokens[2];
		std::string product = tokens[1];
		std::string trackTime = orderBook.getEarliestTime();
		std::vector<OrderBookEntry> orders;
		float avgPriceChange = 0;
		int stepsToCurrent; // counts the time steps taken by the user until now
		float predictedPrice = 0;
		float lastKnownPrice;

		for (stepsToCurrent = 1; trackTime != currentTime; stepsToCurrent++)
		{
			trackTime = orderBook.getNextTime(trackTime);
		}

		//reset time tracker and move it to step to start counting from for avrg
		trackTime = orderBook.getEarliestTime();

		if (type == "ask")
		{
			if (trackTime == currentTime)
			{
				orders = orderBook.getOrders(OrderBookType::ask, product, trackTime);
				avgPriceChange = avgPriceChange + stepPriceChange(orders);
			}
			else {
				while (trackTime != currentTime)
				{
					orders = orderBook.getOrders(OrderBookType::ask, product, trackTime);
					avgPriceChange = avgPriceChange + stepPriceChange(orders);
					trackTime = orderBook.getNextTime(trackTime);
				}
			}
			orders = orderBook.getOrders(OrderBookType::ask, product, currentTime);
			lastKnownPrice = orders.back().price; //the price of the last element of the vector
			//stepsToCurrent++; // Starting step is regarded as step 1.
			avgPriceChange = avgPriceChange / stepsToCurrent;
			std::cout << "======================================================" << std::endl;
			std::cout << "The average change in ASK price for " << product <<  " is " << avgPriceChange << std::endl;
			predictedPrice = lastKnownPrice + avgPriceChange;
			std::cout << "By knowing the average price change in all passed steps"<< std::endl;
			std::cout << "And by knowing that the last known price is  " << lastKnownPrice << std::endl;
			std::cout << "Advisorbot predicts that the next ASK price of " << product << " is going to be " << predictedPrice << std::endl;
			std::cout << "======================================================"<< std::endl;
		}
		else if (type == "bid")
		{
			if (trackTime == currentTime)
			{
				orders = orderBook.getOrders(OrderBookType::bid, product, trackTime);
				avgPriceChange = avgPriceChange + stepPriceChange(orders);
			}
			else {
				while (trackTime != currentTime)
				{
					orders = orderBook.getOrders(OrderBookType::bid, product, trackTime);
					avgPriceChange = avgPriceChange + stepPriceChange(orders);
					trackTime = orderBook.getNextTime(trackTime);
				}
			}
			orders = orderBook.getOrders(OrderBookType::bid, product, currentTime);
			lastKnownPrice = orders.back().price; //the price of the last element of the vector
			//stepsToCurrent++; // Starting step is regarded as step 1.
			avgPriceChange = avgPriceChange / stepsToCurrent;
			std::cout << "======================================================" << std::endl;
			std::cout << "The average change in BID price for " << product << " is " << avgPriceChange << std::endl;
			predictedPrice = lastKnownPrice + avgPriceChange;
			std::cout << "By knowing the average price change in all passed steps" << std::endl;
			std::cout << "And by knowing that the last known price is  " << lastKnownPrice << std::endl;
			std::cout << "Advisorbot predicts that the next BID price of " << product << " is going to be " << predictedPrice << std::endl;
			std::cout << "======================================================" << std::endl;
		}
		else {
			std::cout << "Bad input for bid/ask type. Please select one of both." << std::endl;
		}
	}
	else {
		std::cout << "Bad input. Try again." << std::endl;
	}
}

/// <summary>
/// Used by the predict() function for the 'predict' command. Takes all the prices of a product in a timestep and estimates the mean price change.
/// </summary>
/// <param name="orders"></param>
/// <returns></returns>
float Advisorbot::stepPriceChange(const std::vector<OrderBookEntry>& orders)
{
	float avgPriceChange = 0;
	std::vector<float> priceChanges;
	float diff;

	for ( int i = 0 ; i + 1 < orders.size(); i++) // i+1 so that we can check the difference between the second to last and the last vector elements
	{
		diff = orders[i].price - orders[i + 1].price;
		priceChanges.push_back(diff);
	}

	for (float& change : priceChanges)
	{
		avgPriceChange = avgPriceChange + change;
	}

	avgPriceChange = avgPriceChange / ( orders.size() - 1 ); // -1 because we get 1 less comparison from prices. E.g. from 3 prices we get 2 price changes.
	std::cout << "Has analysed " << orders.size() << " product prices for the timestamp " << orders[0].timestamp << std::endl;
	return avgPriceChange;
}


/// <summary>
/// Make input validation tests depending on how many words the user has input as command.
/// </summary>
/// <param name="tokens">: Takes as input a vector of tokenised user input command. Example: "min" , "ETH/USDT" , "ask".</param>
/// <returns>: True or False</returns>
bool Advisorbot::inputValidator(const std::vector<std::string>& tokens)
{
	// check if space has only been used once as a separator char and not a token
	bool spaceTokenCheck = true;
	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == " ")
		{
			std::cout << "Advisorbot's AI isn't perfect and it has recorded a blank space as a parameter of a command." << std::endl;
			std::cout << "Maybe you've used blank space more than one consecutive times." << std::endl;
			std::cout << "Please re - enter command using space once between commandand parameters." << std::endl;
			spaceTokenCheck = false;
		}
	}
	

	// based on which command is used, different input validations take place
	if (tokens[0] == "help")
	{
		// check the case for different commands that have different token numbers. validate the tokens
		if (tokens.size() > 2)
		{
			std::vector<std::string> commands = getKnownCommands();
			std::string command = commands[1];
			bool commandCheck = false;

			// check if input product is valid
			for (std::string const& c : commands)
			{
				if (c == command)
				{
					commandCheck = true;
				}
			}

			//check if all tests have passed or not
			if (commandCheck && spaceTokenCheck)
			{
				return true;
			}
			else {
				std::cout << "Bad input for product. Use the 'prod' command to see list of products." << std::endl;
				return false;
			}
		}
	}
	else if ((tokens[0] == "min" || tokens[0] == "max" || tokens[0] == "predict") && tokens.size() == 3)
	{
		std::string product = tokens[1];
		std::string type = tokens[2];
		bool productCheck = false;
		bool typeCheck = false;

		// check if input product is valid
		for (std::string const& p : orderBook.getKnownProducts())
		{
			if (p == product)
			{
				productCheck = true;
			}
		}
		if (!productCheck)
		{
			std::cout << "Bad input for product. Use the 'prod' command to see list of products." << std::endl;
		}

		// check if input for ask or bid trade type
		if (type == "ask" || type == "bid")
		{
			typeCheck = true;
		}
		else {
			std::cout << "Bad input for trade action type. Must be either 'ask' or 'bid'." << std::endl;
		}

		//check if all tests have passed or not
		if (typeCheck && productCheck && spaceTokenCheck)
		{
			return true;
		}
		else {
			return false;
		}
	}
	else if (tokens[0] == "compare" && tokens.size() == 4)
	{
		std::string product_1 = tokens[1];
		std::string product_2 = tokens[2];
		std::string type = tokens[3];
		bool product_1Check = false;
		bool product_2Check = false;
		bool typeCheck = false;

		// check if input products are valid
		for (std::string const& p1 : orderBook.getKnownProducts())
		{
			if (p1 == product_1)
			{
				product_1Check = true;
			}
		}
		for (std::string const& p2 : orderBook.getKnownProducts())
		{
			if (p2 == product_2)
			{
				product_2Check = true;
			}
		}
		if (!product_1Check)
		{
			std::cout << "Bad input for product #1. Use the 'prod' command to see list of products." << std::endl;
		}
		if (!product_2Check)
		{
			std::cout << "Bad input for product #2. Use the 'prod' command to see list of products." << std::endl;
		}

		// check if input for ask or bid trade type
		if (type == "ask" || type == "bid")
		{
			typeCheck = true;
		}
		else {
			std::cout << "Bad input for trade action type. Must be either 'ask' or 'bid'." << std::endl;
		}

		//check if all tests have passed or not
		if (typeCheck && product_1Check && product_2Check && spaceTokenCheck)
		{
			return true;
		}
		else {
			return false;
		}
	}
	else if (tokens[0] == "avg" && tokens.size() > 2)
	{
		if (tokens.size() == 4)
		{
			std::string product = tokens[1];
			std::string type = tokens[2];
			std::string steps = tokens[3];

			bool productCheck = false;
			bool typeCheck = false;
			bool stepsCheck = true;

			// check if input product is valid
			for (std::string const& p : orderBook.getKnownProducts())
			{
				if (p == product)
				{
					productCheck = true;
				}
			}
			if (!productCheck)
			{
				std::cout << "Bad input for product. Use the 'prod' command to see list of products." << std::endl;
			}

			// check if input for ask or bid trade type
			if (type == "ask" || type == "bid")
			{
				typeCheck = true;
			}
			else {
				std::cout << "Bad input for trade action type. Must be either 'ask' or 'bid'." << std::endl;
			}

			// check if steps are an integer
			for (char const& c : steps) {
				if (std::isdigit(c) == 0)
				{
					std::cout << "Bad input for steps. Steps must be a number." << std::endl;
					stepsCheck = false;
				}
			}

			//check if all tests have passed or not
			if (stepsCheck && typeCheck && productCheck && spaceTokenCheck)
			{
				return true;
			}
			else {
				return false;
			}
		}

		// or if no steps are specified the default will be set to 1 as the current time step inside printAvg()
		else if (tokens.size() == 3)
		{
			std::string product = tokens[1];
			std::string type = tokens[2];
			bool productCheck = false;
			bool typeCheck = false;

			// check if input product is valid
			for (std::string const& p : orderBook.getKnownProducts())
			{
				if (p == product)
				{
					productCheck = true;
				}
			}
			if (!productCheck)
			{
				std::cout << "Bad input for product. Use the 'prod' command to see list of products." << std::endl;
			}

			// check if input for ask or bid trade type
			if (type == "ask" || type == "bid")
			{
				typeCheck = true;
			}
			else {
				std::cout << "Bad input for trade action type. Must be either 'ask' or 'bid'." << std::endl;
			}

			//check if all tests have passed or not
			if (typeCheck && productCheck && spaceTokenCheck)
			{
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		std::cout << "Advisorbot could not understand your command and its input parameters." << std::endl;
		std::cout << "Please check again if you are using the correct command with the relevant parameters." << std::endl;
		std::cout << "Use the 'help' command to check the menu again or use 'help' plus the command name to get info on specific command usage." << std::endl;
	}
}

std::vector<std::string> Advisorbot::getKnownCommands()
{
	std::vector<std::string> commands{
		"help",
		"prod",
		"min",
		"max",
		"avg",
		"compare",
		"predict",
		"time",
		"step",
		"reset"
	};
	return commands;
}