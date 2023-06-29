/*#include "phonebook.hpp"

#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <iostream>

#ifdef MEASURE_TIME
#include <chrono>
#endif

std::string randstring(std::size_t rr, std::size_t size)
{
	std::string s;

	for (; size > 0; --size)
	{
		char ch = 'a' + (char)(rr % 10);

		rr = (rr * 729 + 21) % 1021;

		s.push_back(ch);
	}

	return s;
}

void addstring(std::string& a, const std::string& b)
{
	auto bs = std::min(b.size(), std::size_t(1));
	if (a.size() < bs)
		a.resize(bs, 'a');

	for (std::size_t i = 0; i < bs; ++i)
	{
		a[i] = 'a' + (a[i] + b[i] + 14) % 26;
	}
}

template< typename E>
void filldomain(E&& engine, std::vector< std::string>& v, std::size_t size, std::size_t domain_size)
{
	for (std::size_t c = 0; c < domain_size; ++c)
	{
		v.push_back(randstring(c, size));
	}

	std::shuffle(v.end() - domain_size, v.end(), engine);
}

void fill(
	phonebook& s,
	std::size_t c,
	std::size_t first_name_s,
	std::size_t first_name_ds,
	std::size_t last_name_s,
	std::size_t last_name_ds,
	std::size_t dept_s,
	std::size_t dept_ds,
	std::size_t phone_c,
	std::size_t phone_s,
	std::size_t phone_ds)
{
	std::mt19937_64 engine(729);

	std::vector< std::string> v_first_name;
	std::vector< std::string> v_last_name;
	std::vector< std::string> v_dept;
	std::vector< std::string> v_phone;

	filldomain(engine, v_first_name, first_name_s, first_name_ds);
	filldomain(engine, v_last_name, last_name_s, last_name_ds);
	filldomain(engine, v_dept, dept_s, dept_ds);
	filldomain(engine, v_phone, phone_s, phone_ds);

	std::uniform_int_distribution<std::size_t> first_name_distro(0, first_name_ds - 1);
	std::uniform_int_distribution<std::size_t> last_name_distro(0, last_name_ds - 1);
	std::uniform_int_distribution<std::size_t> dept_distro(0, dept_ds - 1);
	std::uniform_int_distribution<std::size_t> phone_distro(0, phone_ds - 1);

	for (; c > 0; --c)
	{
		phonebook::person_entry z;

		z.first_name = v_first_name[first_name_distro(engine)];
		z.last_name = v_last_name[last_name_distro(engine)];
		z.dept = v_dept[dept_distro(engine)];

		for (std::size_t cc = phone_c; cc > 0; --cc)
		{
			z.phones.push_back(v_phone[phone_distro(engine)]);
		}

		auto it = std::unique(z.phones.begin(), z.phones.end());
		z.phones.erase(it, z.phones.end());

		s.add_person(z);
	}
}

void dump(const phonebook& s, const phonebook::person_handle_list& b)
{
	for (auto it = b.begin(); it != b.end(); ++it)
	{
		phonebook::person_handle h = *it;
		const phonebook::person_entry& a = s.get_person_entry(h);
		std::cout << a.first_name << " " << a.last_name << " " << a.dept;
		for (auto it2 = a.phones.begin();
			it2 != a.phones.end();
			++it2)
		{
			std::cout << " " << *it2;
		}
		std::cout << std::endl;
	}
}

std::pair<std::size_t, std::string> scan(const phonebook& s, const phonebook::person_handle_list& b)
{
	std::string buf_first_name;
	std::string buf_last_name;
	std::string buf_dept;
	std::string buf_phone;
	for (auto it = b.begin(); it != b.end(); ++it)
	{
		phonebook::person_handle h = *it;
		const phonebook::person_entry& a = s.get_person_entry(h);
		addstring(buf_first_name, a.first_name);
		addstring(buf_last_name, a.last_name);
		addstring(buf_dept, a.dept);
		for (auto it2 = a.phones.begin();
			it2 != a.phones.end();
			++it2)
		{
			addstring(buf_phone, *it2);
		}
	}
	return std::make_pair(b.size(), buf_first_name + buf_last_name + buf_dept + buf_phone);
}


int main(int argc, char** argv)
{
	phonebook s;

#ifdef MEASURE_TIME
	auto c0 = std::chrono::steady_clock::now();
#endif

	fill(s, 10000, 5, 10, 8, 1000, 30, 300, 3, 4, 10000);

#ifdef MEASURE_TIME
	auto c1 = std::chrono::steady_clock::now();
#endif

#ifdef _DEBUG
	std::size_t n = 100;
	std::size_t n2 = 100;
	std::size_t n3 = 100;
#else
	std::size_t n = 10000;
	std::size_t n2 = 10000;
	std::size_t n3 = 10000;
#endif
	std::pair<std::size_t, std::string> ss1;

	for (std::size_t c = n; c > 0; --c)
	{
		phonebook::person_handle_list b = s.find(
			phonebook::dept_substring, randstring(0, 3),
			phonebook::any_last_name, "");

		ss1 = scan(s, b);
	}

#ifdef MEASURE_TIME
	auto c3 = std::chrono::steady_clock::now();
#endif

	std::pair<std::size_t, std::string> ss2;

	for (std::size_t c = n2; c > 0; --c)
	{
		phonebook::person_handle_list b2 = s.find(
			phonebook::any_dept, "",
			phonebook::last_name_prefix, randstring(0, 2));

		ss2 = scan(s, b2);
	}

#ifdef MEASURE_TIME
	auto c5 = std::chrono::steady_clock::now();
#endif

	std::pair<std::size_t, std::string> ss3;

	for (std::size_t c = n3; c > 0; --c)
	{
		phonebook::person_handle_list b3 = s.find(
			phonebook::dept_substring, randstring(0, 2),
			phonebook::last_name_prefix, randstring(0, 1));

		ss3 = scan(s, b3);
	}

#ifdef MEASURE_TIME
	auto c7 = std::chrono::steady_clock::now();
#endif

	std::cout << "Result dept_substring: N=" << ss1.first << " checksum=" << ss1.second << std::endl;
	std::cout << "Result last_name_prefix: N=" << ss2.first << " checksum=" << ss2.second << std::endl;
	std::cout << "Result dept_substring+last_name_prefix: N=" << ss3.first << " checksum=" << ss3.second << std::endl;

#ifdef MEASURE_TIME
	std::cout << "Time fill: " << std::chrono::duration_cast<std::chrono::milliseconds>(c1 - c0).count() << std::endl;
	std::cout << "Time dept_substring: " << std::chrono::duration_cast<std::chrono::milliseconds>(c3 - c1).count() << std::endl;
	std::cout << "Time last_name_prefix: " << std::chrono::duration_cast<std::chrono::milliseconds>(c5 - c3).count() << std::endl;
	std::cout << "Time dept_substring+last_name_prefix: " << std::chrono::duration_cast<std::chrono::milliseconds>(c7 - c5).count() << std::endl;
#endif

	return 0;
}*/

#include "phonebook.hpp"

#include <iostream>

void dump(const phonebook& s, const phonebook::person_handle_list& b)
{
	for (auto it = b.begin(); it != b.end(); ++it)
	{
		phonebook::person_handle h = *it;
		const phonebook::person_entry& a = s.get_person_entry(h);
		std::cout << a.first_name << " " << a.last_name << " " << a.dept;
		for (auto it2 = a.phones.begin();
			it2 != a.phones.end();
			++it2)
		{
			std::cout << " " << *it2;
		}
		std::cout << std::endl;
	}
}


int main(int argc, char** argv)
{
	phonebook s;

	{
		phonebook::person_entry z;

		z.first_name = "Melinda";
		z.last_name = "French";
		z.dept = "Marketing";
		z.phones.push_back("4271");

		phonebook::person_handle h2 = s.add_person(z);
		s.add_phone(h2, "4323");
	}

	phonebook::person_handle_list b = s.find(
		phonebook::dept_equal, "Marketing",
		phonebook::last_name_prefix, "Fr");

	dump(s, b);

	if (b.begin() != b.end())
	{
		phonebook::person_handle h = *b.begin();
		s.add_phone(h, "4111");
		s.remove_phone(h, "4323");
		s.change_last_name(h, "Gates");
	}
	else
	{
		std::cout << "Not found" << std::endl;
	}

	phonebook::person_handle_list b2 = s.find(
		phonebook::dept_equal, "Marketing",
		phonebook::any_last_name, "");

	dump(s, b2);

	if (b.begin() != b.end())
	{
		phonebook::person_handle h = *b.begin();
		s.remove_person(h);
	}

	return 0;
}