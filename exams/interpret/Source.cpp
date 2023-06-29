#include <map>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>

class dynamic_null;
class dynamic_int;
class dynamic_string;
class dynamic_type {
public:
	using dyn_ptr = std::shared_ptr<dynamic_type>;
	virtual std::string to_string() const = 0;
	virtual dyn_ptr add_left_operand(const dynamic_null&) const = 0;
	virtual dyn_ptr add_left_operand(const dynamic_int&) const = 0;
	virtual dyn_ptr add_left_operand(const dynamic_string&) const = 0;
	virtual dyn_ptr add_right_operand(const  dynamic_type&) const = 0;
	virtual ~dynamic_type() = default;
};

using dyn_ptr = dynamic_type::dyn_ptr;

class dynamic_null : public dynamic_type {
public:
	std::string to_string() const override
	{
		return "null";
	}
	virtual dyn_ptr add_left_operand(const dynamic_int&) const override;
	virtual dyn_ptr add_left_operand(const dynamic_string&) const override;
	virtual dyn_ptr add_left_operand(const dynamic_null&) const override;
	virtual dyn_ptr add_right_operand(const dynamic_type&) const override;
};

class dynamic_int : public dynamic_type
{
	std::int32_t value_;
public:
	std::int32_t get_int() const
	{
		return value_;
	}
	std::string to_string() const override
	{
		return std::to_string(value_);
	}
	dynamic_int(int32_t value) : value_(value) {};

	virtual dyn_ptr add_left_operand(const dynamic_null&) const override;
	virtual dyn_ptr add_left_operand(const dynamic_int& lhs) const override;
	virtual dyn_ptr add_left_operand(const dynamic_string& lhs) const override;
	virtual dyn_ptr add_right_operand(const dynamic_type& rhs) const override;
};

class dynamic_string : public dynamic_type {
	std::string value_;
public:
	dynamic_string(const std::string& value) : value_(value) {};
	std::string get_string() const
	{
		return value_;
	}
	std::string to_string() const override
	{
		std::string result = "\"" + value_ + "\"";
		return result;
	}
	virtual dyn_ptr add_left_operand(const dynamic_null&) const override;
	virtual dyn_ptr add_left_operand(const dynamic_int& lhs) const override;
	virtual dyn_ptr add_left_operand(const dynamic_string& lhs) const override;
	virtual dyn_ptr add_right_operand(const dynamic_type& rhs) const override;
};

class variables_context {
	std::map<std::string, std::shared_ptr<dynamic_type>> vars_;
	std::shared_ptr<dynamic_null> null_;
public:
	variables_context() : null_(std::make_shared<dynamic_null>()) {}
	std::shared_ptr<dynamic_type> get_var(std::string &var_name) ///refernece ++
	{
		if (vars_.count(var_name)) 
		{
			return vars_[var_name];
		}
		else
		{
			return null_;
		}

	}
	void add_or_replace_var(const std::string& name, dyn_ptr value)
	{
		vars_.insert_or_assign(name, value);
	}
};


void run_script(std::istream& input)
{
	std::string line;
	variables_context vars;
	while (input.good())
	{
		std::getline(input, line);
		std::stringstream line_stream(line);
		std::vector<std::string> tokens;
		while (line_stream.good())
		{
			std::string token;
			line_stream >> token;
			if (token.size())
			{
				tokens.push_back(token);
			}
		}

		if (!tokens.size()) break;
		if (tokens[0] == "seti")
		{
			if (tokens.size() != 3)
			{
				std::cout << "Invalid namber of arguments" << std::endl;
				continue;
			}
			std::string var_name = tokens[1];
			std::int32_t value = 0;
			try
			{
				value = std::stoi(tokens[2]);
			}
			catch (...)
			{
				std::cout << "Invalid format of the argument, set to 0" << std::endl;
			}
			vars.add_or_replace_var(var_name, std::make_shared<dynamic_int>(value));
		}
		else if (tokens[0] == "sets")
		{
			if (tokens.size() != 3)
			{
				std::cout << "Invalid number of arguments" << std::endl;
				continue;
			}
			std::string var_name = tokens[1];
			std::string value = tokens[2];
			vars.add_or_replace_var(var_name, std::make_shared<dynamic_string>(value));
		}
		else if (tokens[0] == "add")
		{
			if (tokens.size() != 4)
			{
				std::cout << "Invalid number of arguments" << std::endl;
				continue;
			}
			std::string var_name = tokens[1];
			auto lhs = vars.get_var(tokens[2]);
			auto rhs = vars.get_var(tokens[3]);

			auto result = lhs->add_right_operand(*rhs);
			vars.add_or_replace_var(var_name, result);
		}
		else if (tokens[0] == "echo")
		{
			if (tokens.size() != 2)
			{
				std::cout << "Invalid number of arguments" << std::endl;
				continue;
			}
			auto value = vars.get_var(tokens[1]);
			std::cout << value->to_string() << std::endl;
		}
		else
		{
			std::cout << "Uknown command" << std::endl;
		}
		
	}
}

int main(int argc, char** argv)
{
	if (argc == 2)
	{
		std::string file_name = argv[1];
		std::ifstream file(file_name);
		run_script(file);
		file.close();
	}
	else
	{
		run_script(std::cin);
	}
	return 0;
}


dyn_ptr dynamic_null::add_left_operand(const dynamic_int&) const
{
	return std::make_shared<dynamic_null>();
}
dyn_ptr dynamic_null::add_left_operand(const dynamic_string&) const
{
	return std::make_shared<dynamic_null>();
}
dyn_ptr dynamic_null::add_left_operand(const dynamic_null&) const
{
	return std::make_shared<dynamic_null>();
}
dyn_ptr dynamic_null::add_right_operand(const dynamic_type&) const
{
	return std::make_shared<dynamic_null>();
}

dyn_ptr dynamic_int::add_left_operand(const dynamic_null&) const
{
	return std::make_shared<dynamic_null>();
}
dyn_ptr dynamic_int::add_left_operand(const dynamic_int& lhs) const 
{
	auto new_val = lhs.get_int() + this->get_int();
	return std::make_shared<dynamic_int>(new_val);
}
dyn_ptr dynamic_int::add_left_operand(const dynamic_string& lhs) const
{
	auto new_val = lhs.get_string() + std::to_string(this->get_int());
	return std::make_shared<dynamic_string>(new_val);
}
dyn_ptr dynamic_int::add_right_operand(const dynamic_type& rhs) const
{
	return rhs.add_left_operand(*this);
}

dyn_ptr dynamic_string::add_left_operand(const dynamic_null&) const
{
	return std::make_shared<dynamic_null>();
}
dyn_ptr dynamic_string::add_left_operand(const dynamic_int& lhs) const
{
	auto new_val = std::to_string(lhs.get_int()) + this->get_string();
	return std::make_shared<dynamic_string>(new_val);
}
dyn_ptr dynamic_string::add_left_operand(const dynamic_string& lhs) const
{
	auto new_val = lhs.get_string() + this->get_string();
	return std::make_shared<dynamic_string>(new_val);
}
dyn_ptr dynamic_string::add_right_operand(const dynamic_type& rhs) const
{
	return rhs.add_left_operand(*this);
}
