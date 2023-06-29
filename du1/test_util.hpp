#ifndef test_util_hpp_
#define test_util_hpp_

#define TEST_STATIC_SAFETY
#define TEST_OUTPUT

#if defined(TEST_STATIC_SAFETY) || defined (TEST_OUTPUT)
#include <iomanip>
#include <iostream>
#include <string>
#endif

namespace test_util {

	inline const std::string pok = "    ";
	inline const std::string pfail = "!!! ";

#ifdef TEST_STATIC_SAFETY
	template< bool T, bool T2>
	struct print_once_bool {
		print_once_bool(const std::string& n)
		{
			if constexpr (T)
				if constexpr (T2)
					std::cout << pok << n << " works as expected." << std::endl;
				else
					std::cout << pfail << n << " works but it is expected to FAIL." << std::endl;
			else
				if constexpr (T2)
					std::cout << pfail << n << " FAILS but it is expected to work." << std::endl;
				else
					std::cout << pok << n << " fails as expected." << std::endl;
		}
	};
#endif

	template< typename T, T V, T V2>
	struct print_once_const {
		print_once_const(const std::string& n)
		{
			if constexpr (V == V2)
				std::cout << pok << n << " equals " << V2 << " as expected." << std::endl;
			else
				std::cout << pfail << n << " equals " << V << " BUT " << V2 << " was expected." << std::endl;
		}
	};

	template< typename T>
	inline void print_value(T V, T V2, const std::string& n, const std::string& n2)
	{
		if (V == V2)
			std::cout << pok << n << " equals " << n2 << " as expected." << std::endl;
		else
			std::cout << pfail << n << " is DIFFERENT from the expected " << n2 << "." << std::endl;
	}

	template< typename T>
	inline void print_ostream_value(T V, const std::string& V2, const std::string& n)
	{
		std::ostringstream oss;
		oss << V;
		std::string s = oss.str();
		if (s == V2)
			std::cout << pok << "std::cout << " << n << "; prints \"" << V2 << "\" as expected." << std::endl;
		else
			std::cout << pfail << "std::cout << " << n << "; prints \"" << s << "\" BUT \"" << V2 << "\" was expected." << std::endl;
	}

	template< typename T, typename T2>
	struct print_once_type {
		print_once_type(const std::string& n, const std::string& n2)
		{
			if constexpr (std::is_same_v<T, T2>)
				std::cout << pok << n << " is same as " << n2 << " as expected." << std::endl;
			else
				std::cout << pfail << n << " is DIFFERENT from the expected type " << n2 << "." << std::endl;
		}
	};

	template< typename E, typename T>
	inline void print_exception(T F, const std::string& n, const std::string& n2)
	{
		try {
			F();
			std::cout << pfail << n << "; did NOT throw any exception but " << n2 << " was expected." << std::endl;
		}
		catch (const E& e)
		{
			std::cout << pok << n << "; threw " << n2 << " as expected." << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << pfail << n << "; threw " << e.what() << " which is DIFFERENT from the expected " << n2 << "." << std::endl;
		}
		catch (...)
		{
			std::cout << pfail << n << "; threw an unknown exception but NOT the expected " << n2 << "." << std::endl;
		}
	}



	template< typename A, typename I>
	concept bracket = requires (A a, I i) { a[i]; };

	template< typename A, typename B>
	concept equal = requires (A a, B b) { {a == b} -> std::same_as<bool>; };

	template< template<typename ...> class T, typename ... TL>
	concept template_type = requires { typename T<TL ...>; };
}

#ifdef TEST_STATIC_SAFETY

#define BRACKET_TEST(a,i,ok) ([](){ static test_util::print_once_bool<test_util::bracket<decltype(a),decltype(i)>,ok> v( #a "[" #i "]"); }())

#define EQUAL_TEST(a,i,ok) ([](){ static test_util::print_once_bool<test_util::equal<decltype(a),decltype(i)>,ok> v( #a "==" #i); }())

#endif

#define TEMPLATE_TYPE_1_TEST(T,T1,ok) ([](){ static test_util::print_once_bool<test_util::template_type<T,T1>,ok> v( #T "<" #T1 ">"); }())

#define TEMPLATE_TYPE_2_TEST(T,T1,T2,ok) ([](){ static test_util::print_once_bool<test_util::template_type<T,T1,T2>,ok> v( #T "<" #T1 "," #T2 ">"); }())

#define CONST_TEST(V,ok) ([](){ static test_util::print_once_const<std::size_t,V,ok> v( #V); }())

#define TYPE_TEST(T,ok) ([](){ static test_util::print_once_type<T,ok> v( #T, #ok); }())

#define VALUE_TEST(V,ok) (test_util::print_value(V,ok,#V,#ok))

#define EXCEPTION_TEST(V,ok) (test_util::print_exception<ok>([&](){ V; },#V,#ok))

#define OSTREAM_TEST(V,ok) (test_util::print_ostream_value(V,ok,#V))

#endif