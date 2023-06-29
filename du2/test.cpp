#include "pers_ptr.h"

// test.cpp
// main with some tests


//void f(const MyData&) {}

int main()
{
	cached_container< MyData, 3, EvictionOldestUnlock> cc{ 3, 0 };
                      			
	uint32_t sum = 0;
	auto other_ptr = cc.null_ptr();    // null (invalid) ptr
	auto other_ptr2 = cc.null_ptr();
	
	//auto p =  cc.root_ptr();
	for (auto p = cc.root_ptr(); p; p = p.get_ptr(1)) {    // traversing from the root ptr while the following ptr is valid
		//f(*p);    // MyData&
		
		std::cout << p->i32 << "\n";
		other_ptr = p.get_ptr(0);		
		other_ptr2 = p.get_ptr(2);
		sum += p->i32 * (other_ptr ? other_ptr->i32 : 1);
	}

	
	std::cout << sum << std::endl;
	//third_ptr = std::move(cc.root_ptr());

	//auto fourth_ptr = third_ptr.get_ptr(0);
	//auto fifth_ptr = std::move(fourth_ptr);


	//std::cout << fourth_ptr->i32 << std::endl;
	//std::cout << fifth_ptr->i32 << std::endl;

	//for(auto b = cc.root_ptr(); b; )
	
	return 0;
}
