#ifndef GENERATORS_STL_H
#define GENERATORS_STL_H

namespace gen
{
	template<class Container = std::vector<std::int64_t>>
	void generate_lots_of_duplicates(Container& a, std::size_t size)
	{
		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(1, 100);

		#pragma omp parallel for
		for (std::size_t index = 0; index < size; index++)
			a[index] = dist(gen);
	}

	template<class Container = std::vector<std::int64_t>>
	void generate_binary_sequence(Container& a, std::size_t size)
	{
		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 1);

		#pragma omp parallel for
		for (std::size_t index = 0; index < size; index++)
			a[index] = dist(gen);
	}

	template<class Container = std::vector<std::int64_t>>
	void generate_random_sequence(Container& a, std::size_t size)
	{
		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(1, size);

		#pragma omp parallel for
		for (std::size_t index = 0; index < size; index++)
			a[index] = dist(gen);
	}

	template<class Container = std::vector<std::int64_t>>
	void generate_ascending_sequence(Container& a, std::size_t size)
	{
		#pragma omp parallel for
		for (std::size_t index = 0; index < size; index++)
			a[index] = size - index - 1;
	}

	template<class Container = std::vector<std::int64_t>>
	void generate_descending_sequence(Container& a, std::size_t size)
	{
		#pragma omp parallel for
		for (std::size_t index = 0; index < size; index++)
			a[index] = index;
	}

	template<class Container = std::vector<std::int64_t>>
	void generate_interleave_sequence(Container& a, std::size_t size)
	{
		std::size_t n = 0;
		for (std::size_t index = 0; index < size / 5; index++)
			for (std::size_t nindex = 0; nindex < 5; nindex++)
				a[n++] = nindex;
	}

	template<class Container = std::vector<std::int64_t>>
	void generate_single_value(Container& a, std::size_t size)
	{
		#pragma omp parallel for
		for (std::size_t index = 0; index < size; index++)
			a[index] = 1;
	}
}

#endif // GENERATORS_STL_H
