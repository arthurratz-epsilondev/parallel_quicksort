#include "generators.h"

#ifndef UTILITY_STL_H
#define UTILITY_STL_H

namespace misc
{
	const std::size_t minval_radix = 7;
	const std::size_t maxval_radix = 8;

	struct partitioner
	{
		typedef std::pair<std::size_t, std::size_t> partition_range;

	public:
		explicit partitioner(const partition_range& range, const std::size_t count)
			: _left(std::size_t(0)), position(std::size_t(0)), _range(range), partitions(count)
		{
			length = static_cast<std::size_t>(
				std::ceil(this->distance() / (double)this->count_base()));
		}

		partitioner(const partitioner& p)
			: _left(p.left_base()), _range(p.range_base()), partitions(p.count_base()) { }

	public:
		const partitioner& operator[](std::size_t index) {
			position = index; return *this;
		}

	public:
		std::size_t size() const {
			return static_cast<std::size_t>(
				std::ceil(this->distance() / (double)this->size_base()));
		}

		std::size_t partition_size() const {
			return this->size_base();
		}

		std::size_t first() const {
			std::size_t pos = this->position_base() * this->size_base();
			return (pos <= _range.second) ? pos : _range.second;
		}
		std::size_t second() const {
			std::size_t pos = (this->position_base() + 1) * this->size_base();
			return (pos <= _range.second) ? pos : _range.second;
		}

	public:
		std::size_t distance() const {
			return this->range_base().second - this->range_base().first + 1;
		}

	public:
		std::size_t left_base() const { return _left; }
		partition_range range_base() const { return _range; }
		std::size_t count_base() const { return partitions; }
		std::size_t size_base() const { return length; }
		std::size_t position_base() const { return position; }

	protected:
		struct
		{
			std::size_t _left;
			partition_range _range;
			std::size_t partitions;
			std::size_t length;
			std::size_t position;
		};
	};

	void init(std::vector<std::int64_t>& a, \
			std::pair<std::size_t, std::size_t> range, std::size_t& count, int sort_type)
	{
		// Use STL Mersenne-Twister random values generator
		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(range.first, range.second);

		if (a.size() <= 0) {
			if (count == 0)
				count = dist(gen);

			a.clear(); a.resize(count);
		}

		// Select the intial order of the array to be sorted
		if (sort_type == 0)
			gen::generate_random_sequence(a, count);
		else if (sort_type == 1)
			gen::generate_lots_of_duplicates(a, count);
		else if (sort_type == 2)
			gen::generate_ascending_sequence(a, count);
		else if (sort_type == 3)
			gen::generate_descending_sequence(a, count);
		else if (sort_type == 4)
			gen::generate_interleave_sequence(a, count);

	}

	std::string sorttype2string(int sort_type)
	{
		std::string s_type = "";
		if (sort_type == 0)
			s_type = "randomized sequence";
		else if (sort_type == 1)
			s_type = "lots of duplicates";
		else if (sort_type == 2)
			s_type ="ascending sequence";
		else if (sort_type == 3)
			s_type = "descending sequence";
		else if (sort_type == 4)
			s_type = "interleave sequence";

		return s_type;
	}

	template<class BidirIt, class _Pred>
	std::size_t sorted(BidirIt _First, BidirIt _Last, \
		std::size_t& position, _Pred compare)
	{
		bool is_sorted = true;

		// Iterate through the array and perform a check if its sorted
		for (auto _FwdIt = _First; _FwdIt != _Last - 1 && is_sorted; _FwdIt++)
		{
			if (!compare(*_FwdIt, *(_FwdIt + 1)) && *_FwdIt != *(_FwdIt + 1))
			{
				if (is_sorted == true)
					position = std::distance(_First, _FwdIt);

				is_sorted = false;
			}
		}

		return is_sorted;
	}

	template<class RandomIt>
	void print_out(RandomIt _First, RandomIt _Second)
	{
		// Print out the array
		for (auto it = _First; it != _Second; it++)
			std::cout << *it << " ";

		std::cout << "\n";
	}
}

#endif // UTILITY_STL_H
