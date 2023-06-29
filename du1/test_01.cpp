#include "tagged_vector.hpp"

#include "test_util.hpp"

#include <iterator>
#include <type_traits>
#include <iostream>
#include <sstream>
#include <concepts>

struct pear_tag : tag_base {};
struct apple_tag : tag_base {
    using carrier_type = int;
};

void test_pears()
{
    range_class<pear_tag> pear_range(1, 10);
    range_class<apple_tag> apple_range(-2, 2);

    
    vector_class<double, pear_tag> pear_weights(pear_range, 3.14);
    vector_class<double, apple_tag> apple_weights(apple_range, 9.81);
    vector_class<int, pear_tag> pear_counts(pear_range, 1);
    vector_class<int, apple_tag> apple_counts(apple_range, 4);
    
    for (auto i : pear_range)
    {
        std::cout << "pear[" << i << "]: " << pear_counts[i] << " * " << pear_weights[i] << std::endl;
    }
    
    for (auto i : apple_range)
    {
        std::cout << "apple[" << i << "]: " << apple_counts[i] << " * " << apple_weights[i] << std::endl;
    }
    
    double total = 0.0;
    for (auto i : pear_range)
        total += pear_counts[i] * pear_weights[i];
    for (auto i : apple_range)
        total += apple_counts[i] * apple_weights[i];

    for (auto i : pear_range)
    {
        //total += apple_counts[i] * apple_weights[i];
        BRACKET_TEST(apple_counts, i, false);
        BRACKET_TEST(apple_weights, i, false);
    }
}
/*
int main(int argc, char** argv)
{
    try {
        test_pears();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
*/