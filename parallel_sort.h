#include "utility.h"

#ifndef PARALLEL_SORT_STL_H
#define PARALLEL_SORT_STL_H

namespace internal
{
	std::size_t g_depth = 0L;

	// The lower cutoff boundary
	const std::size_t cutoff_low = 100;
	// The higher cutoff boundary
	const std::size_t cutoff_high = 1000;

	template<class BidirIt, class _Pred>
	void sort3v(BidirIt _First, BidirIt _Mid, BidirIt _Last, _Pred compare)
	{
		// If the last data item is less than the first, exchange these both items
		if (compare(*_Last, *_First))
			std::iter_swap(_First, _Last);
		// If the middle data item is less than the first, exchange these both items
		if (compare(*_Mid, *_First))
			std::iter_swap(_First, _Mid);
		// If the last data item is less than the middle, exchange these both items
		if (compare(*_Last, *_Mid))
			std::iter_swap(_Mid, _Last);
	}

	template<class BidirIt>
	BidirIt med3v(BidirIt _First, BidirIt _Mid, BidirIt _Last)
	{
		if ((*_First <= *_Mid && *_First >= *_Last) ||
			(*_First <= *_Last && *_First >= *_Mid))
			// Return the first value if it is less than or equal to the middle value
			return _First;
		else if ((*_Mid <= *_First && *_Mid >= *_Last) ||
			(*_Mid <= *_Last && *_Mid >= *_First))
			// Return the middle value if it is less than or equal to the first value
			return _Mid;
		else if ((*_Last <= *_First && *_Last >= *_Mid) ||
			(*_Last <= *_Mid && *_Last >= *_First))
			// Return the last value if it is less than or equal to the first value
			return _Last;

		return _First;
	}

	template<class BidirIt>
	BidirIt med9v(BidirIt _First, BidirIt _Mid, BidirIt _Last)
	{
		std::size_t _Size = 0L;
		std::vector<BidirIt> median; median.resize(9);

		// Compute the size of each fragment of the array
		if ((_Size = (std::distance(_First, _Last) / 9)) >= 3)
		{
			//#pragma  ivdep
			//#pragma  vector always
			// Iterate through the array and copy each item
			// located at the specific position to the temporary array
			for (int index = 0; index < 9; index++)
			     median[index] = _First + _Size * index;

			// Find the median of the first fragment
			BidirIt _Med1 = med3v(median[0], median[1], median[2]);
			// Find the median of the second fragment
			BidirIt _Med2 = med3v(median[3], median[4], median[5]);
			// Find the median of the third fragment
			BidirIt _Med3 = med3v(median[6], median[7], median[8]);

			// Compute the final value of the median-of-nine
			return med3v(_Med1, _Med2, _Med3);
		}

		else {
			return med3v(_First, _Mid, _Last);
		}
	}

	template<class BidirIt, class _Pred>
	void adjacent_sort(BidirIt _First, BidirIt _Last, _Pred compare)
	{
		// Iterate through the array of items in parallel
		#pragma omp parallel for
		for (auto _FwdIt = _First; _FwdIt != _Last - 1; _FwdIt++)
		{
			// For each item perform a check if the following item
			// is greater than the next adjacent item. If so, exchange these items
			if (!compare(*_FwdIt, *(_FwdIt + 1)) && *_FwdIt != *(_FwdIt + 1))
				std::iter_swap(_FwdIt, _FwdIt + 1);
		}
	}

	template<class BidirIt, class _Pred>
	void shaker_sort(BidirIt _First, BidirIt _Last, _Pred compare)
	{
		BidirIt _LeftIt = _First, _RightIt = _Last, \
			_MidIt = _LeftIt + (_RightIt - _LeftIt) / 2;

		// Do the cocktail shaker sort by exchanging the data items from left and right
		while (!compare(*_LeftIt, *_RightIt) && _RightIt > _MidIt)
			std::iter_swap(_LeftIt++, _RightIt--);
	}

	template<class BidirIt, class _Pred>
	void do_insertion(BidirIt _First, BidirIt _RevIt, BidirIt _Last, _Pred compare)
	{
		typename std::iterator_traits<BidirIt>::\
			value_type _Value = *(_RevIt + 1);

		// Iterate through the array from the current 
		// position downto the position of the first data item
	    // For each data item in the subset of previous data items
		// perform a check if it's greater or equal to the specific 
		// value of argument
		while (compare(_Value, *_RevIt))
		{
			// If so, do the insertion of the 
			// value of argument to the specific position
			*(_RevIt + 1) = *_RevIt; *_RevIt = _Value;
			if (_RevIt <= _First) break; _RevIt--;
		}
	}

	template<class RandomIt, class _Pred>
	void insertion_sort(RandomIt _First, RandomIt _Last, _Pred compare)
	{
		RandomIt _LeftIt = _First + 1, _RightIt = _Last;

		// Perform the insertion sort by iterating through the array
		while (_LeftIt <= _RightIt)
		{
			// For each data item make the number of calls 
			// to the function that performs the actual sorting
			do_insertion(_First, _LeftIt - 1, _RightIt, compare); _LeftIt++;
			do_insertion(_First, _LeftIt - 1, _RightIt, compare); _LeftIt++;
			do_insertion(_First, _LeftIt - 1, _RightIt, compare); _LeftIt++;
			do_insertion(_First, _LeftIt - 1, _RightIt, compare); _LeftIt++;
		}
	}

	template<class RanIt, class _Pred>
        void _qs3w(RanIt _First, RanIt _Last, _Pred compare)
	{
		// Check if the array size is not zero
		if (_First >= _Last) return;
		
		// Perform a check if the size of the array is equal to 1
		if (std::distance(_First, _Last) == 1)
		{
			// If so, check if the value of the first item is greater
			// than the value of the last item. If so, exchange these both items
			if (*_First > *_Last)
				std::iter_swap(_First, _Last);

			// Terminate the process of sorting
			return;
		}

		// Compute the size of the array to be sorted
		std::size_t _Size = 0L; g_depth++;
      		if ((_Size = std::distance(_First, _Last)) > 0)
		{
			// Compute the middle of the array to be sorted
			RanIt _LeftIt = _First, _RightIt = _Last, 
				_MidIt = _First + (_Last - _First) / 2;

			bool is_swapped_left = false, is_swapped_right = false;
			// Compute the value of median by using median-of-nine algorithm
			RanIt _Median = internal::med9v(_LeftIt, _MidIt, _RightIt);
			// Obtain the value of pivot equal to the value of median
			typename std::iterator_traits<RanIt>::value_type _Pivot = *_Median;

			// Iterate through the array to be sorted and for each item
			// perform a check if it's less than the value of pivot
			for (RanIt _FwdIt = _LeftIt; _FwdIt <= _RightIt; _FwdIt++)
			{
				// Check if the value of the item is less than the value of pivot
				if (compare(*_FwdIt, _Pivot))
				{
					// If so, exchange the current data item with the next item from left
					is_swapped_left = true;
					std::iter_swap(_FwdIt, _LeftIt);
					// Increment the value of pointer to the succeeding data item from left
					_LeftIt++;
				}

				// Check if the value of the item is greater than the value of pivot
				if (compare(_Pivot, *_FwdIt))
				{
					is_swapped_right = true;
					// If so, exchange the current data item with the next item from right
					std::iter_swap(_FwdIt, _RightIt);
					// Decrement the value of pointer to the succeeding data item from right
					// and the pointer to the current data item in the array
					_RightIt--; _FwdIt--;
				}
			}

			// Perform a check if the size is greater than 
			// the value of the higher cutting off boundary
			if (_Size >= internal::cutoff_high)
			{
				// If so, launch parallel tasks to sort 
				// the either leftmost or rightmost part of the array
				#pragma omp task untied mergeable
				if (std::distance(_First, _LeftIt) > 0 && is_swapped_left)
					internal::_qs3w(_First, _LeftIt, compare);

				#pragma omp task untied mergeable
				if (std::distance(_RightIt, _Last) > 0 && is_swapped_right)
					internal::_qs3w(_RightIt, _Last, compare);
			}

			else
			{
				// Otherwise, merge the two concurrent parallel tasks into a single parallel task
				// in which the leftmost and rightmost parts are sorted sequentially
				#pragma omp parallel num_threads(12)
				#pragma omp single nowait
				{
					if (std::distance(_First, _LeftIt) > 0 && is_swapped_left)
						internal::_qs3w(_First, _LeftIt, compare);
					if (std::distance(_RightIt, _Last) > 0 && is_swapped_right)
						internal::_qs3w(_RightIt, _Last, compare);
				}
			}
		}
	}

	template<class BidirIt, class _Pred >
	inline std::pair<BidirIt, BidirIt> partition(BidirIt _First, \
		BidirIt _Last, _Pred compare)
	{
		std::size_t _Size = 0L;
		// Compute the actual size of the array. If the size is equal to 1
		// then perform a tiny sort by exchanging the adjacent data items
		if ((_Size = std::distance(_First, _Last)) == 1)
		{
			if (!compare(*_First, *_Last))
				std::iter_swap(_First, _Last);

			// Return the pair of pointers to the two subsequent
			// partitions in the array to be sorted
			return std::make_pair(_First, _Last);
		}

		if (_Size == 2)
		{
			// If the array size is equal to 2, perform a tiny sort by sorting three values.
			internal::sort3v(_First, _First + 1, _Last, compare);
			// Return the pair of pointers to the two subsequent
			// partitions in the array to be sorted
			return std::make_pair(_First, _Last);
		}

		// Compute the middle of the array to be sorted
		BidirIt _LeftIt = _First, _RightIt = _Last, \
			_MidIt = _LeftIt + _Size / 2;

		// Perform a tiny sort of three values 
		// at the beginning, middle and the end of the array
		internal::sort3v(_LeftIt, _MidIt, _RightIt, compare);
		// Perform a tiny sort of three values at the middle of the array
		internal::sort3v(_MidIt - 1, _MidIt, _MidIt + 1, compare);

		// Compute the median by using median-of-nine algorithm
		BidirIt _Median = internal::med9v(_LeftIt, _MidIt, _RightIt);
		// Obtain the value of pivot based on the value of median
		typename std::iterator_traits<BidirIt>::value_type _Pivot = *_Median;

		// Perform a check if the first data item is equal to the value of median
		if (*_First == _Pivot)
			// If so, swap it with the middle data item
			std::iter_swap(_First, _MidIt);

		// Perform a check if the last data item is equal to the value of median
		if (*_Last == _Pivot)
			// If so, swap it with the middle data item
			std::iter_swap(_Last, _MidIt);

		// Perform partitioning iteratively until we've re-arranged the array to be sorted
		// and obtained the pointers to the leftmost and rightmost partitions.
		while (_LeftIt <= _RightIt)
		{
			// Iterate through the array from left and for each data item
			// perform a check if it's greater than the value of pivot
			for (; _LeftIt <= _Last; _LeftIt++)
			{
				// Perform a check if the value of the current 
				// data item is greater than the value of pivot or is equal to it 
				if (compare(_Pivot, *_LeftIt) || _Pivot == *_LeftIt)
				{
					// If so, perform the iteration through the array from the right
					// until we've reached the data item which is already found from left
					// For each item perform a check if it's not less than the value of pivot
					for (; _RightIt >= _LeftIt; _RightIt--)
					{
						// If the current item's value is less than the value of pivot or 
						// at least equal to it, exchange the specific data item being found
						if (compare(*_RightIt, _Pivot) || _Pivot == *_RightIt)
						{
							// Perform a check if the value of the pointer obtained from left
							// is less than or equal to the value of pointer from the right
							if (_LeftIt <= _RightIt)
							{
								// If so, exchange the specific data items
								std::iter_swap(_LeftIt, _RightIt);
								// Increment the pointer to the left item and 
								// decrement the pointer to the right item
								_LeftIt++; _RightIt--;
							}

							// Terminate the loop execution
							break;
						}
					}

					// Terminate the loop execution
					break;
				}
			}
		}

		// Return the pair of pointers to the two subsequent
		// partitions in the array to be sorted
		return std::make_pair(_LeftIt, _RightIt);
	}

	template<class BidirIt, class _Pred >
	void intro_sort(BidirIt _First, BidirIt _Last, _Pred compare)
	{
		// Check if the array size is not zero
		if (_First >= _Last) return;

		std::size_t pos = 0L; g_depth++;
		// Perform a check if the array has already been sorted.
		// If so terminate the process of sorting
		if (misc::sorted(_First, _Last, pos, compare)) return;

		std::size_t _Size = 0L;
		// Compute the actual size of the array and perform a check
		// if the size does not exceed a lower cutting off boundary
		if ((_Size = std::distance(_First, _Last)) > internal::cutoff_low)
		{
			BidirIt _LeftIt = _First, _RightIt = _Last;

			// If so, partition the array by using Hoare's quicksort partitioning
			std::pair<BidirIt, BidirIt> p \
				= internal::partition(_First, _Last, compare);

			// Perform a check if the size of the array does not 
			// exceed the higher cutting off boundary
			if (_Size > internal::cutoff_high)
			{
				// If not, launch the first parallel task 
				// that will perform the improved 3-way quicksort at the backend
				#pragma omp task untied mergeable
				// Perform a check if the size of the partition is not zero and
				// we're not performing an empty null-task
				if (std::distance(p.first, _Last) > 0)
					internal::_qs3w(p.first, _Last, compare);

				// If not, launch the second parallel task 
				// that will perform the improved 3-way quicksort at the backend
				#pragma omp task untied mergeable
				// Perform a check if the size of the partition is not zero and
				// we're not performing an empty null-task
				if (std::distance(_First, p.second) > 0)
					internal::_qs3w(_First, p.second, compare);
			}

			else
			{
				// Otherwise, merge the two concurrent parallel tasks into a single parallel task
				// in which calls to the sorter routine are performed sequentially
				#pragma omp parallel num_threads(12)
				#pragma omp single nowait
				{
					if (std::distance(p.first, _Last) > 0)
						internal::_qs3w(p.first, _Last, compare);
					if (std::distance(_First, p.second) > 0)
						internal::_qs3w(_First, p.second, compare);
				}
			}
		}

		else
		{
			// If the size of the array is less than 10^2, 
			// perform a regular insertion sort launched 
			// during the parallel task execution
			#pragma omp parallel num_threads(12)
			#pragma omp single nowait
			{
				if (std::distance(_First, _Last) > 0)
					internal::insertion_sort(_First, _Last + 1, compare);
			}
		}
	}

	template<class BidirIt, class _Pred >
	void parallel_sort1(BidirIt _First, BidirIt _Last, _Pred compare)
	{
		// Compute the potential size of the array to be sorted
		std::size_t _Size = std::distance(_First, _Last);
		// Find the value of the maximum data item in the array
		typename std::iterator_traits<BidirIt>::value_type \
			_MaxValue = *std::max_element(_First, _Last);

		// Compute the radix MSD position for the size value
		std::size_t _MaxSizeRadix  = std::log10((double)_Size);
		// Compute the radix MSD position for the maximum value
		std::size_t _MaxValueRadix = std::log10((double)_MaxValue);

		// Perform a check if the MSD position of the size is greater
		// (e.g. the array is an interleave sequence)
		if (_MaxSizeRadix > _MaxValueRadix)
		{
			// Partition the entire array into chunks of a fixed size
			// based on finding sub-intervals for each particular chunk
			misc::partitioner p(std::make_pair(0, _Size), \
				omp_get_max_threads());

			// Execute a parallel region in which the introspective sorter
			// function is invoked for each particular chunks to be sorted
			#pragma omp parallel num_threads(12)
			{
				volatile int tid = omp_get_thread_num();
				internal::intro_sort(_First + p[tid].first(),
					_First + p[tid].second(), compare);
			}

			// Perform a parallel task to perform a final sort that
			// will arrange the entire array into an ordered sequence
			#pragma omp parallel num_threads(12)
			#pragma omp master
				internal::intro_sort(_First, _Last - 1, compare);
		}
		
		else
		{
			// Otherwise, launch a parallel task to perform 
			// an introspective sort of the entire array
			#pragma omp parallel num_threads(12)
			#pragma omp master
				internal::intro_sort(_First, _Last - 1, compare);
		}
	}

	template<class BidirIt, class _Pred >
	void parallel_sort(BidirIt _First, BidirIt _Last, _Pred compare)
	{
		std::size_t pos = 0L; g_depth = 0L;
		// Compute the potential size of the array to be sorted
		std::size_t _Size = std::distance(_First, _Last);

		// Perform the parallel cocktail shaker sort
		#pragma omp task untied mergeable
			internal::shaker_sort(_First, _Last - 1, compare);

		// Synchronize threads until the parallel task has completed its execution
		#pragma omp taskwait
		
		// Let's give a first chance check if the array has already been sorted and
		// obtain the position of the first unsorted data item
		if (!misc::sorted(_First, _Last, pos, compare))
		{
			BidirIt _LeftIt = _First, _RightIt = _First + pos;

			// Compute the radix MSD position for the size value
			std::size_t _MaxSizeRadix  = std::log10((double)_Size);
			// Compute the radix MSD position for the maximum value
			std::size_t _MaxValueRadix = std::log10((double)*_RightIt);

			// Perform a check if the MSD position of the size is greater
			if (_MaxSizeRadix > _MaxValueRadix)
			{
				// If so, perform a check if the value of the first data item is
				// equal to the value of the first unsorted item that occurs in the array
				// (e.g. the array is an interleave sequence)
				if (*_LeftIt == *(_RightIt + 1))
				{
					// Perform the parallel task that executes 
					// the 3-way quicksort routine at the backend
					#pragma omp parallel num_threads(12)
					#pragma omp master
						internal::_qs3w(_First, _Last - 1, compare);
				
					// Terminate the process of sorting.
					return;
				}
			}

			// Otherwise, if the array is not an interleave sequence
			// execute a loop to sort the array by performing the introspective sort
			do
			{
				// Perform the pre-sorting of the array by using adjacent sort
				internal::adjacent_sort(_First, _Last, compare);
				// Perform the actual sorting by launching the introspective sort
				internal::parallel_sort1(_First, _Last, compare);
			  // Perform the last chance check if the array has already been sorted
			  // If not, proceed with the process of sorting over again until the entire array is sorted
			} while (!misc::sorted(_First, _Last, pos, compare));
		}
	}
}

#endif // PARALLEL_SORT_STL_H
