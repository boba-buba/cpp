#pragma once
#ifndef tagged_vector_hpp
#include <iostream>
#include <ranges>
#include <vector>
#include <concepts>
#include <type_traits>
#include <istream>
#include <iterator>
#include <limits>
#include <type_traits>

struct tag_base { using carrier_type = std::size_t; };

template <typename T>
concept tag = std::is_base_of<tag_base, T>::value;
template<tag T> using index_carrier_type = T::carrier_type;

template<tag T> class index_class {
public:
	index_class() { v_ = std::numeric_limits<index_carrier_type<T>>::max(); };
	index_class(index_carrier_type<T> icr) : v_(icr) {};
	index_class(const index_class& ic)  
	{
		if (std::numeric_limits<index_carrier_type<T>>::max() == ic.v_)
		{
			throw std::range_error("Uninitialized index");
		}
		v_ = ic.v_;
	};
	index_class& operator=(const index_class& ic)
	{
		if (this != &ic)
		{
			if (std::numeric_limits<index_carrier_type<T>>::max() == ic.v_)
			{
				throw std::range_error("Uninitialized index");
			}
			v_ = ic.v_;
		}
		return *this;
	}
	index_carrier_type<T> value() //This function shall be const.
	{
		if (std::numeric_limits<index_carrier_type<T>>::max() == v_)
		{
			throw std::range_error("Uninitialized index");
		}
		return v_;
	}
	bool operator==(const index_class<T>& second) const 
	{ 
		if (std::numeric_limits<index_carrier_type<T>>::max() == v_)
		{
			throw std::range_error("Uninitialized index");
		}
		return v_ == second.v_; 
	}
	bool operator!=(const index_class<T>& second) const 
	{ 
		if (std::numeric_limits<index_carrier_type<T>>::max() == v_)
		{
			throw std::range_error("Uninitialized index");
		}
		return !(v_ == second.v_); 
	}

private:
	index_carrier_type<T> v_;
};

template<tag T>
std::ostream& operator<<(std::ostream& os, index_class<T>& val) //The second argument shall be const
{
	if (std::numeric_limits<index_carrier_type<T>>::max() == val.value())
	{
		throw std::range_error("Uninitialized index");
	}
	os << val.value();
	return os;
}

template<tag T> class BDit
{
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = index_class<T>;
	using reference = index_class<T>;
	using difference_type = typename std::make_signed<index_carrier_type<T>>::type;
	BDit() {};
	BDit(index_carrier_type<T> i) :i_(i) {}
	BDit(const BDit& bdit){ i_ = bdit.i_; }
	BDit& operator=(const BDit& bdit)
	{
		i_ = bdit.i_;
		return *this;
	}
	BDit& operator++() { i_++; return *this; }
	BDit operator++(int) { auto t = *this; operator++(); return t; }
	BDit& operator--() { i_--; return *this; }
	BDit operator--(int) { auto t = *this; operator--(); return t; }

	bool operator==(const BDit& second) const { return i_ == second.i_; }
	bool operator!=(const BDit& second) const { return i_ != second.i_; }
	reference operator*()const { return index_class<T>(i_); }

private:
	index_carrier_type<T> i_;
};


template<tag T> class range_class {
public:
	range_class(index_carrier_type<T> ib, index_carrier_type<T> ie) : ib_(ib), ie_(ie) {};
	range_class(index_carrier_type<T> ie) :ib_(0), ie_(ie) {};
	range_class(const range_class& rc) : ib_(rc.ib_), ie_(rc.ie_) {};
	range_class<T>& operator=(const range_class<T> & rc)
	{
		if (this != &rc)
		{
			ib_ = rc.ib_;
			ie_ = rc.ie_;
		}
		return *this;
	}
	bool operator==(const range_class<T>& second) const { return ((ib_ == second.ib_) && (ie_ == second.ie_)); }
	bool operator!=(const range_class<T>& second) const { return !((ib_ == second.ib_) && (ie_ == second.ie_)); }


	BDit<T> begin() const 
	{ 
		return BDit<T>(ib_);
	}
	BDit<T> end() const 
	{ 
		return BDit<T>(ie_); 
	}
	std::size_t length()
	{
		return ie_ - ib_;
	}
private:
	index_carrier_type<T> ib_;
	index_carrier_type<T> ie_;
};

template<typename E, tag T> class vector_class
{
public:
	vector_class() : r_(range_class<T>(0, 0)) {};
	vector_class(range_class<T> r, const E& val = E()) : r_(r), val_(val)
	{
		std::size_t size_v = r.length();
		v_.insert(v_.begin(), size_v, val_);
	}; 
	vector_class(const vector_class& vc) :v_(vc.v_), r_(vc.r_) {};
	vector_class& operator=(const vector_class& vc)
	{
		if (this != &vc)
		{
			v_ = vc.v_;
			r_ = vc.r_;
		}
		return *this;
	}
	vector_class(vector_class&& vc) :v_(std::move(vc.v_)), r_(std::move(vc.r_)) {};
	vector_class& operator=(const vector_class&& vc)  //There should be no const in an R - value reference.
	{
		if (this != &vc)
		{
			v_ = std::move(vc.v_);
			r_ = std::move(vc.r_);
		}
		return *this;
	}
	E& operator[](index_class<T> ind)
	{
		if (std::numeric_limits<index_carrier_type<T>>::max() == ind)
		{
			throw std::range_error("Uninitialized index");
		}
		auto cbegin = (*r_.begin()).value();
		auto index = ind.value() - cbegin;
		return v_[index];
	}

	/*There must be two operators for correct implementation of logical constness:

		E& operator[](...);
		const E& operator[](...) const;

	*/
	range_class<T> range() const { return r_; }
private:
	std::vector<E> v_;
	range_class<T> r_;
	E val_ = E();
};

#endif // !tagged_vector_hpp
