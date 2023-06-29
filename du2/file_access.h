#pragma once
// file_access.h
// to be included


#ifndef FILE_ACCESS_H__
#define FILE_ACCESS_H__

#include <array>
#include <map>
#include <unordered_map>
#include <iostream>

using file_descriptor = std::size_t;			// some sort of magic
using internal_ptr = int;		// internal persistent ptr === item _index_ within the file (0-based)
constexpr internal_ptr null_internal_ptr = -1;		// nullptr
constexpr internal_ptr root_internal_ptr = 0;		// ptr to the root item

// persistent data structure
template <typename ValueType, size_t arity>
struct persistent_node {
	ValueType value;
	std::array< internal_ptr, arity> ptr;
};

struct MyData {
	uint32_t i32;
	std::array<char, 8> ca;
};

inline MyData c{100, std::array<char, 8>{'a', 'b', 'c', 'd', 'e', 'f', 'g', '2'}};
inline MyData d{ 200, std::array<char, 8>{'a', 'b', 'c', 'd', 'e', 'f', 'g', '2'} };
inline MyData e{ 300, std::array<char, 8>{'a', 'b', 'c', 'd', 'e', 'f', 'g', '2'} };
inline MyData g{ 400, std::array<char, 8>{'a', 'b', 'c', 'd', 'e', 'f', 'g', '2'} };
inline MyData i{ 500, std::array<char, 8>{'a', 'b', 'c', 'd', 'e', 'f', 'g', '2'} };

inline persistent_node<MyData, 3> aa{ c, {0, 1, null_internal_ptr} }; //0 100
inline persistent_node<MyData, 3> bb{ d, { 1, 2, 0} }; //1 200
inline persistent_node<MyData, 3> cc{ e, {2, 3, 1} }; //2 300
inline persistent_node<MyData, 3> ff{ g, { 3, 4,2} }; //3 400
inline persistent_node<MyData, 3> ii{ i, { 4, null_internal_ptr,3} }; //4 400




template <typename PersistentData>
class file_access {
public:
	file_access() { init(); }
	bool read(file_descriptor fd, internal_ptr ip, PersistentData& item)
	{
		auto it = (file_.find(ip));
		if (it != file_.end())
		{
			item = it->second;
			return true;
		}
		return false;
	}
	
private:
	void init()
	{
		
		file_.emplace(0, aa);
		file_.emplace(1, bb);
		file_.emplace(2, cc);
		file_.emplace(3, ff);
		file_.emplace(4, ii);

	}
	std::unordered_map<internal_ptr, PersistentData> file_;
};

/* specializations IMPLEMENTATION-DEFINED */

#endif
