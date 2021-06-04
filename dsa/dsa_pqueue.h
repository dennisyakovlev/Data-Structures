#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>

#include <dsa_macros.h>
#include <dsa_memory.h>
#include <dsa_memory_traits.h>
#include <dsa_iterator_traits.h>

#define _left(ptr) first_v + ((2 * (ptr - first_v)) + 1);
#define _right(ptr) first_v + ((2 * (ptr - first_v)) + 2);
#define _valid_pointer(ptr) ptr <= last_v && ptr >= first_v
#define _alloc_pqueue(alloc, size) _alloc_traits::allocate(alloc, size)
#define _dealloc_pqueue(alloc, pointer, size) _alloc_traits::deconstruct(alloc, pointer, size)

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
		++(reinterpret_cast<base*>(this)->elem);
		return *this;
	}

	_iter_ref operator-- () {
		--(reinterpret_cast<base*>(this)->elem);
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

	using _iter = PriorityQueueBreadthIterator;
	using _iter_ref = _iter&;

	_iter_ref operator++ () {
		return static_cast<_iter_ref>(base::operator++());
	}

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

	static_assert(_std is_same_v<T, _std_alloc_traits<Alloc>::value_type>, 
		"allocator value type must be same as priorityqueue value type");

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
		first_v = reinterpret_cast<_pointer_type>(_alloc_pqueue(al, size));
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

	PriorityQueue(size_type sz, const value_type& val, const allocator_type& alloc) : al(alloc) {

		_create_size(sz, val);

	}
	
	template<typename Iter, typename = _is_valid_iterator<Iter>>
	PriorityQueue(Iter begin, Iter end) {

		_create_range(begin, end);
		_build_heap();

	}
	
	template<typename Iter, typename = _is_valid_iterator<Iter>>
	PriorityQueue(Iter begin, Iter end, const allocator_type& alloc) : al(alloc) {
		
		_create_range(begin, end);
		_build_heap();

	}
	
	PriorityQueue(_std initializer_list<value_type> lis) : al() {
		
		_create_range(lis.begin(), lis.end());
		_build_heap();
	
	}
	
	PriorityQueue(_std initializer_list<value_type> lis, const allocator_type& alloc) : al(alloc) {
		
		_create_range(lis.begin(), lis.end());
		_build_heap();

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

private:



public:

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

	size_type size() const {
		 
		return last_v - first_v;

	}

	value_type& operator[] (size_type i) {
		return first_v + i;
	}

private:

	void _float_down(iterator iter) {
		_pointer_type ptr = static_cast<_PriorityQueue_Iter_Base<PriorityQueue>>(iter).elem;
		auto l = _left(ptr);
		auto r = _right(ptr);
		_pointer_type largest = *l > *r ? l : r;
		// float down while current node is smaller than a child and not in final row
		for (; *largest > *ptr && (ptr - first_v <= (size() + 1) / 2);) {
			_std swap(*ptr, *largest);
			ptr = largest;
			l = _left(ptr);
			r = _right(ptr);
			largest = *l > *r ? l : r;
		}		
	}

	void _build_heap() {
		size_type sz = size_type((last_v - first_v) / 2);
		iterator iter = iterator(first_v + sz);
		iterator beg = begin();
		for (; iter != beg; --iter) {
			_float_down(iter);
		}
		_float_down(iter);
	}

	_pointer_type first_v;
	_pointer_type last_v;
	_pointer_type final_v;
	allocator_type al;

};

template<typename Iter>
PriorityQueue(Iter, Iter)->PriorityQueue<typename Iter::value_type>;

template<typename Iter, typename Alloc_New>
PriorityQueue(Iter, Iter, Alloc_New)->PriorityQueue<typename Iter::value_type, Alloc_New>;

template<typename sz_type, typename T_Same, typename Alloc_New>
PriorityQueue(sz_type, T_Same, Alloc_New)->PriorityQueue<T_Same, Alloc_New>;


#include <iostream>
#include <sstream>
#include <string>

template<typename T>
_std string::size_type _max_width(const T& val) {
	
	_std stringstream stream;
	stream << val;
	_std istringstream iss(stream.str());
	_std string line = "";
	_std string::size_type max_len = 0;
	while (_std getline(iss, line, '\n')) { // for windows only
		max_len = _std max(max_len, line.size());
	}
	return max_len;

}

template<typename T, typename Alloc>
_std ostream& operator<< (_std ostream& out, const PriorityQueue<T, Alloc> pq) {

	using PQueue = PriorityQueue<T, Alloc>;
	using size_type = typename PQueue::size_type;
	using const_iterator = typename PQueue::const_iterator;

	const_iterator curr = pq.cbegin();
	size_type num_level(_std log2(pq.size() + 1));
	double num_bottom_pairs = 0;

	for (size_type i = num_level; i != 0; --i) {
		
		// get number of spaces
		num_bottom_pairs =  _std pow(2, i) / 8; // divide by 8 to avoide negative i
		size_type num_spaces = (3 * num_bottom_pairs) + (num_bottom_pairs - 1);

		// add to accumulator <out>
		for (size_type j = 0; j != size_type(_std pow(2, num_level - i)); ++j) {
			out << _std string(num_spaces, ' ');
			out << *curr;
			out << _std string(num_spaces, ' ');
			out << " ";
			++curr;
		}

		out << '\n';

	}

	return out;

}

_end_dsa

