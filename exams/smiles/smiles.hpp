#pragma once
#include <vector>
#include <string>
#include <map>
#include <iostream>
class SMILES
{
public:
	void prevedvzorec(const std::vector<char>&);
	void vypisprvky();
	void vypisvazby();
private:
	void vytvorvazbu();
	std::vector<std::string> prvky;
	std::map<int, std::vector<int>> jednoduchevazby;
	std::map<int, std::vector<int>> dvojitevazby;
	std::map<int, std::vector<int>> trojitevazby;
	int value = 0;
	char poslednivazba = '-';
	int aktualnipredchozi = 0;
	std::vector<int> odbocky;
	std::map<int, int> smycky;
	std::string aktualniprvek;
	bool cteniprvku = false;
	bool ctenibackrefernce = false;
	char predchoziznak = ' ';
	bool kontrola(const std::vector<char>&);
};