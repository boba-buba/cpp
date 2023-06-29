#include "zavorky.hpp"

std::string remove_parentheses(const std::string& line);
std::string parse_line(const std::string& line);


int process_line(const std::string& in_line)
{
	std::string line = parse_line(in_line);
	std::string ouline = remove_parentheses(line);
	std::cout << ouline << std::endl;
	return 0;
}

std::string parse_line(const std::string& line)
{
	std::string outline = "";
	for (size_t i = 0; i < line.size(); i++)
	{
		if (isdigit(line[i])) return "";
		if (!isspace(line[i]))
		{
			outline += line[i];
		}
	}
	return outline;
}

bool is_mono(const std::string& expr)
{
	int par_lvl = 0;
	if (expr[0] == '/')
	{
		return false;
	}
	for (size_t i = 1; i < expr.size(); i++)
	{
		char znak = expr[i];
		if ((znak == '+' || znak == '-') && par_lvl <= 1)
		{
			return false;
		}
		if (znak == '(')
		{
			par_lvl++;
		}
		if (znak == ')')
		{
			par_lvl--;
		}
	}
	return true;
}


std::string remove_parentheses(const std::string& line)
{
	int par_lvl = 0;
	int max_par_lvl = 0;
	std::string out_string = line;
	std::string wip_string = "";

	while (out_string != wip_string)
	{
		wip_string = out_string;
		out_string = "";
		std::vector<bool> purge_param;
		std::vector<size_t> param_beg;


		for (size_t i = 0; i < wip_string.size(); i++)
		{
			if (wip_string[i] == '(')
			{
				bool purge;
				if (i == 0)
				{
					purge = true;
				}
				else
				{
					if (wip_string[i - 1] == '+')
					{
						purge = true;
					}
					else
					{
						purge = false;
					}
					if (isalnum(wip_string[i - 1]))
					{
						return "";
					}
				}



				if (par_lvl < max_par_lvl)
				{
					purge_param[par_lvl] = purge;
					param_beg[par_lvl] = i;
				}
				else
				{
					purge_param.push_back(purge);
					param_beg.push_back(i);
				}

				par_lvl++;
				max_par_lvl++;
			}
		
			if (wip_string[i] == ')')
			{
				par_lvl--;
				if (par_lvl < 0)
				{
					return "";
				}
				if (i == wip_string.size() - 1)
				{
					purge_param[par_lvl] == purge_param[par_lvl] && true;

				}
				else
				{
					if (wip_string[i + 1] == '+' || wip_string[i + 1] == '-' || wip_string[i + 1] == ')')
					{
						purge_param[par_lvl] = purge_param[par_lvl] && true;
					}
					else
					{
						purge_param[par_lvl] = false;
					}
					if (isalnum(wip_string[i + 1]))
					{
						return "";
					}
				}

				std::string expr;
				size_t  beg_of_purge_param = param_beg[par_lvl];

				if (beg_of_purge_param == 0)
				{
					expr = wip_string.substr(beg_of_purge_param +1, i - beg_of_purge_param -1);
				}
				else
				{
					expr = wip_string.substr(beg_of_purge_param - 1, i - beg_of_purge_param);
				}

				purge_param[par_lvl] = purge_param[par_lvl] && is_mono(expr);

				if (purge_param[par_lvl])
				{
					std::string pre_par = wip_string.substr(0, beg_of_purge_param);
					std::string par_con = wip_string.substr(beg_of_purge_param + 1, i - beg_of_purge_param - 1);
					std::string post_par = wip_string.substr(i + 1);
					out_string = pre_par + par_con + post_par;
					wip_string = out_string;
					i -= 2;
				}

			}
			if (out_string == "")
			{
				out_string = wip_string;
			}
		}
		if (par_lvl != 0)
		{
			return "";
		}

	}
	return out_string;

}


