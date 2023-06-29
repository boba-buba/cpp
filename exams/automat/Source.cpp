#include <iostream>
#include <string>
#include <fstream>
#include <istream>
#include <map>
#include <vector>
#include <memory>
#include <sstream>


class Automate
{
public:
	Automate(std::ifstream& f) : input(f) {};
	
	std::string name_;
	std::vector<std::string> alphabet;
	std::map<std::string, std::multimap<std::string, std::string>> automat;
	std::map<std::string, std::string> what_states;
	void set_alphabet(std::string& al)
	{
		std::string letter = "";
		for (size_t i = 0; i < al.size(); i++)
		{
			letter += al[i];
			alphabet.push_back(letter);
			letter = "";
		}
	}	
	void add_state_to_automat(std::string& state)
	{
		std::vector<std::string> v;
		std::stringstream ss(state);
		std::string token;
		while (true)
		{
			ss >> token;
			if (!ss) break;
			v.push_back(token);
		}

		std::string what_state = "";
		std::string name = "";
		size_t beg = 0;
		if (v[0] == "<->" || v[0] == "<-" || v[0] == "->")
		{
			what_state = v[0];
			name = v[1];
			beg = 4;
		}
		else {
			beg = 3;
			name = v[0];
		}
		
		std::vector<std::string> states_to;
		size_t letter = 0;
		std::multimap<std::string, std::string> line;
		for (size_t i = beg; i < v.size(); i++)
		{
			if (v[i] == "|")
			{
				for (size_t i = 0; i < states_to.size(); i++)
				{
					line.insert(alphabet[letter], states_to[i]);
				}
				letter++;
			}
		}

		automat.emplace(name, line);
		what_states.emplace(name, what_state);
	}
	void set_data()
	{
		std::string line;
		std::getline(input, line);
		name_ = line;
		std::getline(input, line);
		set_alphabet(line);

		while (true)
		{
			std::getline(input, line);
			if (!input) return;
			add_state_to_automat(line);
		}

		
	}	
private:
	std::ifstream& input;
};

/*
class Automates
{
public:
	void create_automate(std::string& file_name, std::string& type)
	{
		
		std::ifstream file(file_name);
		Automate a(file);
		a.set_data();
		if (type == "-d") d_automates.push_back(std::move(std::make_shared<Automate>(a)));
		else n_automates.push_back(std::move(std::make_shared<Automate>(a)));

	}
	void process_input(std::string& file_name)
	{

	}
private:
	std::vector<std::shared_ptr<Automate>> n_automates;
	std::vector<std::shared_ptr<Automate>> d_automates;

};


class Parser
{
public:
	Parser(std::vector<std::string>& files) : files_(files) {};
	void parse_files()
	{
		//Automates automates;
		std::string type = "-d";
		size_t i = 0;
		while (i < files_.size())
		{
			if ((type == "-d" || type == "-n") && files_[i] != "-d" && files_[i] != "-n" && files_[i] != "-i")
			{
				//automates.create_automate(files_[i], type);
				std::cout << files_[i] << std::endl;
				i++;
			}
			else if((type == "-i") && files_[i] != "-i" && files_[i] != "-d" && files_[i] != "-n")
			{
				std::cout << files_[i] << std::endl;

				//automates.process_input(files_[i]);
				i++;
			}
			else
			{
				type = files_[i];
				i++;
			}

		}
	}

private:
	std::vector<std::string> files_;
};

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);

	if (args.size() != 0)
	{
		Parser p(args);
		p.parse_files();
		
	}

	std::cout << "hello" << std::endl;
	return 0;
}*/