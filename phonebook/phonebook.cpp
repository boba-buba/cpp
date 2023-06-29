#include "phonebook.hpp"

phonebook::person_handle_list phonebook::find(
	dept_filter p_dept,
	const std::string& s_dept,
	last_name_filter p_last_name,
	const std::string& s_last_name)
{
	person_handle_list b;

	switch (p_last_name)
	{
	case phonebook::any_last_name:
	{
		switch (p_dept)
		{
		case phonebook::any_dept:
		{
			for (auto it = LastNamePrefix.begin(); it != LastNamePrefix.end(); ++it)
			{
				for (auto it1 = (*it).second.begin(); it1 != (*it).second.end(); ++it1)
				{
					b.emplace_back(*it1);
				}
			}
			break;
		}
		case phonebook::dept_equal:
		{
			auto it = Department.find(s_dept);
			if (it != Department.end())
			{
				for (auto it1 = (*it).second.begin(); it1 != (*it).second.end(); ++it1)
				{
					for (auto it2 = (*it1).second.begin(); it2 != (*it1).second.end(); ++it2)
					{
						b.emplace_back(*it2);
					}
				}
			}
			break;
		}
		case phonebook::dept_substring:
		{
			for (auto it = Department.begin(); it != Department.end(); ++it)
			{
				if ((*it).first.find(s_dept) != std::string::npos)
				{
					for (auto it1 = (*it).second.begin(); it1 != (*it).second.end(); ++it1)
					{
						for (auto it2 = (*it1).second.begin(); it2 != (*it1).second.end(); ++it2)
						{
							b.emplace_back(*it2);
						}
					}
				}
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	case phonebook::last_name_prefix:
	{
		switch (p_dept)
		{
		case phonebook::any_dept:
		{
			for (auto it = LastNamePrefix.begin(); it != LastNamePrefix.end(); ++it)
			{
				if ((*it).first.find(s_last_name) == 0)
				{
					for (auto it1 = (*it).second.begin(); it1 != (*it).second.end(); ++it1)
					{
						b.emplace_back(*it1);
					}
				}
			}
			break;
		}
		case phonebook::dept_equal:
		{
			auto it = Department.find(s_dept);

			if (it != Department.end())
			{
				for (auto it1 = (*it).second.begin(); it1 != (*it).second.end(); ++it1)
				{
					if ((*it1).first.find(s_last_name) == 0)
					{
						for (auto it2 = (*it1).second.begin(); it2 != (*it1).second.end(); ++it2)
						{
							b.emplace_back(*it2);
						}
					}
				}
			}
			break;
		}
		case phonebook::dept_substring:
		{
			for (auto it = Department.begin(); it != Department.end(); ++it)
			{
				if ((*it).first.find(s_dept) != std::string::npos)
				{
					for (auto it1 = (*it).second.begin(); it1 != (*it).second.end(); ++it1)
					{
						if ((*it1).first.find(s_last_name) == 0)
						{
							for (auto it2 = (*it1).second.begin(); it2 != (*it1).second.end(); ++it2)
							{
								b.emplace_back(*it2);
							}
						}
					}
				}
			}
			break;
		}
		default: break;
		}
		break;
	}
	default:
		break;
	}
	return b;
}


const phonebook::person_entry& phonebook::get_person_entry(person_handle h) const
{
	auto it_dp = Department.find((*h).dept);
	if (it_dp != Department.end())
	{
		auto it_dp_ln = (*it_dp).second.find((*h).last_name);
		if (it_dp_ln != (*it_dp).second.end())
		{
			for (auto it = (*it_dp_ln).second.begin(); it != (*it_dp_ln).second.end(); ++it)
			{
				if (*it == h)
					return *(*it).get();
			}
		}
	}
	throw std::runtime_error("Invalid handle");
}

void phonebook::add_Last_name(phonebook::person_handle h)
{
	std::string ln = (*h).last_name;
	auto it_ln = LastNamePrefix.find(ln);
	if (it_ln != LastNamePrefix.end())
	{
		(*it_ln).second.emplace_back(std::move(h));
		return;
	}
	LastNamePrefix.emplace(ln, person_handle_list{ std::move(h) });

}

void phonebook::add_Department(phonebook::person_handle h)
{
	std::string dp = (*h).dept;
	std::string ln = (*h).last_name;
	auto it_dp = Department.find(dp);
	if (it_dp != Department.end())
	{
		auto it_ln_dp = (*it_dp).second.find(ln);
		if (it_ln_dp != (*it_dp).second.end())
		{
			(*it_ln_dp).second.emplace_back(std::move(h));
		}
		else
		{
			(*it_dp).second.emplace(ln, person_handle_list{ std::move(h) });
		}
		return;
	}
	Department.emplace(dp, last_names{ });
	auto it = Department.find(dp);
	(*it).second.emplace(ln, person_handle_list{ std::move(h) });
}


phonebook::person_handle phonebook::add_person(const person_entry& z)
{
	phonebook::person_handle p = std::make_shared<person_entry>(z);
	add_Last_name(p);
	add_Department(p);
	return p;
}

void phonebook::add_phone(person_handle h, const std::string& phone)
{
	for (auto it = (*h).phones.begin(); it != (*h).phones.end(); ++it)
	{
		if (phone == *it) return;
	}
	(*h).phones.emplace_back(phone);
}

void phonebook::remove_phone(person_handle h, const std::string& phone)
{
	for (auto it = (*h).phones.begin(); it != (*h).phones.end(); ++it)
	{
		if (phone == *it)
		{
			(*h).phones.erase(it);
			return;
		}
	}
}

void phonebook::erase_Department(person_handle h)
{
	auto it_dp = Department.find((*h).dept);
	if (it_dp != Department.end())
	{
		auto it_dp_ln = (*it_dp).second.find((*h).last_name);
		if (it_dp_ln != (*it_dp).second.end())
		{
			for (auto it = (*it_dp_ln).second.begin(); it != (*it_dp_ln).second.end(); ++it)
			{
				if (*it == h)
				{
					if ((*it_dp_ln).second.size() == 1) (*it_dp).second.erase((*h).last_name);
					else (*it_dp_ln).second.erase(it);
					if ((*it_dp).second.size() == 0)
					{
						Department.erase((*h).dept);
					}
					break;
				}
			}
		}
	}
}

void phonebook::erase_Last_name(person_handle h)
{
	auto it_ln = LastNamePrefix.find((*h).last_name);
	if (it_ln != LastNamePrefix.end())
	{
		for (auto it = (*it_ln).second.begin(); it != (*it_ln).second.end(); ++it)
		{
			if (*it == h)
			{
				if ((*it_ln).second.size() == 1) LastNamePrefix.erase(it_ln);
				else (*it_ln).second.erase(it);
				break;
			}
		}
	}
}

void phonebook::change_last_name(person_handle h, const std::string& last_name)
{
	erase_Last_name(h);
	erase_Department(h);

	(*h).last_name = last_name;
	add_Department(h);
	add_Last_name(h);
}

void phonebook::change_dept(person_handle h, const std::string& dept)
{
	erase_Department(h);

	(*h).dept = dept;
	add_Department(h);

}

void phonebook::remove_person(person_handle h)
{

	erase_Last_name(h);
	erase_Department(h);

}

/**/





