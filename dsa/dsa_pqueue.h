#pragma once
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>

#include <dsa_macros.h>
#include <dsa_memory.h>
#include <dsa_memory_traits.h>
#include <dsa_iterator_traits.h>
#include <dsa_utils.h>

_begin_dsa

template<typename PriorityQueue>
struct _PriorityQueue_Iter_Base {

	using _alloc_type = typename PriorityQueue::allocator_type;

	using iterator_category = _std bidirectional_iterator_tag;
	using difference_type = _std ptrdiff_t;
	using value_type = typename PriorityQueue::value_type;
	using reference = typename PriorityQueue::reference;
	using pointer = _pointer<_std_alloc_traits<_alloc_type>>;

};

template<typename PriorityQueue>
struct ConstPriorityQueueBreadthIterator : _PriorityQueue_Iter_Base<PriorityQueue> {

	using base = _PriorityQueue_Iter_Base<PriorityQueue>;
	using iterator_category = typename base::iterator_category;
	using difference_type = typename base::difference_type;
	using value_type = typename base::value_type;
	using pointer = typename base::pointer;
	using reference = typename base::reference;
	using iterator_type = breadth_iterator_tag;


};

template<typename PriorityQueue>
struct PriorityQueueBreadthIterator : ConstPriorityQueueBreadthIterator<PriorityQueue> {

	using base = ConstPriorityQueueBreadthIterator<PriorityQueue>;
	using iterator_category = typename base::iterator_category;
	using difference_type = typename base::difference_type;
	using value_type = typename base::value_type;
	using pointer = typename base::pointer;
	using reference = typename base::reference;
	using iterator_type = typename base::iterator_type;

};

template<typename PriorityQueue>
struct ConstPriorityQueueDepthIterator : _PriorityQueue_Iter_Base<PriorityQueue> {

	using base = _PriorityQueue_Iter_Base<PriorityQueue>;
	using iterator_category = typename base::iterator_category;
	using difference_type = typename base::difference_type;
	using value_type = typename base::value_type;
	using pointer = typename base::pointer;
	using reference = typename base::reference;
	using iterator_type = depth_iterator_tag;

};

template<typename PriorityQueue>
struct PriorityQueueDepthIterator : ConstPriorityQueueDepthIterator<PriorityQueue> {

	using base = ConstPriorityQueueDepthIterator<PriorityQueue>;
	using iterator_category = typename base::iterator_category;
	using difference_type = typename base::difference_type;
	using value_type = typename base::value_type;
	using pointer = typename base::pointer;
	using reference = typename base::reference;
	using iterator_type = typename base::iterator_type;

};

template<typename T, typename Alloc = _std allocator<T>>
class PriorityQueue {

public:

	using allocator_type = Alloc;
	using value_type = T;
	using reference = value_type&;
	using conse_reference = const reference;
	using iterator = PriorityQueueBreadthIterator<PriorityQueue>;
	using const_iterator = ConstPriorityQueueBreadthIterator<PriorityQueue>;
	using iterator_depth = PriorityQueueDepthIterator<PriorityQueue>;
	using const_iterator_depth = ConstPriorityQueueDepthIterator<PriorityQueue>;
    // using reverse_iterator = XXX
    // using const_reverse_iterator = XXX
	using size_type = _std size_t;

	using _alloc_traits = _std_alloc_traits<allocator_type>;

	using difference_type = typename _PriorityQueue_Iter_Base<PriorityQueue>::difference_type;

	PriorityQueue() {}
	PriorityQueue(const allocator_type&);
	explicit PriorityQueue(SizeInit);
	explicit PriorityQueue(SizeInit, const allocator_type&);
	PriorityQueue(const PriorityQueue&);
	PriorityQueue(const PriorityQueue&, const allocator_type&);
	PriorityQueue(const PriorityQueue&&);
	PriorityQueue(const PriorityQueue&&, const allocator_type&);
	PriorityQueue(size_type, const value_type&);
	PriorityQueue(size_type, const value_type&, const allocator_type&);
	template<typename Iter> // enable if
	PriorityQueue(Iter, Iter);
	template<typename Iter> // enable if
	PriorityQueue(Iter, Iter, const allocator_type&);
	PriorityQueue(_std initializer_list<value_type>);
	PriorityQueue(_std initializer_list<value_type>, const allocator_type&);

	PriorityQueue& operator= (const PriorityQueue&);
	PriorityQueue& operator= (const PriorityQueue&&);
	PriorityQueue& operator= (const value_type&);
	
	iterator insert(iterator, const value_type&);
	iterator_depth insert(iterator_depth, const value_type&);
	iterator insert(iterator, const value_type&&);
	iterator_depth insert(iterator_depth, const value_type&&);
	iterator insert(iterator, size_type, const value_type&);
	iterator_depth insert(iterator_depth, size_type, const value_type&);
	template<typename Iter> // enable if
	iterator insert(iterator, Iter, Iter);
	template<typename Iter> // enable if
	iterator_depth insert(iterator_depth, Iter, Iter);
	template<typename Val> // enable if
	iterator insert(iterator, _std initializer_list<Val>);
	template<typename Val> // enable if
	iterator_depth insert(iterator_depth, _std initializer_list<Val>);


};

_end_dsa

