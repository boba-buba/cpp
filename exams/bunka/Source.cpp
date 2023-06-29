#include <iostream>
#include<vector>
#include <string>
#include <fstream>
#include <memory>
#include <map>
#include <sstream>

namespace InterpreteR {

	class Cell;
	
	std::map<std::string, std::shared_ptr<Cell>> mem;
	class Cell
	{
	public:
		virtual ~Cell() {};
		virtual void printNumber() = 0;
		virtual void setLeft(std::string&) = 0;
		virtual void setRight(std::string&) = 0;
		virtual std::shared_ptr<Cell> getRight() = 0;
		virtual std::shared_ptr<Cell> getLeft() = 0;
	};

	class Constant : public Cell
	{
	public:
		Constant(int v) : value_(v) {};
		virtual void printNumber() override
		{
			std::cout << value_;
		}
		virtual void setLeft(std::string&) override {};
		virtual void setRight(std::string&) override {};
		virtual std::shared_ptr<Cell> getLeft() override { return nullptr; };
		virtual std::shared_ptr<Cell> getRight() override { return nullptr; };

	private:
		int value_;
	};

	class Node : public Cell
	{
	public:

		virtual void setLeft(std::string& l) override
		{
			//auto p = std::make_shared<Cell>(l);
			if (mem.find(l) != mem.end())
			{
				auto p = mem.find(l);
				
				left_ = p->second;
			}
			else
			{
				left_ = std::move(std::make_shared<Constant>(std::stoi(l)));
			}
			
		}
		virtual void setRight(std::string& r) override
		{
			if (mem.find(r) != mem.end())
			{
				auto p = mem.find(r);

				right_ = p->second;
			}
			else
			{
				right_ = std::move(std::make_shared<Constant>(std::stoi(r)));
			}
		}
		virtual std::shared_ptr<Cell> getLeft() override
		{
			return left_;
		}
		virtual std::shared_ptr<Cell> getRight() override
		{
			return right_;
		}
		virtual void printNumber() override 
		{
			std::cout << "(";
			left_->printNumber();
			std::cout << ", "; 
			right_->printNumber();
			std::cout << ")";
		}
	private:
		std::shared_ptr<Cell> left_ = nullptr;
		std::shared_ptr<Cell> right_ = nullptr;
	};


	class Interpreter {
	public:
		Interpreter(std::istream& input) : input_(input) { mem.clear(); };

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
		bool is_const(std::string& l)
		{
			try {
				auto val = std::stoi(l);
				return 1;
			}
			catch (...)
			{
				return 0;
			}
		}

		void interpret_line(std::string& line)
		{
			std::vector<std::string> tokens = get_tokens(line);
			if (tokens.size() <= 1) return;

			if (tokens.size() == 3 && tokens[1] == "=")
			{
				if (tokens[0][0] != '$')
				{
					std::cout << "Syntax error on line" << std::endl;
					return;
				}
				//std::string name = tokens[0].substr(1);
				std::string name = tokens[0];

				int value = 0;
				try {
					value = std::stoi(tokens[2]);
				}
				catch (...)
				{
					auto val = mem.find(tokens[2]);
					if (val == mem.end())
					{
						std::cout << "Syntax error on line" << std::endl;
						return;
					}
					
					//auto val2 = std::make_shared<Node>(val->second);
					
					mem.emplace(name, val->second);
	
				}

				auto val = std::make_shared<Constant>(value);
				mem.emplace(name, val);
			}
			else if (tokens.size() == 2)
			{
				if (tokens[0] != "print")
				{
					std::cout << "Syntax error on line" << std::endl;
					return;
				}
				try {
					auto val = std::stoi(tokens[1]);
					std::cout << val << std::endl;
				}
				catch (...)
				{
					auto val = mem.find(tokens[1]);
					if (val != mem.end())
					{
						val->second->printNumber();
						std::cout << std::endl;
					}
					else
					{
						std::cout << "Synatx error on line" << std::endl;
						return;
					}
				}
			}
			else if (tokens.size() == 5 && tokens[1] == "=" && tokens[2] == "cell")
			{
				Node node;
				if (!is_const(tokens[3]) && (mem.find(tokens[3]) == mem.end()))
				{
					std::cout << "Unknown variable" << std::endl;
					return;
				}
				node.setLeft(tokens[3]);

				//-------------
				if (!is_const(tokens[4]) && (mem.find(tokens[4]) == mem.end()))
				{
					std::cout << "Unknown variable" << std::endl;
					return;
				}
				node.setRight(tokens[4]);
				auto p = std::make_shared<Node>(node);

				mem.insert_or_assign(tokens[0], p);
			}
			else if (tokens.size() == 4)
			{
				std::string name = tokens[0];
				if (mem.find(tokens[3]) == mem.end())
				{
					std::cout << "Unknown argument" << std::endl;
					return;
				}
				auto val = mem.find(tokens[3]);

				auto p = (val->second)->getRight();
				if (p == NULL)
				{
					std::cout << "Invalid operation on line" << std::endl;
					return;
				}


				if (tokens[2] == "right")
				{
					mem.insert_or_assign(name, (val->second)->getRight());
				}
				else {
					mem.insert_or_assign(name, (val->second)->getLeft());
				}

				
			}
			else { std::cout << "Syntax error on line" << std::endl; return; }
		}
		void interpret_file()
		{
			std::string line;
			while (true)
			{
				std::getline(input_, line);
				if (!input_) return;
				
				try {
					interpret_line(line);
				}
				catch (std::runtime_error e)
				{
					std::cout << e.what() << " ";
				}	
			}
		}
	private:
		std::istream& input_;
		
	};
}
/*
int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);
	if (args.size() != 0)
	{
		for (size_t i = 1; i < args.size(); i++)
		{	
			
			std::cout << args[0] << std::endl;
			std::ifstream file(args[i]);
			InterpreteR::Interpreter in(file);
			in.interpret_file();
			std::cout << std::endl;
		}
	}
	else
	{
		InterpreteR::Interpreter in(std::cin);
		in.interpret_file();
	}
	return 0;
}*/