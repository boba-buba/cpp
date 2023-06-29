#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <map>



class Function {
public:
	virtual ~Function() = default;
	virtual std::string print() { return "fun(" + std::to_string(arg_count()) + ")"; };
	virtual int arg_count() = 0;
	virtual void pass_argument(std::shared_ptr<Function> f) = 0;
	virtual void pass_first(std::shared_ptr<Function> f) = 0;
	virtual void pass_second(std::shared_ptr<Function> f) = 0;
	virtual std::shared_ptr<Function> evaluate() = 0;
	virtual std::shared_ptr<Function> clone() = 0;
	virtual int value() { throw std::runtime_error("|Argument error|"); }
};

class ConstantFunction : public Function {
public:
	explicit ConstantFunction(int val) : value_(val) {};
	virtual std::string print() override
	{
		return std::to_string(value_);
	}
	virtual int arg_count() override
	{
		return 0;
	}

	virtual void pass_argument(std::shared_ptr<Function> f) override {}
	virtual void pass_first(std::shared_ptr<Function> f) override {}
	virtual void pass_second(std::shared_ptr<Function> f) override {}
	virtual std::shared_ptr<Function> clone() override
	{
		return std::make_shared<ConstantFunction>(value_);
	}
	virtual int value() override
	{
		return value_;
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		return std::make_shared<ConstantFunction>(value_);
	}
private:
	int value_;
};

class BasicFunction : public Function {
public:
	virtual int arg_count() override
	{
		if (first_arg == nullptr && second_arg == nullptr) return 2;
		if (first_arg == nullptr || second_arg == nullptr) return 1;
		return 0;
	}


	virtual void pass_argument(std::shared_ptr<Function> f) override
	{
		if (first_arg == nullptr) first_arg = f;
		else if (second_arg == nullptr) second_arg = f;
	}
	virtual void pass_first(std::shared_ptr<Function> f) override
	{
		if (first_arg == nullptr) first_arg = f;
	}
	virtual void pass_second(std::shared_ptr<Function> f) override
	{
		if (second_arg == nullptr) second_arg = f;
	}
protected:
	std::shared_ptr<Function> first_arg = nullptr;
	std::shared_ptr<Function> second_arg = nullptr;
};

class AddFunction : public BasicFunction {
	
	virtual std::shared_ptr<Function> clone() override
	{
		auto ret = std::make_shared<AddFunction>();
		ret->pass_first(first_arg);
		ret->pass_second(second_arg);
		return std::move(ret);
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		int result = first_arg->value() + second_arg->value();
		return std::make_shared<ConstantFunction>(result);
	}
};

class SubFunction : public BasicFunction
{
	virtual std::shared_ptr<Function> clone() override
	{
		auto ret = std::make_shared<SubFunction>();
		ret->pass_first(first_arg);
		ret->pass_second(second_arg);
		return std::move(ret);
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		int result = first_arg->value() - second_arg->value();
		return std::make_shared<ConstantFunction>(result);
	}
};

class MulFunction : public BasicFunction
{
	virtual std::shared_ptr<Function> clone() override
	{
		auto ret = std::make_shared<MulFunction>();
		ret->pass_first(first_arg);
		ret->pass_second(second_arg);
		return std::move(ret);
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		int result = first_arg->value() * second_arg->value();
		return std::make_shared<ConstantFunction>(result);
	}
};

class DivFunction : public BasicFunction
{
	virtual std::shared_ptr<Function> clone() override
	{
		auto ret = std::make_shared<DivFunction>();
		ret->pass_first(first_arg);
		ret->pass_second(second_arg);
		return std::move(ret);
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		if (second_arg->value() == 0) throw std::runtime_error("|division by zero|");
		int result = first_arg->value() / second_arg->value();
		return std::make_shared<ConstantFunction>(result);
	}
};

class BindFunction : public Function{
public:
	virtual int arg_count() override
	{
		if (arg_f == nullptr && arg_x == nullptr) return 2;
		if (arg_x == nullptr) return  1;
		return 0;
	}
	virtual void pass_argument(std::shared_ptr<Function> f) override
	{
		if (arg_f == nullptr) arg_f = f;
		else if (arg_x == nullptr) arg_x = f;
	}
	virtual void pass_first(std::shared_ptr<Function> f) override
	{
		if (arg_f == nullptr) arg_f = f;
	}
	virtual void pass_second(std::shared_ptr<Function> f) override
	{
		if (arg_x == nullptr) arg_x = f;
	}
protected:
	std::shared_ptr<Function> arg_f = nullptr;
	std::shared_ptr<Function> arg_x = nullptr;
};

class Bind1Function : public BindFunction {
public:
	virtual std::shared_ptr<Function> clone() override
	{
		auto ret = std::make_shared<Bind1Function>();
		ret->pass_first(arg_f);
		ret->pass_second(arg_x);
		return std::move(ret);
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		auto ret = arg_f->clone();
		ret->pass_first(arg_x);
		return std::move(ret);
	}
};

class Bind2Function : public BindFunction {
public:
	virtual std::shared_ptr<Function> clone() override
	{
		auto ret = std::make_shared<Bind2Function>();
		ret->pass_first(arg_f);
		ret->pass_second(arg_x);
		return std::move(ret);
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		auto ret = arg_f->clone();
		ret->pass_second(arg_x);
		return std::move(ret);
	}
};

class DuplResult : public BindFunction {
public:
	virtual std::shared_ptr<Function> clone() override
	{
		auto ret = std::make_shared<DuplResult>();
		ret->pass_first(arg_f);
		ret->pass_second(arg_x);
		return std::move(ret);
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		auto ret = arg_f->clone();
		ret->pass_first(arg_x);
		ret->pass_second(arg_x);
		return std::move(ret->evaluate());
	}
};

class DuplFunction : public Function {
public:
	virtual int arg_count() override
	{
		if (arg_f == nullptr) return 1;
		return 0;
	}
	virtual void pass_argument(std::shared_ptr<Function> f) override
	{
		if (arg_f == nullptr) arg_f = f;
	}
	virtual void pass_first(std::shared_ptr<Function> f) override
	{
		pass_argument(f);
	}
	virtual void pass_second(std::shared_ptr<Function> f) {}
	virtual std::shared_ptr<Function> clone() override
	{
		auto ret = std::make_shared<DuplFunction>();
		ret->pass_first(arg_f);
		return std::move(ret);
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		auto ret = std::make_shared<DuplResult>();
		ret->pass_first(arg_f);
		return std::move(ret);
	}

protected:
	std::shared_ptr<Function> arg_f = nullptr;
};

class JoinResult : public BasicFunction {
public:
	virtual std::shared_ptr<Function> clone() override
	{
		auto ret = std::make_shared<JoinResult>();
		ret->pass_first(first_arg);
		ret->pass_second(second_arg);
		ret->pass_f(arg_f);
		ret->pass_g(arg_g);
		return std::move(ret);
	}
	void pass_f(std::shared_ptr<Function> f)
	{
		arg_f = f;
	}
	void pass_g(std::shared_ptr<Function> f)
	{
		arg_g = f;
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		auto f = arg_f->clone();
		auto g = arg_g->clone();
		g->pass_argument(first_arg);
		g->pass_second(second_arg);
		f->pass_argument(g->evaluate());
		return std::move(f->evaluate());
	}
protected:
	std::shared_ptr<Function> arg_f = nullptr;
	std::shared_ptr<Function> arg_g = nullptr;
};

class JoinFunction : public Function {
public:
	virtual int arg_count() override
	{
		if (arg_f == nullptr && arg_g == nullptr) return 2;
		if (arg_f == nullptr || arg_g == nullptr) return 1;
		return 0;
	}
	virtual void pass_argument(std::shared_ptr<Function> f) override
	{
		if (arg_f == nullptr) arg_f = f;
		else if (arg_g == nullptr) arg_g = f;
	}
	virtual void pass_first(std::shared_ptr<Function> f) override
	{
		if (arg_f == nullptr) arg_f = f;
	}
	virtual void pass_second(std::shared_ptr<Function> f) override
	{
		if (arg_g == nullptr) arg_g = f;
	}
	virtual std::shared_ptr<Function> clone() override
	{
		auto ret = std::make_shared<JoinFunction>();
		ret->pass_first(arg_f);
		ret->pass_second(arg_g);
		return std::move(ret);
	}
	virtual std::shared_ptr<Function> evaluate() override
	{
		auto ret = std::make_shared<JoinResult>();
		ret->pass_f(arg_f);
		ret->pass_g(arg_g);
		return std::move(ret);
	}
protected:
	std::shared_ptr<Function> arg_f = nullptr;
	std::shared_ptr<Function> arg_g = nullptr;
};


//-------------------------------------------------------------------------------------------
std::map<std::string, std::shared_ptr<Function>> variables;

std::vector<std::string> get_tokens(const std::string& line)
{
	std::stringstream ss(line);
	std::vector<std::string> tokens;

	while (true)
	{
		std::string token;
		ss >> token;
		if (!ss) return tokens;
		tokens.push_back(token);
	}
}

std::shared_ptr<Function> GetVariable(const std::string& var_name)
{
	try {
		int constant = std::stoi(var_name);
		return std::make_shared<ConstantFunction>(constant);
	}
	catch (...)
	{
		auto f = variables.find(var_name);
		if (f == variables.end())
		{
			throw std::runtime_error("|Unknown function|");
		}
		return f->second;
	} 
}

void InterpretLine(const std::string& line)
{
	std::vector<std::string> tokens = get_tokens(line);
	if (tokens.size() == 0) return;

	size_t token_index = 0;
	if (tokens[0] == "let")
	{
		if (tokens.size() < 4 || tokens[2] != "=")
			throw std::runtime_error("|Syntax error|");
		token_index = 3;
	}
	auto f = GetVariable(tokens[token_index++]);
	std::shared_ptr<Function> new_func = f->clone();

	if (new_func->arg_count() != tokens.size() - token_index)
		throw std::runtime_error("|Invalid argument count|");

	for (size_t i = 0; new_func->arg_count(); i++)
	{
		new_func->pass_argument(GetVariable(tokens[token_index + i]));
	}
	if (tokens[0] == "let")
	{
		variables[tokens[1]] = std::move(new_func->evaluate());
	}
	else std::cout << new_func->evaluate()->print() << " ";
}

void Interpreter(std::istream& input)
{
	variables.clear();
	variables.emplace("add", std::make_shared<AddFunction>());
	variables.emplace("sub", std::make_shared<SubFunction>());
	variables.emplace("mul", std::make_shared<MulFunction>());
	variables.emplace("div", std::make_shared<DivFunction>());
	variables.emplace("bind1", std::make_shared<Bind1Function>());
	variables.emplace("bind2", std::make_shared<Bind2Function>());
	variables.emplace("dupl", std::make_shared<DuplFunction>());
	variables.emplace("join", std::make_shared<JoinFunction>());

	std::string line;
	while (true)
	{
		std::getline(input, line);
		if (!input) return;
		try {
			InterpretLine(line);
		}
		catch (std::runtime_error e)
		{
			std::cout << e.what() << " ";
		}
	}
}

int main(int argc, char** argv)
{

	std::vector<std::string> files(argv, argv+argc);

	if (files.size() > 1)
	{
		for (size_t i = 1; i < files.size(); i++)
		{
			std::string name = files[i];
			std::ifstream file(name);
			Interpreter(file);
			std::cout << std::endl;
		}
	}
	else
	{
		Interpreter(std::cin);
	}
	return 0;
}