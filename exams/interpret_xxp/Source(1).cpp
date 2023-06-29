#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include <sstream>
#include <map>


class Parser
{
public:
	size_t line_number = 0;
	size_t line_number_ex = 0;
	Parser(std::ifstream& file) : input_(file) { mem.clear(); program.clear(); };
	std::map<std::string, int> mem{};
	std::vector<std::string> get_tokens(std::string& line)
	{
		std::vector<std::string> tokens{};
		std::string token = "";
		bool znak = false;
		
		for (size_t i = 0; i < line.size(); i++)
		{
			if (isspace(line[i]) || i == line.size() - 1)
			{
				if (i == line.size() - 1 && line[i] != ';')
				{
					token += line[i];
				}
				if (token != "") tokens.push_back(token);
				token = "";
			}
			else if (line[i] == '=')
			{
				if (token != "")  tokens.push_back(token);
				token = "";
				tokens.push_back("=");
			}
			else if (line[i] == '+' || line[i] == '-' || line[i] == '/' || line[i] == '*' || line[i] == '?')
			{
				if (token != "")  tokens.push_back(token);
				token = "";
				std::string t = ""; t += line[i];
				tokens.push_back(t);
				
			}
			else 
			{ 
				if (line[i] == ';') znak = true;
				if (!isspace(line[i])) token += line[i];
			}
		}

		if (tokens[0][0] != ';' && znak) {
			while (*(tokens.end() - 1) != ";")
			{
				tokens.pop_back();
			}
			tokens.pop_back();
		}
		return tokens;

	}
	int operation(int a, int b, std::string& op)
	{
		if (op == "-") 
			return a - b;
		if (op == "+") 
			return a + b;
		if (op == "*")
			return a * b;
		else return a / b;  // deleni 0
	}

	void parse_line(std::string& line) 
	{
		std::vector<std::string> tokens = get_tokens(line);
		line_number_ex = line_number;  line_number++;
		if (tokens[0][0] == ';') return;

		if (tokens[1] == "=")
		{
			std::string name = tokens[0];
			if (tokens.size() == 5)
			{
				int temp = 0;
				auto var1 = mem.find(tokens[2]);
				auto var2 = mem.find(tokens[4]);
				if (var1 != mem.end() && var2 != mem.end())
				{
					temp = operation(var1->second, var2->second, tokens[3]);
				}
				else
				{
					if (var1 == mem.end() && var2 == mem.end())
					{
						try {
							int temp1 = std::stoi(tokens[2]);
							int temp2 = std::stoi(tokens[4]);
							temp = operation(temp1, temp2, tokens[3]);
						}
						catch (...)
						{
							std::cout << "error " << line_number_ex << std::endl;
							return;
						}
					}
					else if (var1 == mem.end())
					{
						try {
							int temp1 = std::stoi(tokens[2]);
							temp = operation(temp1, var2->second, tokens[3]);

						}
						catch (...)
						{
							std::cout << "error " << line_number_ex << std::endl;
							return;
						}
					}
					else if (var2 == mem.end())
					{
						try {
							int temp2 = std::stoi(tokens[4]);
							temp = operation(var1->second, temp2, tokens[3]);
						}
						catch (...)
						{
							std::cout << "error " << line_number_ex << std::endl;
							return;
						}
					}
				}
				mem.insert_or_assign(name, temp);
			}
			else if (tokens.size() == 3)
			{

				auto val1 = mem.find(tokens[2]);
				if (val1 != mem.end())
				{
					mem.insert_or_assign(name, val1->second);
				}
				else
				{
					try
					{
						int temp = std::stoi(tokens[2]);
						mem.insert_or_assign(name, temp);
					}
					catch (...)
					{
						std::cout << "error " << line_number_ex << std::endl;
						return;
					}
				}
			}
		}
		else if (tokens.size() == 3 && tokens[1] == "?")
		{
			auto val1 = mem.find(tokens[0]);
			if (val1 != mem.end())
			{
				if (val1->second != 0)
				{
					auto val2 = mem.find(tokens[2]);
					if (val2 != mem.end())
					{
						line_number = val2->second;
					}
					else
					{
						try {
							int temp = std::stoi(tokens[2]);
							line_number = temp;
						}
						catch (...)
						{
							std::cout << "error " << line_number_ex << std::endl;
							return;
						}
					}
				}
			}
			else
			{
				std::cout << "error " << line_number_ex << std::endl;
				return;
			}
		}
		else
		{
			std::cout << "error " << line_number_ex << std::endl;
			return;
		}

	}
	void print_all_vars()
	{
		if (mem.size() > 0)
		{
			for (auto it = mem.begin(); it != mem.end(); it++)
			{
				std::cout << it->first << "=" << it->second << std::endl;
			}
		}
		
	}

	void parse_file()
	{
		std::string line = "";
		while (true)
		{
			std::getline(input_, line);
			if (!input_) break;
			program.push_back(line);
		}
		while (line_number != program.size())
		{
			parse_line(program[line_number]);
		}		
	}
private:
	std::ifstream& input_;
	std::vector<std::string> program{};
};


int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);

	if (args.size()>0)
	{
		std::ifstream file(args[0]);
		Parser p(file);
		p.parse_file();
		p.print_all_vars();
	}
	else
	{
		std::string name = "";
		std::cin >> name;
		std::ifstream file(name);
		Parser p(file);
		p.parse_file();
		p.print_all_vars();

	}
	return 0;
}