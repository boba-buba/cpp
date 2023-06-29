#include "tagged_vector.hpp"

#include "test_util.hpp"

namespace test_simple_globals {

	//	using tagged::tag_base;
	//	using tagged::range_class;
	//	using tagged::index_class;
	//	using tagged::vector_class;

	struct i_tag : tag_base {};
	struct j_tag : tag_base {};
	struct k_tag : tag_base {};

	using i_range = range_class< i_tag>;
	using j_range = range_class< j_tag>;
	using k_range = range_class< k_tag>;

	using i_index = index_class< i_tag>;
	using j_index = index_class< j_tag>;
	using k_index = index_class< k_tag>;

	using j_vector = vector_class< double, j_tag>;
	using ij_matrix = vector_class< j_vector, i_tag>;
	using k_vector = vector_class< double, k_tag>;
	using ik_matrix = vector_class< k_vector, i_tag>;
	using kj_matrix = vector_class< j_vector, k_tag>;
}

void test_simple(std::size_t in, std::size_t jn, std::size_t kn)
{
	using namespace test_simple_globals;

	i_range ir(1, in + 1);
	j_range jr(1, jn + 1);
	k_range kr(1, kn + 1);
	

	ij_matrix ct(ir, j_vector(jr));
	ik_matrix at(ir, k_vector(kr));
	kj_matrix bt(kr, j_vector(jr));

	double x = 0.0;
	for (auto&& i : ir)
	{
		for (auto&& k : kr)
		{
			at[i][k] = x;
			x += 1.0;
#ifdef TEST_OUTPUT
			std::cout << "at[" << i << "," << k << "] = " << at[i][k] << std::endl;
#endif
		}
	}

	double y = 1.0;
	for (auto&& k : kr)
	{
		for (auto&& j : jr)
		{
			bt[k][j] = y;
			y += 2.0;
#ifdef TEST_OUTPUT
			std::cout << "bt[" << k << "," << j << "] = " << bt[k][j] << std::endl;
#endif
		}
	}

	for (auto&& i : ir)
	{
		for (auto&& j : jr)
		{
			for (auto&& k : kr)
			{
#ifdef TEST_STATIC_SAFETY
				BRACKET_TEST(at, i, true);
				BRACKET_TEST(at, j, false);
				BRACKET_TEST(at, k, false);
				BRACKET_TEST(at[i], i, false);
				BRACKET_TEST(at[i], j, false);
				BRACKET_TEST(at[i], k, true);
				BRACKET_TEST(bt, i, false);
				BRACKET_TEST(bt, j, false);
				BRACKET_TEST(bt, k, true);
				BRACKET_TEST(bt[k], i, false);
				BRACKET_TEST(bt[k], j, true);
				BRACKET_TEST(bt[k], k, false);
				BRACKET_TEST(ct, i, true);
				BRACKET_TEST(ct, j, false);
				BRACKET_TEST(ct, k, false);
				BRACKET_TEST(ct[i], i, false);
				BRACKET_TEST(ct[i], j, true);
				BRACKET_TEST(ct[i], k, false);
#endif
				ct[i][j] += at[i][k] * bt[k][j];
			}
		}
	}

	for (auto&& i : ir)
	{
		for (auto&& j : jr)
		{
#ifdef TEST_OUTPUT
			std::cout << "ct[" << i << "," << j << "] = " << ct[i][j] << std::endl;
#endif
		}
	}
}
/*
int main(int argc, char** argv)
{
	try {
		std::size_t in = 3, jn = 4, kn = 5;

		if (argc == 4)
		{
			in = std::stoi(argv[1]);
			jn = std::stoi(argv[2]);
			kn = std::stoi(argv[3]);
		}

		test_simple(in, jn, kn);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}*/