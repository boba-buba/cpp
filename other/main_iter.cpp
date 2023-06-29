#include <iostream>
#include <iterator>




template<typename T, typename D>

class iota_Iterator
{

private:
	using self_ = iota_Iterator<T, D>;
public:


	using difference_type = std::ptrdiff_t;
	using iterator_category = std::forward_iterator_tag;
	using value_type = T;
	using pointer = void;
	using reference = T;
	iota_Iterator() {}
	iota_Iterator(T i, D c): i_(i), c_(c){}
	
	T operator*() const { return i_; }

	constexpr self_& operator++() requires std::forward_iterator<iota_Iterator>
	{ i_ += c_; return *this; }
	iota_Iterator operator++(int) { auto t = *this; operator++(); return t; }

	T& operator*()
	{
		return i_;
	}
	//const T& operator*() const { return i_; }
	bool operator!=(const self_& b) { return i_ != b.i_; }
	bool operator==( const self_& b) { return i_ == b.i_; }

private:
	T i_;
	D c_;

};

class Integers
{
public:
	struct Iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = int;
		using pointer = int*;
		using reference = int&;

		Iterator(pointer ptr) : m_ptr(ptr) {}

		reference operator*() const { return *m_ptr; }
		pointer operator->() { return m_ptr; }
		Iterator& operator++() { m_ptr++; return *this; }
		Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
		friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
		friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

	private:
		pointer m_ptr;
	};

	Iterator begin() { return Iterator(&m_data[0]); }
	Iterator end() { return Iterator(&m_data[200]); }

private:
	int m_data[200];
};



template< typename T, typename D>
class iota_t {
public:
	iota_t(T a, T b, D c) : a_(a), b_(b), c_(c) {}
	using iterator = iota_Iterator<T, D>;

	iterator begin() const { return iterator(a_, c_); }
	iterator end() const { return iterator(b_, c_); }

private:
	T a_, b_;
	D c_;

};

template<typename T, typename D>
iota_t<T, D> iota(T a, T b, D c)
{
	return iota_t<T, D>(a, b, c);
}


int main(int argc, char** argv)
{
	for (auto&& i : iota(3, 9, 2))
	{
		std::cout << i << std::endl;
	}
	if (true == std::forward_iterator<decltype(iota(3, 9, 2).begin())>)
		std::cout << "OK\n";

	Integers integers;
	for (auto i : integers)
		std::cout << i << "\n";
	if (true == std::input_iterator<decltype(integers.begin())>)
		std::cout << "OK\n";
	return 0;
}