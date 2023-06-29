#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "zavorky.hpp"

int main(int argc, char** argv)
{

	std::vector<std::string> files(argv, argv + argc);

	if (files.size() != 1)
	{
		for (size_t i = 1; i < files.size(); i++)
		{
			std::string name = files[i];
			std::ifstream file(name);
			std::string line;
			while (std::getline(file, line))
			{
				process_line(line);
			}

		}
	}
	else
	{
		std::string line;
		while (std::getline(std::cin, line))
		{
			process_line(line);
		}
	}
	return 0;

}