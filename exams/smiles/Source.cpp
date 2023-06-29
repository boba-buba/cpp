#include <iostream>
#include <string>
#include <vector>
#include "smiles.hpp"

int main(int argc, char** argv)
{
	std::vector<char> vstupnivzorec;
	char c;
	while (std::cin.get(c))
	{
		if (c == '*')
		{
			break;
		}
		vstupnivzorec.push_back(c);
	}
	if (isalpha(vstupnivzorec[vstupnivzorec.size() - 1]))
	{
		vstupnivzorec.push_back(' ');
	}
	SMILES Smiles;
	Smiles.prevedvzorec(vstupnivzorec);
	Smiles.vypisprvky();
	Smiles.vypisvazby();
	std::cin >> c;
	return 0;
}