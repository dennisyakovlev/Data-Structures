#pragma once
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>

#include <dsa_macros.h>
#include <dsa_memory.h>
#include <dsa_memory_traits.h>
#include <dsa_iterator_traits.h>
#include <dsa_utils.h>

#define _alloc_pqueue(alloc, size) _alloc_traits::allocate(alloc, size)
#define _dealloc_pqueue(alloc, pointer, size) _alloc_traits::deconstruct(alloc, pointer, size)

#include <iostream>

_begin_dsa

template<typename PriorityQueue>
struct _PriorityQueue_Iter_Base {

	using _alloc_type = typename PriorityQueue::allocator_type;

	using iterator_category = _std bidirectional_iterator_tag;
	using difference_type = _std ptrdiff_t;
	using value_type = typename PriorityQueue::value_type;
	using reference = typename PriorityQueue::reference;
	using pointer = _pointer<_std_alloc_traits<_alloc_type>>;

	_PriorityQueue_Iter_Base(pointer ptr) : elem(ptr) {}

	reference operator* () {
		return *elem;
	}

	pointer elem;

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

	using _iter = ConstPriorityQueueBreadthIterator;
	using _iter_ref = _iter&;

	using base::base;

	_iter_ref operator++ () {
		++((*reinterpret_cast<base*>(this)).elem);
		return *this;
	}

	reference operator* () {
		return **(reinterpret_cast<base*>(this));
	}

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

	using base::base;

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

	using base::base;

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

	using base::base;

};

template<typename PriorityQueue>
bool operator!= (_PriorityQueue_Iter_Base<PriorityQueue> l, _PriorityQueue_Iter_Base<PriorityQueue> r) {
	return l.elem != r.elem;
}

template<typename PriorityQueue>
typename PriorityQueue::difference_type operator- (_PriorityQueue_Iter_Base<PriorityQueue> first, _PriorityQueue_Iter_Base<PriorityQueue> second) {
	return second.elem - first.elem;
}

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
	using _pointer_type = _pointer<_std_alloc_traits<_alloc_traits>>;

private:

	// allocate in terms of levels
	size_type _size_from_size(size_type sz) {
		return static_cast<size_type>(_std pow(2, _std ceil(_std log2(sz + 1))) - 1);
	}

	void _construct_size(size_type sz, _pointer_type into, const value_type& val) {
		for (; sz != 0; --sz, ++into) {
			_alloc_traits::construct(al, into, val);
		}
	}

	// assume first_v == last_v is true
	template<typename Iter>
	void _construct_range(Iter begin, Iter end) {
		for (; begin != end; ++begin, ++last_v) {
			_alloc_traits::construct(al, last_v, *begin);
		}
	}

	template<typename Iter>
	void _create_range(Iter begin, Iter end) {
		auto size = _size_from_size(_std distance(begin, end));
		first_v = _alloc_pqueue(al, size);
		last_v = first_v;
		final_v = first_v + size;
		_construct_range(begin, end);
	}

	void _create_size(size_type sz, const value_type& val) {
		auto size = _size_from_size(sz);
		first_v = _alloc_pqueue(al, size);
		last_v = first_v;
		final_v = first_v + size;
		_construct_size(sz, last_v, val);
	}

public:

	PriorityQueue() : al() {}
	PriorityQueue(const allocator_type& alloc) : al(alloc) {}
	//explicit PriorityQueue(SizeInit);
	//explicit PriorityQueue(SizeInit, const allocator_type&);
	
	PriorityQueue(const PriorityQueue& r) {
		_create_range(r.cbegin(), r.cend());
	}
	
	PriorityQueue(const PriorityQueue& r, const allocator_type& alloc) : al(alloc) {
		_create_range(r.cbegin(), r.cend());
	}

private:

	void _move(PriorityQueue& lis) {
		al = lis.al;
		first_v = lis.first_v;
		last_v = lis.last_v;
		final_v = lis.final_v;
		size_v = lis.size_v;
	}

public:

	PriorityQueue(PriorityQueue&& r) {
		_move(r);
	}
	
	PriorityQueue(PriorityQueue&& r, const allocator_type& alloc): al(alloc) {
		_move(r);
	}
	
	PriorityQueue(size_type sz, const value_type& val) {
		_create_size(sz, val);
	}

	// need 3 templates since partial deduction guide is not allowed
	template<typename sz_type, typename T2, typename Alloc_New,
		     typename = _std enable_if_t<_std is_constructible_v<size_type, sz_type>>>
	PriorityQueue(sz_type sz, const T2& val, const Alloc_New& alloc) : al(alloc) {
		_create_size(sz, val);
	}
	
	template<typename Iter, typename = _is_valid_iterator<Iter>>
	PriorityQueue(Iter begin, Iter end) {
		_create_range(begin, end);
	}
	
	template<typename Iter, typename Alloc_New, typename = _is_valid_iterator<Iter>>
	PriorityQueue(Iter begin, Iter end, Alloc_New alloc) : al(alloc) {
		_create_range(begin, end);
	}
	
	PriorityQueue(_std initializer_list<value_type> lis) : al() {
		_create_range(lis.begin(), lis.end());
	}
	
	PriorityQueue(_std initializer_list<value_type> lis, const allocator_type& alloc) : al(alloc) {
		_create_range(lis.begin(), lis.end());
	}

	PriorityQueue& operator= (const PriorityQueue&);
	PriorityQueue& operator= (const PriorityQueue&&);
	PriorityQueue& operator= (const value_type&);
	
	iterator begin() {
		return iterator(first_v);
	}
	
	iterator_depth begin(depth_iterator_tag) {
		return iterator_depth(first_v);
	}
	
	const_iterator cbegin() const {
		return const_iterator(first_v);
	}
	
	const_iterator_depth begin(depth_iterator_tag) const {
		return const_iterator_depth(first_v);
	}

	iterator end() {
		return iterator(last_v);
	}
	
	iterator_depth end(depth_iterator_tag) {
		return iterator_depth(last_v);
	}
	
	const_iterator cend() const {
		return const_iterator(last_v);
	}
	
	const_iterator_depth cend(depth_iterator_tag) const {
		return const_iterator_depth(last_v);
	}

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

//private:

	_pointer_type first_v;
	_pointer_type last_v;
	_pointer_type final_v;
	allocator_type al;
	size_type size_v;

};

template<typename Iter>
PriorityQueue(Iter, Iter)->PriorityQueue<typename Iter::value_type>;

template<typename Iter, typename Alloc_New>
PriorityQueue(Iter, Iter, Alloc_New)->PriorityQueue<typename Iter::value_type, Alloc_New>;

template<typename sz_type, typename T_Same, typename Alloc_New>
PriorityQueue(sz_type, T_Same, Alloc_New)->PriorityQueue<T_Same, Alloc_New>;

_end_dsa

