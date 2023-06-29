#include <iostream>
#include <string>
#include <fstream>
#include <istream>
#include <vector>

class Parser {
public:
	Parser(std::ifstream& in, std::ofstream& out) : in_(in), out_(out) {};
	void parse()
	{
		parsing();
	}

private:
	std::vector<std::string> tagstack;
	std::vector<char> ctag;
	std::vector<char> cwsp;
	std::ifstream& in_;
	std::ofstream& out_;

	bool tagtext = false;
	bool wasspace = false;
	bool wasnl = false;
	bool intag = false;
	char c;

	bool eline(std::string& tag)
	{
		if (tag == "H1" || tag == "H2" || tag == "p" || tag == "HR")
		{
			return true;
		}
		return false;
	}
	void err(std::string& s)
	{
		std::cerr << s << std::endl;
		exit(1);
	}
	char uppercase(char c)
	{
		if (c <= 'z' && c >= 'a')
		{
			c -= 'a' - 'A';
		}
		return c;
	}
	void horizline()
	{
		for (size_t i = 0; i < 40; i++)
		{
			out_ << "-";
		}
	}

	void parsing()
	{
		while (in_.get(c))
		{
			if (!intag)
			{
				if (c == '<')
				{
					ctag.erase(ctag.begin(), ctag.end());
					wasnl = false;
					wasspace = true;
					intag = true;
				}
				else
				{
					if (!isspace(c))
					{
						if (tagstack.empty())
						{
							std::string error = "payload w/o tag";
							err(error);
						}
						if (tagstack.back() == "LI" && !tagtext)
						{
							int depth = 0;
							for (auto it = tagstack.begin(); it != tagstack.end(); it++)
							{
								if (*it == "UL")
								{
									depth++;
								}
							}
							int spaces = depth * 4;
							out_ << std::endl;
							for(size_t i = 0; i < spaces; i++)
							{
								out_ << " ";
							}
							out_ << "* ";

							for (auto it = cwsp.begin(); it != cwsp.end(); it++)
							{
								out_ << *it;
							}
							cwsp.clear();
						}
						
						if (tagstack.back() == "H1")
						{
							out_ << uppercase(c);
						}
						else if (tagstack.back() == "H2")
						{
							if (wasspace) {
								out_ << uppercase(c);
							}
							else 
							{
								out_ << c;
							}
						}
						else if (tagstack.back() == "HR")
						{
							std::string error = "text in Hr tag";
							err(error);
						}
						else if(tagstack.back() == "UL")
						{
							std::string error = "text in UL tag";
							err(error);
						}
						else
						{
							out_ << c;
						}
						wasspace = false;
						tagtext = true;
					}
					else
					{
						if (!tagstack.empty())
						{
							if (tagstack.back() != "HR" && tagstack.back() != "UL")
							{
								char iot = 0;
								if (c == '\n')
								{
									if (!wasnl)
									{
										iot = ' ';
									}
									wasnl = true;
								}
								else
								{
									iot = c;
									wasnl = false;
								}
								if (iot)
								{
									if (tagstack.back() == "LI" && !tagtext)
									{
										cwsp.push_back(iot);
									}
									else
									{
										out_ << iot;
									}
								}
							}
							wasspace = true;
						}
					}
				}
			}
			else
			{
				if (c == '>')
				{
					intag = false;

					if (ctag.size() < 1)
					{
						std::string error = "empty tag";
						err(error);
					}
					if (ctag.at(0) == '/')
					{
						std::string tag(ctag.begin()+1, ctag.end());
						if (tag.size() == 0)
						{
							std::string error = "empty closing tag";
							err(error);
						}

						if (tagstack.empty())
						{
							std::string error = "closing tag with no tag open";
							err(error);
						}
						std::string last = tagstack.back();
						if (last == tag)
						{
							tagstack.pop_back();
						}
						else
						{
							std::string error = "closing tag that was not opened";
							err(error);
						}
						if (eline(tag))
						{
							out_ << std::endl;
							out_ << std::endl;
						}
					}
					else
					{
						std::string tag(ctag.begin(), ctag.end());
						if (!tagstack.empty())
						{
							std::string last = tagstack.back();
							if ((last == "LI" && !tagtext) || (last == "UL" && tag == "LI"))
							{

							}
							else
							{
								std::string error = "bad tag nesting";
								err(error);
							}
						}
						if (eline(tag))
						{
							out_ << std::endl;
						}
						if (tag == "HR")
						{
							horizline();
						}
						tagstack.push_back(tag);
						ctag.clear();
					}
					tagtext = false;
				}
				else
				{
					ctag.push_back(uppercase(c));
				}
			}
		}
	}
};


int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);
	if (args.size() != 0)
	{
		std::ifstream in(args[0]);
		std::ofstream out(args[1]);
		Parser p(in, out);
		p.parse();

	}
	else
	{
		std::cout << "No arguments" << std::endl;
		return 8;
	}
}