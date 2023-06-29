#include "smiles.hpp"

bool SMILES::kontrola(const std::vector<char>& vstup)
{
	char predchoziznak = ' ';
	std::string aktualniprvek;
	bool nazevprvku = false;
	if (isalpha(vstup[0]) == false) return false;

	if (vstup[vstup.size() - 1] == '#' || vstup[vstup.size() - 1] == '-' || vstup[vstup.size() - 1] == '=')
	{
		return false;
	}
	for (auto&& it : vstup)
	{
		if ((it == '-' || it == '=' || it == '#') && (predchoziznak == '-' || predchoziznak == '#' || predchoziznak == '='))
		{
			return false;
		}
		if ((it == ')') && (predchoziznak == ')' || predchoziznak == '#' || predchoziznak == '-' || predchoziznak == '='))
			return false;
		predchoziznak = it;
	}
	return true;
}

void SMILES::prevedvzorec(const std::vector<char>& vstup)
{
	if (kontrola(vstup))
	{
		for (auto&& it : vstup)
		{
			if (cteniprvku)
			{
				if (islower(it))
				{
					aktualniprvek = aktualniprvek + it;
				}
				else
				{
					prvky.push_back(aktualniprvek);
					aktualniprvek.clear();
					cteniprvku = false;
					if (aktualnipredchozi != 0)
					{
						vytvorvazbu();
					}
					aktualnipredchozi = prvky.size();
				}
			}
			if (it == '(')
			{
				odbocky.push_back(aktualnipredchozi);
			}
			if (it == ')')
			{
				aktualnipredchozi = odbocky[odbocky.size() - 1];
				odbocky.erase(odbocky.end() - 1);
			}
			if (ctenibackrefernce && !isdigit(it))
			{
				ctenibackrefernce = false;
				auto klic = smycky.find(value);
				if (klic != smycky.end())
				{
					int temp = aktualnipredchozi;
					aktualnipredchozi = klic->second;
					vytvorvazbu();
					aktualnipredchozi = temp;
				}
				else {
					smycky.insert(std::make_pair(value, aktualnipredchozi));
				}
				value = 0;
			}
			if (isdigit(it))
			{
				value = value * 10 + it - '0';
				ctenibackrefernce = true;
			}
			if (isupper(it))
			{
				aktualniprvek = it;
				cteniprvku = true;
			}
			if (it == '=')
			{
				poslednivazba = it;
			}
			if (it == '#')
			{
				poslednivazba = it;
			}
		}
	}
	else
	{
		std::cout << "invalid input";
	}
}

void SMILES::vytvorvazbu()
{
	if (poslednivazba == '-')
	{
		auto it2 = jednoduchevazby.find(aktualnipredchozi);
		if (it2 == jednoduchevazby.end())
		{
			std::vector<int> temp;
			temp.push_back(prvky.size());
			jednoduchevazby.insert(std::make_pair(aktualnipredchozi, temp));
		}
		else
		{
			it2->second.push_back(prvky.size());
		}
	}
	if (poslednivazba == '=')
	{
		auto it2 = dvojitevazby.find(aktualnipredchozi);
		if(it2 == dvojitevazby.end())
		{
			std::vector<int> temp;
			temp.push_back(prvky.size());
			dvojitevazby.insert(std::make_pair(aktualnipredchozi, temp));

		}
		else
		{
			it2->second.push_back(prvky.size());
		}
		poslednivazba = '-';
	}
	if (poslednivazba == '#')
	{
		auto it2 = trojitevazby.find(aktualnipredchozi);
		if (it2 == trojitevazby.end())
		{
			std::vector<int> temp;
			temp.push_back(prvky.size());
			trojitevazby.insert(std::make_pair(aktualnipredchozi, temp));
		}
		else
		{
			it2->second.push_back(prvky.size());
		}
		poslednivazba = '-';
	}
}

void SMILES::vypisprvky()
{
	int count = 0;
	std::cout << "Prvky:" << std::endl;
	for (auto&& it : prvky)
	{
		std::cout << count << it << std::endl;
		count++;
	}
}


void SMILES::vypisvazby()
{
	int count = 1;
	for (auto&& it : prvky)
	{
		auto it2 = jednoduchevazby.find(count);
		if (it2 != jednoduchevazby.end())
		{
			for (size_t i = 0; i < it2->second.size(); i++)
			{
				std::cout << it2->first << " - " << it2->second[i] << std::endl;
			}
		}
	
		auto it3 = dvojitevazby.find(count);
		if (it3 != dvojitevazby.end())
		{
			for (size_t i = 0; i < it3->second.size(); i++)
			{
				std::cout << it3->first << " = " << it3->second[i] << std::endl;
			}
		}
	
		auto it4 = trojitevazby.find(count);
		if (it4 != trojitevazby.end())
		{
			for (size_t i = 0; i < it3->second.size(); i++)
			{
				std::cout << it4->first << " # " << it4->second[i] << std::endl;
			}
		}
		count++;
	}
}