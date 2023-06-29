#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <array>

class Assembler
{
public:
	
	bool f_register = false;
	int pc = 0;
	int pc_prev = -1;
	std::map<std::string, int> instructions
	{
		{"LDC", 2}, {"MOV", 2}, {"LD", 2}, {"ST", 2},
		{"ADD", 2}, {"SUB", 2}, {"MUL", 2}, {"DIV", 2}, {"MOD", 2},
		{"EQ", 2}, {"LT", 2}, {"LE", 2}, {"JMP", 1},
		{"JF", 1}, {"JNF", 1}, {"OUT", 1}
	};

	bool reg_in_range(int reg)
	{
		if (reg >= 0 && reg < registers.size())
			return true;
		return false;
	}
	void ldc(int reg, int c)
	{
		if (reg_in_range(reg))
		{
			registers[reg] = c;
		}
	}
	void mov(int reg1, int reg2)
	{
		if (reg_in_range(reg1) && reg_in_range(reg2))
		{
			registers[reg1] = registers[reg2];
		}
	}
	void ld(int reg1, int reg2)
	{
		if (reg_in_range(reg1) && reg_in_range(reg2) && reg_in_range(registers[reg2]))
		{
			registers[reg1] = registers[registers[reg2]];
		}
	}
	void st(int r1, int r2)
	{
		if (reg_in_range(r1) && reg_in_range(r2) && reg_in_range(registers[r1]))
		{
			registers[registers[r1]] = registers[r2];
		}
	}

	void add(int r1, int r2)
	{
		if (reg_in_range(r1) && reg_in_range(r2))
		{
			registers[r1] += registers[r2];
		}
	}
	void sub(int r1, int r2)
	{
		if (reg_in_range(r1) && reg_in_range(r2))
		{
			registers[r1] -= registers[r2];
		}
	}
	void mul(int r1, int r2)
	{
		if (reg_in_range(r1) && reg_in_range(r2))
		{
			registers[r1] *= registers[r2];
		}
	}
	void div(int r1, int r2)
	{
		if (reg_in_range(r1) && reg_in_range(r2))
		{
			registers[r1] /= registers[r2];
		}
	}
	void mod(int r1, int r2)
	{
		if (reg_in_range(r1) && reg_in_range(r2))
		{
			registers[r1] %= registers[r2];
		}
	}
	
	void eq(int r1, int r2)
	{
		if (reg_in_range(r1) && reg_in_range(r2))
		{
			if (registers[r1] == registers[r2]) f_register = true;
			else f_register = false;
		}
	}
	void lt(int r1, int r2)
	{
		if (reg_in_range(r1) && reg_in_range(r2))
		{
			if (registers[r1] < registers[r2]) f_register = true;
			else f_register = false;
		}
	}
	void le(int r1, int r2)
	{
		if (reg_in_range(r1) && reg_in_range(r2))
		{
			if (registers[r1] <= registers[r2]) f_register = true;
			else f_register = false;
		}
	}
	
	void jmp(int offset)
	{

		pc += (offset);
	}
	
	void jf(int offset)
	{
		if (f_register)
		{
			pc += (offset + 1);
		}
	}
	void jnf(int offset)
	{
		if (!f_register)
		{
			pc += (offset + 1);
		}
	}
	void out(int r)
	{
		if (reg_in_range(r))
		{
			std::cout << registers[r] << std::endl;
		}
	}
	
private:
	std::array<int, 256> registers{};

};

class Parser
{
public:
	Assembler a;
	Parser(std::istream& input) : input_(input) { };
	
	std::vector<std::string> get_tokens(std::string& line)
	{
		std::vector<std::string> tokens;
		
		std::stringstream ss(line);
		while (true)
		{
			std::string token;
			ss >> token;
			if (!ss) return tokens;
			tokens.push_back(token);

		}
	}

	void parse_input()
	{
		std::string line;
		std::vector<std::string> program;
		
		while (true)
		{
			
			std::getline(input_, line);
			
			if (!input_) break;
			try
			{
				//parse_line(line);
				program.push_back(line);
			}
			catch (std::runtime_error e)
			{
				std::cout << e.what() << std::endl;
			}
		}
		while (true)
		{
			if (a.pc == program.size()) return;
			parse_line(program[a.pc]);

		}

	}
	
	void parse_line(std::string& line)
	{
		a.pc_prev = a.pc;
		a.pc++;
		std::vector<std::string> tokens = get_tokens(line);
		if (tokens.size() == 0 || tokens[0] == ";") return;
		
		auto inst = a.instructions.find(tokens[0]);
		if (inst == a.instructions.end())
		{
			throw std::runtime_error("Unknown instruction"); // cout and return
		}
		if (inst->first == "LDC")
		{
			try {
				int reg = std::stoi(tokens[1].substr(1));
				int c = std::stoi(tokens[2]);
				a.ldc(reg, c);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "LD")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.ld(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "MOV")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.mov(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "ST")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.st(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "ADD")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.add(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "SUB")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.sub(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "MUL")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.mul(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "DIV")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.div(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "MOD")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.mod(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "EQ")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.eq(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "LT")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.lt(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "LE")
		{
			try {
				int reg1 = std::stoi(tokens[1].substr(1));
				int reg2 = std::stoi(tokens[2].substr(1));
				a.le(reg1, reg2);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "JMP")
		{
			try {
				int reg = std::stoi(tokens[1]);
				a.jmp(reg);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "JF")
		{
			try {
				int reg = std::stoi(tokens[1]);
				a.jf(reg);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "JNF")
		{
			try {
				int reg = std::stoi(tokens[1]);
				a.jnf(reg);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		else if (inst->first == "OUT")
		{
			try {
				int reg = std::stoi(tokens[1].substr(1));
				a.out(reg);
			}
			catch (std::exception e)
			{
				std::cout << "wrong arguments" << std::endl;
				return;
			}
		}
		
	}


private:
	std::istream& input_;
};


int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv+argc);
	if (args.size() != 0)
	{
		
		std::ifstream file(args[0]);
		Parser p(file);
		p.parse_input();
	}
	else
	{
		Parser p(std::cin);
		p.parse_input();
	}
	return 0;
}
