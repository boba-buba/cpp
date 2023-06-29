#include<iostream>
#include <vector>
#include <fstream>
#include <istream>
#include<string>
#include <deque>

class tetris {
public:
	tetris() {}

	void read_input(std::istream& input)
	{

		input >> h;
		input >> w;
		for (int i = 0; i < h; i++)
		{
			std::vector<int> row;

			for (int j = 0; j < w; j++)
			{
				row.push_back(0);
			}
			field.push_back(row);
		}
		std::string coord = "";
		while (input.good())
		{
			input >> coord;
			if (!input.good()) break;
			falling(std::stoi(coord));
		}
	}
	bool in_borders(int x, int y)
	{
		if ((x >= 0 && x < w) && (y >= 0 && y < h)) return true;
		else return false;
	}

	std::pair<int, int> get_coord(int y, int x)
	{
		if (field[y][x] != 0) return std::make_pair(y, x);

	}

	bool find_stable_neighbour(int y, int x)
	{
		if (field[y][x] == 2) return true;
		std::deque<std::pair<int, int>> q;
		if (in_borders(y - 1, x) && field[y - 1][x] != 0) q.push_back(std::make_pair(y - 1, x));
		if (in_borders(y + 1, x) && field[y + 1][x] != 0) q.push_back(std::make_pair(y + 1, x));
		if (in_borders(y, x - 1) && field[y][x - 1] != 0) q.push_back(std::make_pair(y, x - 1));
		if (in_borders(y, x + 1) && field[y][x + 1] != 0) q.push_back(std::make_pair(y, x + 1));
		while (!q.empty())
		{
			std::pair<int, int> coord = q.front();
			q.pop_front();
			if (field[coord.first][coord.second] == 2 || coord.first == 0) return true;
			else
			{
				if (in_borders(coord.first - 1, coord.second))
					q.push_back(get_coord(coord.first - 1, coord.second));

				if (in_borders(coord.first + 1, coord.second))
					q.push_back(get_coord(coord.first + 1, coord.second));

				if (in_borders(coord.first, coord.second - 1))
					q.push_back(get_coord(coord.first, coord.second - 1));

				if (in_borders(coord.first, coord.second + 1))
					q.push_back(get_coord(coord.first, coord.second + 1));
			}
		}
		return false;
	}

	bool find_bottom(int y, int x)
	{
		if (y == 0) return true;
		std::deque<std::pair<int, int>> q;
		if (in_borders(y - 1, x) && field[y - 1][x] != 0) q.push_back(std::make_pair(y - 1, x));
		if (in_borders(y + 1, x) && field[y + 1][x] != 0) q.push_back(std::make_pair(y + 1, x));
		if (in_borders(y, x - 1) && field[y][x - 1] != 0) q.push_back(std::make_pair(y, x - 1));
		if (in_borders(y, x + 1) && field[y][x + 1] != 0) q.push_back(std::make_pair(y, x + 1));
		while (!q.empty())
		{
			std::pair<int, int> coord = q.front();
			q.pop_front();
			if (coord.first == 0) return true;
			else
			{
				if (in_borders(coord.first - 1, coord.second))

					q.push_back(get_coord(coord.first - 1, coord.second));

				if (in_borders(coord.first + 1, coord.second))

					q.push_back(get_coord(coord.first + 1, coord.second));

				if (in_borders(coord.first, coord.second - 1))

					q.push_back(get_coord(coord.first, coord.second - 1));

				if (in_borders(coord.first, coord.second + 1))

					q.push_back(get_coord(coord.first, coord.second + 1));
			}
		}
		return false;
	}

	void delete_row(int y)
	{
		for (int j = 0; j < w; j++)
		{
			field[y][j] = 0;
		}
		//draw_field(); std::cout << "\n";
		for (int i = h - 1; i > y; i--)
		{
			for (int j = 0; j < w; j++)
			{
				if (field[i][j] == 2) field[i][j] = 1;
			}
		}
		for (int i = y - 1; i > -1; i--)
		{
			for (int j = 0; j < w; j++)
			{
				if (!find_bottom(i, j)) field[i][j] = 1;
			}
		}
		//draw_field();
		all_cubes_fall(y);
		//draw_field();
	}

	void all_neigbours_to_stable(int y, int x)
	{
		std::deque<std::pair<int, int>> q;
		if (in_borders(y - 1, x) && field[y - 1][x] == 1) q.push_back(std::make_pair(y - 1, x));
		if (in_borders(y + 1, x) && field[y + 1][x] == 1) q.push_back(std::make_pair(y + 1, x));
		if (in_borders(y, x - 1) && field[y][x - 1] == 1) q.push_back(std::make_pair(y, x - 1));
		if (in_borders(y, x + 1) && field[y][x + 1] == 1) q.push_back(std::make_pair(y, x + 1));
		while (!q.empty())
		{
			std::pair<int, int> coord = q.front();
			q.pop_front();
			if (field[coord.first][coord.second] == 1)
			{
				if (find_stable_neighbour(coord.first, coord.second))
					field[coord.first][coord.second] = 2;
				if (in_borders(coord.first - 1, coord.second) && field[coord.first - 1][coord.second] == 1)
					q.push_back(std::make_pair(coord.first - 1, coord.second));

				if (in_borders(coord.first + 1, coord.second) && field[coord.first + 1][coord.second] == 1)
					q.push_back(std::make_pair(coord.first + 1, coord.second));

				if (in_borders(coord.first, coord.second - 1) && field[coord.first][coord.second - 1] == 1)
					q.push_back(std::make_pair(coord.first, coord.second - 1));

				if (in_borders(coord.first, coord.second + 1) && field[coord.first][coord.second + 1] == 1)
					q.push_back(std::make_pair(coord.first, coord.second + 1));
			}
		}
	}

	bool field_has_ones()
	{
		for (int i = h - 1; i >= 0; i--)
		{
			for (int j = 0; j < w; j++)
			{
				if (field[i][j] == 1)
				{
					return true;
				}
			}
		}
		return false;
	}
	void all_cubes_fall(int row)
	{
		//draw_field(); std::cout << "\n";
		while (field_has_ones())
		{
			for (int i = 0; i < h; i++)
			{
				for (int j = 0; j < w; j++)
				{
					if (field[i][j] == 1)
					{
						field[i][j] = 0;
						field[i - 1][j] = 1;
						if ((i - 1 == 0) || find_stable_neighbour(i - 1, j))
						{
							field[i - 1][j] = 3;
						}

					}
				}
			}
			//draw_field();
			for (int i = h - 1; i >= 0; i--)
			{
				for (int j = 0; j < w; j++)
				{
					if (field[i][j] == 3)
					{
						all_neigbours_to_stable(i, j);
						field[i][j] = 2;
					}
				}
			}
		}
	}

	void annihilation()
	{
		int count = 0;
		for (int i = h - 1; i > -1; i--)
		{
			count = 0;
			for (int j = 0; j < w; j++)
			{
				if (field[i][j] == 2) count++;
			}
			if (count == w)
			{
				std::string s = "A " + std::to_string(i);
				falling_sequence.push_back(s);
				delete_row(i);
			}
		}
	}

	bool stable_neightbour(int y, int x) //check for borders
	{
		if (in_borders(y - 1, x) && field[y - 1][x] == 2) return true;
		if (in_borders(y + 1, x) && field[y + 1][x] == 2) return true;
		if (in_borders(y, x - 1) && field[y][x - 1] == 2) return true;
		if (in_borders(y, x + 1) && field[y][x + 1] == 2) return true;
		return false;

	}

	void falling(int cube)
	{
		int coord_x = cube;
		int coord_y = 9;
		bool stable = false;
		while (!stable)
		{
			if (coord_y == 0 || stable_neightbour(coord_y, coord_x))
			{
				field[coord_y][coord_x] = 2;
				stable = true;
				falling_sequence.push_back(std::to_string(coord_y));
				annihilation();
			}
			else
			{
				coord_y--;
			}
			//draw_field(); std::cout << '\n';
		}
	}

	void draw_field()
	{
		for (std::size_t i = 0; i < falling_sequence.size(); i++)
		{
			std::cout << falling_sequence[i] << std::endl;
		}
		for (int i = h - 1; i > -1; i--)
		{
			for (int j = 0; j < w; j++)
			{
				if (field[i][j] != 0) std::cout << "#";
				else std::cout << ".";
			}
			std::cout << "\n";
		}
	}

private:
	std::vector<std::string> falling_sequence;
	int h = 0;
	int w = 0;
	std::vector<std::vector<int>> field;
};

int main(int argv, char** argc)
{
	std::vector<std::string> args(argc, argc + argv);
	if (args.size() > 1)
	{
		std::ifstream f(args[1]);
		tetris tet{};
		tet.read_input(f);
		tet.draw_field();
	}
	else
	{
		tetris tet{};
		tet.read_input(std::cin);
		tet.draw_field();
	}
	return 0;
}