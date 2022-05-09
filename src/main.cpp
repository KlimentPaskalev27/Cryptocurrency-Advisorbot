#include <iostream>
#include "Advisorbot.h"
#include<chrono> //library to handle time

int main()
{
	//framework that relates time points to physical time
	const auto start = std::chrono::high_resolution_clock::now();
	Advisorbot app{};
	app.init();

	const auto end = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration<double>(end - start).count() << "s\n";
}