#ifndef phonebook_hpp_
#define phonebook_hpp_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <sstream>
#include <memory>
#include <unordered_map>


class phonebook
{
public:
	struct person_entry
	{
		using phone_list = std::vector<std::string>;

		std::string first_name;
		std::string last_name;
		std::string dept;
		phone_list phones;
	};

	using person_handle = std::shared_ptr<person_entry>;

	using person_handle_list = std::vector<person_handle>;
	//
	using last_names = std::map < std::string, person_handle_list>;
	last_names LastNamePrefix;
	std::unordered_map<std::string, last_names> Department;
	//
	enum dept_filter
	{
		any_dept,
		dept_equal,
		dept_substring
	};

	enum last_name_filter
	{
		any_last_name,
		last_name_prefix
	};

	person_handle_list find(
		dept_filter p_dept,
		const std::string& s_dept,
		last_name_filter p_last_name,
		const std::string& s_last_name);


	const person_entry& get_person_entry(person_handle h) const;

	void add_Last_name(phonebook::person_handle h);
	void add_Department(phonebook::person_handle h);
	void erase_Department(person_handle h);
	void erase_Last_name(person_handle h);

	person_handle add_person(const person_entry& z);

	void add_phone(person_handle h, const std::string& phone);

	void remove_phone(person_handle h, const std::string& phone);

	void change_last_name(person_handle h, const std::string& last_name);

	void change_dept(person_handle h, const std::string& dept);

	void remove_person(person_handle h);
};

#endif

/**/




