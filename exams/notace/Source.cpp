#include <vector>
#include <iostream>
#include <string>
#include <fstream>


class Node
{
public:
	void set_value(std::string& val)
	{
		value_ = val;
	}
	void set_left(Node l)
	{
		left_  = std::move(std::make_shared<Node>(l));
	}
	void set_right(Node r)
	{
		right_ = std::move(std::make_shared<Node>(r));
	}
	void print_value()
	{
		std::cout << value_;
	}
	std::string get_value()
	{
		return value_;
	}
	std::shared_ptr<Node> left_ = nullptr;
	std::shared_ptr<Node> right_ = nullptr;
private:

	std::string value_;
};


class Tree
{
	int i = 0;
public:
	Node root;
	Tree(std::vector<std::string>& v) : v_(v) { };
	
	void create_root_tree()
	{
		root = construct_tree();
	}
	Node construct_tree()
	{
		while (i < v_.size())
		{
			if ((v_[i] != "+" && v_[i] != "-" && v_[i] != "*" && v_[i] != "/") && 
				(v_[i] != "#+" && v_[i] != "#-" && v_[i] != "#*" && v_[i] != "#/"))
			{
				Node list;
				list.set_value(v_[i]);
				return list;
			}
			else
			{
				Node node;
				node.set_value(v_[i]);
				i++;
				auto l = construct_tree();
				node.set_left(l);
				i++;
				auto r = construct_tree();
				node.set_right(r);
				return node;
			}
		}
	}
	void print_tree(std::shared_ptr<Node> r)
	{
		if (r == nullptr) return;
		bool left_p = false;
		bool right_p = false;
		bool paren = false;
		
		std::string operat = r.get()->get_value();
		if (operat == "#+" || operat == "#-" || operat == "#*" || operat == "#/")
		{
			std::string o = operat.substr(1);
			r.get()->set_value(o);
			paren = true;
		}
		else
		{
			if ((operat == "*" && (r.get()->left_.get()->get_value() == "+" || r.get()->left_.get()->get_value() == "-"))
				|| (operat == "/" && (r.get()->left_.get()->get_value() == "+" || r.get()->left_.get()->get_value() == "-")))
				left_p = true;

			if ((operat == "-" && (r.get()->right_.get()->get_value() == "+" || r.get()->right_.get()->get_value() == "-"))
				|| (operat == "*" && (r.get()->right_.get()->get_value() == "+" || r.get()->right_.get()->get_value() == "-"))
				|| (operat == "/" && (r.get()->right_.get()->get_value() == "+" || r.get()->right_.get()->get_value() == "*" || r.get()->right_.get()->get_value() == "-")))
				right_p = true;
		}
		if (paren) std::cout << "-(";
		if (left_p) std::cout << "(";
		print_tree(r.get()->left_);
		if (left_p) std::cout << ")";
		r.get()->print_value();
		if (right_p) std::cout << "(";
		print_tree(r.get()->right_);
		if (right_p) std::cout << ")";
		if (paren) std::cout << ")";
	}
private:
	std::vector<std::string>& v_;
};

class Parser
{
public:
	using vector = std::vector<std::string > ;
	Parser(std::ifstream& input) : input_(input) {};

	void parse_file()
	{
		std::string line;
		while (input_.good())
		{
			std::getline(input_, line);
			auto v = parse_line(line);
			if (v.size() == 0) continue;
			Tree tree(v);
			tree.create_root_tree();
			tree.print_tree(std::make_shared<Node>(tree.root));
			std::cout << std::endl;
		}
	}
	std::vector<std::string> parse_line(const std::string& line)
	{
		std::vector<std::string> expression;
		std::string element = "";
		bool negative = false;
		for (auto&& c : line)
		{
			if (c == '-' || c == '+' || c == '/' || c == '*')
			{
				std::string op = "";
				if (negative) {
					op += "#";
					negative = false;
				}
				 op += c;
				expression.push_back(op);
			}
			if (c == '#')
			{
				negative = true;
			}
			if (isalnum(c))
			{
				element += c;
				if (negative)
				{
					element = "-" + element;
					//element += c;
					expression.push_back(element);
					element = "";
					negative = false;
				}
				else {
					expression.push_back(element);
					element = "";
				}
			}
		}
		return expression;
	}

private:
	std::ifstream& input_;
};

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);
	if (args.size() != 0)
	{
		for (size_t i = 0; i < args.size(); i++)
		{
			std::ifstream file(args[i]);
			Parser p(file);
			p.parse_file();
		}
	}
	else
	{
		std::cout << "no file" << std::endl;
		return 8;
	}
	return 0;
}