#include <iostream>
#include <istream>
#include<fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>



struct func
{
	std::vector<std::string> input;
	std::vector<std::string> output_pattern;
	std::map<std::string, std::string> output;
};
class Evaluator
{
public:
	Evaluator(std::istream& f) : f_(f) {}
	std::vector<std::string> get_tokens(std::string& line)
	{
		std::vector<std::string> tokens;
		std::stringstream ss(line);
		while (true)
		{
			std::string token;
			ss >> token;
			tokens.push_back(token);
			if (!ss.good()) return tokens;
		}
	}

	std::vector<std::string> get_tokens2(std::string& l)
	{
		std::vector<std::string> v;
		std::string token;
		std::stringstream ss(l);
		while (true)
		{
			ss >> token;
			v.push_back(token);
			if (!ss.good()) return v;
		}
	}
	//fff a d = d a

	void interpret_func_file()
	{
		std::string line;

		while (true)
		{
			std::getline(f_, line);
			try
			{
				read_function(line);
			}
			catch (std::runtime_error& e)
			{
				std::cout << e.what() << std::endl;
			}
			if (!f_.good()) return;
		}
	}
	void read_function(std::string& l)
	{
		std::vector<std::string> line = get_tokens(l);
		try
		{
			func new_func;
			std::string name = line[0];
			std::size_t i = 1;
			while (line[i] != "=")
			{
				new_func.input.push_back(line[i]);
				new_func.output.emplace(line[i], "");
				i++;
			}
			i++;
			while (i != line.size())
			{
				if (new_func.output.find(line[i]) != new_func.output.end())
				{
					new_func.output_pattern.push_back(line[i]);

				}
				else
				{
					std::cout << "def error" << std::endl;
					return;
				}
				i++;
			}
			functions.emplace(name, new_func);
		}
		catch (...)
		{
			std::cout << "parse error" << std::endl;
			return;
		}
	}

	// nekam to dat
	bool all_letters(std::string& line)
	{
		for (auto it = line.begin(); it != line.end(); it++)
		{
			if (!isalpha(*it)) return false;
		}
		return true;
	}

	void evaluate_paramaters(std::string& name, std::string& p, std::size_t j)
	{
		//std::cout << p << std::endl;
		//std::size_t j = 1;
		func fnc = functions.find(name)->second;

		for (std::size_t i = 0; i < fnc.input.size(); i++)
		{
			if (j == p.size())
			{
				std::cout << "param error" << std::endl;
				return;
			}
			if (j != p.size() && p[j] == '\"')
			{
				j++;
				std::string par = "";
				while (j != p.size() - 1 && p[j] != '\"')
				{
					par += p[j]; j++;
				}
				fnc.output[fnc.input[i]] = par;
				j += 2;
			}
			else if (p[j] != '\"')
			{
				std::string fnc_name = "";
				while (j != p.size() && p[j] != '(')
				{
					fnc_name += p[j];
					j++;
				}
				if (functions.find(fnc_name) == functions.end())
				{
					std::cout << "nodef error" << std::endl;
					return;
				}
				else
				{
					while (j != p.size() && p[j] != ')')
					{
						fnc_name += p[j];
						j++;
					}
					fnc_name += p[j];
					fnc.output[fnc.input[i]] = fnc_name;
					j += 2;
				}
			}
		}

		for (std::size_t k = 0; k < fnc.output_pattern.size(); k++)
		{
			std::string out_p = fnc.output[fnc.output_pattern[k]];
			if (out_p.find('\"') != std::string::npos)
			{
				evaluate_line(out_p);
			}
			else
				std::cout << fnc.output[fnc.output_pattern[k]]<< " ";
		}
	}

	void evaluate_line(std::string& l)
	{
		std::string name = "";
		for (std::size_t i = 0; i < l.size(); i++)
		{
			if (l[i] != '(')
			{
				name += l[i];
			}
			if (l[i] == '(')
			{
				if (functions.find(name) == functions.end())
				{
					std::cout << "nodef error" << std::endl;
					return;
				}
				if (l[l.size() - 1] == ')')
				{
					std::string paramaters = l.substr(i);
					evaluate_paramaters(name,paramaters, 1);
				}
				break;
			}

		}			

	}

	void evaluate_ex_file(std::istream& input_)
	{
		std::string line;
		while (true)
		{			
			std::getline(input_, line);
			try
			{
				evaluate_line(line);
				std::cout << std::endl;
			}
			catch (std::runtime_error& e)
			{
				std::cout << e.what() << std::endl;
			}
			if (!input_.good()) return;
		}
	}
private:
	std::istream& f_;
	std::map < std::string, func> functions;
};


int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argc + argv);
	if (args.size() > 1)
	{
		std::ifstream file(args[1]);
		std::ifstream file2(args[2]);
		//Evaluator ev(file);
		std::string line;
		std::vector<std::string> fncs;
		while (true)
		{
			getline(std::cin, line);
			if (line == "") break;
			fncs.push_back(line);
		}
		std::cout << fncs.size() << std::endl;
		Evaluator ev(file);
		ev.interpret_func_file();
		ev.evaluate_ex_file(file2);
 	}
	else
	{
		Evaluator ev(std::cin);
		ev.interpret_func_file();
	}
	return 0;
}