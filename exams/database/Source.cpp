#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <iostream>

struct Point {
	std::vector<double> coords;
};

class Metricsable {
public: 
	virtual ~Metricsable() {};
	virtual double GetD(Point& from, Point& to) = 0;
};

class L2Metrecs : public Metricsable {
public:
	virtual double GetD(Point& from, Point& to)
	{
		if (from.coords.size() != to.coords.size())
			throw std::exception("wrong dimension");
		double dist = 0;
		for (size_t i = 0; i < from.coords.size(); i++)
		{
			dist += (from.coords[i] * from.coords[i] - to.coords[i] * to.coords[i]);
		}
		dist = std::sqrt(dist);
		return dist;
	}
};

class CosMetrics : public Metricsable{
	const double pi = std::atan(1) * 4;
public:
	virtual double GetD(Point& from, Point& to)
	{
		if (from.coords.size() != to.coords.size())
			throw std::exception("wrong dimension");
		double mul = 0;
		double size_from = 0;
		double size_to = 0;
		for (size_t i = 0; i < from.coords.size(); i++)
		{
			mul += (from.coords[i] * to.coords[i]);
			size_from += from.coords[i] * from.coords[i];
			size_to += to.coords[i] * to.coords[i];
		}
		double sim = mul / (std::sqrt(size_from) * std::sqrt(size_to));
		double res = 1 - (std::acos(sim)) / pi;
		return res;
	}
};


class Analyzer {
	Metricsable& metrics;
	std::map<int, Point> db;
	std::map<int, Point> qrs;
	void LoadPointsToMap(std::istream& input, std::map<int, Point>& map)
	{
		std::string line;
		while (true)
		{
			if (!input.good())
			{
				if (input.eof()) break;
				else throw std::exception("File read error");
			}
			std::getline(input, line);
			if (line != "")
			{
				std::istringstream line_stream{ line };
				Point point;
				int id;
				std::string field;
				for (int i = 0; !line_stream.eof(); i++)
				{
					std::getline(line_stream, field, ';');
					if (i == 0)
						id = std::stoi(field);
					else
						point.coords.push_back(std::stod(field));
				}
				if (!map.size() == 0 && map.rbegin()->second.coords.size() != point.coords.size())
				{
					throw std::exception("wrong inpu coord sizes");
				}
				map.emplace(id, std::move(point));
			}
		}


	}

	void GetNearestKPointsFromDbToPoint(size_t k, Point point_from, std::multimap<double, std::pair<int, Point>>& result_points) 
	{
		for (auto&& point : db)
		{
			double d = metrics.GetD(point_from, point.second);
			if (k > result_points.size())
				result_points.emplace(d, point);
			else
			{
				if (result_points.rbegin()->first > d)
				{
					result_points.erase(result_points.rbegin()->first);
					result_points.emplace(d, point);
				}
			}
		}
		return;
	}

public:
	Analyzer(Metricsable& metrics) : metrics(metrics) {};

	void LoadData(std::istream& input)
	{
		LoadPointsToMap(input, db);
	}
	void LoadQueries(std::istream& input)
	{
		LoadPointsToMap(input, qrs);
	}
	/*double ReturnDistance(int db_id_1, int db_id_2)
	{
		return metrics.GetD(db[db_id_1], db[db_id_2]);
	}*/

	void PrintKNearestPointsFromDbToPoint(std::ostream& out, int k)
	{
		for (auto&& point : qrs)
		{
			out << point.first << " ";
			std::multimap<double, std::pair<int, Point>> points_near;
			GetNearestKPointsFromDbToPoint(k, point.second, points_near);

			for (auto&& point_near : points_near)
			{
				out << point_near.second.first << "|" << point_near.first << " ";
			}
			out << std::endl;
		}

	}
};


int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);
	if (args.size() != 4)
	{
		std::cout << "wrong parameters count";
		return -1;
	}
	if (args[0].size() == 0 || args[1].size() == 0 || args[3].size() == 0)
	{
		std::cout << "input/output file cannot be empty";
		return -1;
	}
	if (std::stoi(args[2]) < 0)
	{
		std::cout << "k cannot be negative";
		return -1;
	}

	try
	{
		std::ifstream db(args.at(0));
		std::ifstream qrs(args.at(1));
		int k = std::stoi(args.at(2));
		std::ofstream out(args.at(3));

		CosMetrics cos_metrics; // {}
		Analyzer analyzer{ cos_metrics };

		analyzer.LoadData(db);
		analyzer.LoadQueries(qrs);

		analyzer.PrintKNearestPointsFromDbToPoint(out, k);


	}
	catch (std::exception& e)
	{
		std::cout << e.what();
	}
	catch (...)
	{
		std::cout << "uknown fatal error";
	}
	return 0;
}
