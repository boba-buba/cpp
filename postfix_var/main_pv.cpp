#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <memory>
#include <limits>


//template<typename T>
namespace parser { 
	
	
	using val_t = std::int_least32_t;
	//using val_t = T;
	void parse(std::istream& is, val_t from, val_t to);
	val_t  convert(const std::string& num);
	bool div_zero = false;

	class AK {
	public:
		virtual ~AK() {};
		virtual val_t  eval(val_t x_num) = 0;
	};
	using ptr_t = std::unique_ptr<AK>;

	class K_VAR : public AK{
	public:
		K_VAR(){}
		virtual val_t eval(val_t x_num) { return x_num; }
		val_t value(val_t num)
		{ 
			return num; 
		}
		
	};
	class K_BIN : public AK {
	public:
		K_BIN(char o, ptr_t&& l, ptr_t&& r) : op_(o), left_(move(l)), right_(move(r)){}

		virtual val_t eval(val_t x_num) 
		{
			auto x = left_->eval(x_num);
			auto y = right_->eval(x_num);
			switch (op_)
			{
				case '+': return x + y;
				case '-': return x - y;
				case '*': return x * y;
				case '/': 
				{
					if (y == 0) { div_zero = true; return 0; }
					return x / y;
				}
			}
			throw std::logic_error("Unknown operator " + op_);	
		}
	private: 
		char op_;
		ptr_t left_, right_;

	};
	class K_NUM : public AK 
	{
	public:
		K_NUM(val_t v) : value_(v){}
		virtual val_t eval(val_t x_num)
		{
			return value_;
		}

	private:
		val_t value_;
	};

	using vec_t = std::vector<ptr_t>;
	
	namespace impl {

		class parser_class {
		private:
			friend void parser::parse(std::istream& is, val_t from, val_t to);
			parser_class(std::istream* isp) : isp_(isp) {}
			char next() const
			{
				return next_;
			}
			void next_char()
			{
				char c;
				c = is().get();
				next_ = c;
			}
			
			std::istream& is()
			{
				return *isp_;
			}
			std::istream* isp_;
			char next_ = is().get();
			
			void expression_from_file(val_t from, val_t to);
			void min_max(val_t from, val_t to);

			void add_el(val_t el) 
			{ 
				expres_.emplace_back(std::make_unique<K_NUM>(K_NUM(el)));
			}
			void add_bin_op(char el) 
			{ 
				if (expres_.size() < 2) throw std::runtime_error("Incorrect expression: less then necessary values in stack");
				ptr_t p1 = std::move(expres_.back());
				expres_.pop_back();
				ptr_t p2 = std::move(expres_.back());
				expres_.pop_back();

				expres_.emplace_back(std::make_unique<K_BIN>(K_BIN(el, std::move(p2), std::move(p1))));
			}
			void add_x() 
			{ 
				expres_.emplace_back(std::make_unique<K_VAR>(K_VAR()));
			}

			vec_t expres_;
		};
			
		void parser_class::min_max(val_t from, val_t to)
			{
				val_t minimum = std::numeric_limits<val_t>::max();
				val_t maximum = std::numeric_limits<val_t>::min();
				for (val_t i = from; i <= to; i++)
				{
					try
					{
						if (expres_.size() > 1)  throw std::runtime_error("Incorrect expression: too many values in stack");
						val_t k = expres_.back()->eval(i);
						if (!div_zero)
						{
							minimum = std::min(k, minimum);
							maximum = std::max(k, maximum);
						}
						else div_zero = false;
					}
					catch (std::exception& e)
					{
						std::cerr << e.what() << std::endl;
						return;
					}		
				}
				std::cout << "min="<<minimum << "  " <<"max="<< maximum << std::endl;
			}

		void parser_class::expression_from_file(val_t from, val_t to)
		{
			val_t x = 0;
			try
			{
				while (next() != EOF)
				{
					if (isdigit(next()))
					{
						while (isdigit(next())) {
							x = x * 10 + (val_t)next() - 48;
							next_char();
						}
						add_el(x);
						x = 0;
					}
					else if (next() == 'x')
					{
						add_x();
						next_char();
					}
					else if (next() == '/' || next() == '*' || next() == '+' || next() == '-')
					{
						add_bin_op(next());
						next_char();
					}
					else
					{
						next_char();
					}
				}
				if (expres_.size() == 0) throw std::runtime_error("Empty file");
				min_max(from, to);
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
	}

	void parse(std::istream& is, val_t f, val_t t)
	{
		impl::parser_class p(&is);
		p.expression_from_file(f, t);	
	}

	val_t convert(const std::string& num)
	{
		val_t f = 0;
		bool minus = false;
		std::size_t j = 0;
		if (num[0] == '-') { minus = true; j = 1; }
		for (std::size_t i = j; i < num.size(); ++i)
		{
			if (!isdigit(num[i])) throw std::runtime_error("Incorrect format of the bounds of an interval");
			int digit = (int)num[i] - 48;
			f = f * 10 + digit;
		}
		if (minus) return -f;
		return f;
	}
}

int main(int argc, char** argv)
{
	try {
		if (argc < 4)
		{
			throw std::runtime_error("Missing some of the paarameters: file name or the bounds of an interval");
		}

		std::string filename = argv[1];

		// read the file:
		{
			std::ifstream is(filename);

			if (!is.good())
			{
				throw std::runtime_error("Cannot open file \"" + filename + "\"");
			}
			parser::parse(is, parser::convert(argv[2]), parser::convert(argv[3]));
			
		}
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception" << std::endl;
	}
	return 8;
}
