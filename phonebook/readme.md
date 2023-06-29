Your task is implementing a class named phonebook capable of storing (in memory) a list of employees and their phone numbers.

Your code will be compiled and linked together with test code; therefore, your code must provide the types, constants, and functions referenced in the test code. All the required types, constants, and functions are member of the class phonebook.

The class phonebook must be defined in a header file named phonebook.hpp. You may submit any number of .cpp files containing the implementation; you may also submit other header files included from the phonebook.hpp or elsewhere.

A possible skeleton of the submitted files is here:

    phonebook.hpp
    phonebook.cpp

The interface of class phonebook is not completely fixed; you may alter any definition as long as the test code is compilable.

The test code is available here:

    phonebook_test1.cpp
    phonebook_test2.cpp

The class phonebook must contain the following definitions:

struct person_entry
{
	using phone_list = std::vector< std::string>;
	std::string first_name;
	std::string last_name;
	std::string dept;
	phone_list phones;
};

(You may alter the definiton of phone_list type; however, it must behave like a sequential container containing std::string.)

using person_handle = /*...*/;

The person_handle must be a lightweight type (supporting copy operations in constant time) which provides programmatical identification of a person entry in the database.

using person_handle_list = std::vector< person_handle>;

(You may alter the definiton of person_handle_list type; however, it must behave like a sequential container containing person_handle.)

enum dept_filter { any_dept, dept_equal, dept_substring	};
enum last_name_filter { any_last_name, last_name_prefix	};

person_handle_list find(
		dept_filter p_dept, const std::string& s_dept,
		last_name_filter p_last_name, const std::string& s_last_name);

The member function find shall return a list of handles to the person entries which satisfy a condition specified by the function arguments. The condition is a conjunction of two sub-conditions, one acting on the dept attribute of person entries, the other on the last_name attribute. The p_... arguments specify the shape of the conditions, the s_... arguments are their parameters:

    dept_equal specifies the sub-condition dept == s_dept
    dept_substring means s_dept is a substring of dept
    last_name_prefix means s_last_name is a prefix of last_name
    any_... means that the particular sub-condition is always true (the corresponding s_... is ignored)

const person_entry& get_person_entry(person_handle h) const;

Returns (the const reference to) the person entry identified by the handle. This function must work in (at most) logarithmic time (wrt. the total number of entries in the phonebook).

person_handle add_person(const person_entry& z);

Creates a new person entry in the phonebook, as a copy of the parameter z. This function must allow creation of duplicate entries.

void add_phone(person_handle h, const std::string& phone);

Adds a phone number to the person entry identified by the handle. Does nothing if this number already exists in the person entry.

void remove_phone(person_handle h, const std::string& phone);

Adds the specified phone number from the person entry identified by the handle. Does nothing if there is no such phone number.

void change_last_name(person_handle h, const std::string& last_name);

Changes the last_name attribute of the person entry identified by the handle.

void change_dept(person_handle h, const std::string& dept);

Changes the dept attribute of the person entry identified by the handle.

void remove_person(person_handle h);

Removes the person entry identified by the handle from the phonebook. The handle is no longer usable after calling this function (no diagnostics required).
