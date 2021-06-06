#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>

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

	using _base = _PriorityQueue_Iter_Base<PriorityQueue>;

	using iterator_category = typename _base::iterator_category;
	using difference_type = typename _base::difference_type;
	using value_type = typename _base::value_type;
	using pointer = typename _base::pointer;
	using reference = typename _base::reference;
	using iterator_type = breadth_iterator_tag;

	using _iter = ConstPriorityQueueBreadthIterator;
	using _iter_ref = _iter&;

	using _base::_base;

	_iter_ref operator++ () {
		++(reinterpret_cast<_base*>(this)->elem);
		return *this;
	}

	_iter_ref operator-- () {
		--(reinterpret_cast<_base*>(this)->elem);
		return *this;
	}

	reference operator* () {
		return **(reinterpret_cast<_base*>(this));
	}

};

template<typename PriorityQueue>
struct PriorityQueueBreadthIterator : ConstPriorityQueueBreadthIterator<PriorityQueue> {

	using _base = ConstPriorityQueueBreadthIterator<PriorityQueue>;
	using iterator_category = typename _base::iterator_category;
	using difference_type = typename _base::difference_type;
	using value_type = typename _base::value_type;
	using pointer = typename _base::pointer;
	using reference = typename _base::reference;
	using iterator_type = typename _base::iterator_type;

	using _base::_base;

	using _iter = PriorityQueueBreadthIterator;
	using _iter_ref = _iter&;

	_iter_ref operator++ () {
		return static_cast<_iter_ref>(_base::operator++());
	}

	_iter_ref operator-- () {
		return static_cast<_iter_ref>(_base::operator--());
	}

};

template<typename PriorityQueue>
struct ConstPriorityQueueDepthIterator : _PriorityQueue_Iter_Base<PriorityQueue> {

	using _base = _PriorityQueue_Iter_Base<PriorityQueue>;
	using iterator_category = typename _base::iterator_category;
	using difference_type = typename _base::difference_type;
	using value_type = typename _base::value_type;
	using pointer = typename _base::pointer;
	using reference = typename _base::reference;
	using iterator_type = depth_iterator_tag;

	using _base::_base;

};

template<typename PriorityQueue>
struct PriorityQueueDepthIterator : ConstPriorityQueueDepthIterator<PriorityQueue> {

	using _base = ConstPriorityQueueDepthIterator<PriorityQueue>;
	using iterator_category = typename _base::iterator_category;
	using difference_type = typename _base::difference_type;
	using value_type = typename _base::value_type;
	using pointer = typename _base::pointer;
	using reference = typename _base::reference;
	using iterator_type = typename _base::iterator_type;

	using _base::_base;

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

	static_assert(_std is_same_v<T, typename _std_alloc_traits<Alloc>::value_type>, 
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
		first_v = _alloc_pqueue(al, size);
		last_v = first_v;
		final_v = first_v + size;
		_construct_range(begin, end);
	}

	void _create_size(size_type sz, const value_type& val) {
		auto size = _size_from_size(sz);
		first_v = _alloc_pqueue(al, size);
		last_v = first_v + sz;
		final_v = first_v + size;
		_construct_size(sz, first_v, val);
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

private:

	void _delete_all(_pointer_type first, _pointer_type last, _pointer_type final) {

		_pointer_type temp = first;
		for (; temp != last; ++temp) {
			_std destroy_at(temp);
		}
		size_type sz = final - first;
		_alloc_traits::deallocate(al, first, sz);

	}

public:

	~PriorityQueue() {

		_delete_all(first_v, last_v, final_v);

	}

	PriorityQueue& operator= (const PriorityQueue&);
	PriorityQueue& operator= (const PriorityQueue&&);
	PriorityQueue& operator= (const value_type&);
	
	iterator begin() {
		
		return iterator(first_v);
	
	}
	
	iterator begin() const {
		
		return iterator(first_v);
	
	}

	iterator_depth begin(depth_iterator_tag) {
		
		return iterator_depth(first_v);
	
	}
	
	iterator_depth begin(depth_iterator_tag) const {

		return iterator_depth(first_v);

	}

	const_iterator cbegin() const {
		
		return const_iterator(first_v);
	
	}
	
	const_iterator_depth cbegin(depth_iterator_tag) const {
		
		return const_iterator_depth(first_v);
	
	}

	iterator end() {
		
		return iterator(last_v);
	
	}

	iterator end() const {
		
		return iterator(last_v);
	
	}
	
	iterator_depth end(depth_iterator_tag) {
		
		return iterator_depth(last_v);
	
	}
	
	iterator_depth end(depth_iterator_tag) const {

		return iterator_depth(last_v);

	}

	const_iterator cend() const {
		
		return const_iterator(last_v);
	
	}
	
	const_iterator_depth cend(depth_iterator_tag) const {
		
		return const_iterator_depth(last_v);
	
	}

private:

	void _alloc_if_needed(size_type to_add) {

		// cast to size_type should be safe
		if (to_add > static_cast<size_type>(final_v - last_v)) {

			size_type num_new = _size_from_size(size() + to_add);

			_pointer_type new_first = _alloc_traits::allocate(al, num_new);
			_pointer_type old_first = first_v;
			_pointer_type old_last = last_v;
			_pointer_type old_final = final_v;

			first_v = new_first;
			last_v = first_v;
			final_v = first_v + num_new;

			_construct_range(old_first, old_last);
			
			_delete_all(old_first, old_last, old_final);

		}

	}



public:

	iterator insert(iterator iter, const value_type& val) {

		_alloc_if_needed(1);

		return iter;

	}
	//iterator_depth insert(iterator_depth, const value_type&);
	//iterator insert(iterator, const value_type&&);
	//iterator_depth insert(iterator_depth, const value_type&&);
	
	iterator insert(iterator iter, size_type sz, const value_type& val) {

		_alloc_if_needed(sz);
		_construct_size(sz, last_v, val);

		for (; sz != 0; --sz, ++last_v) {
			_float_up(iterator(last_v));
		}

		return iter;

	}

	iterator_depth insert(iterator_depth, size_type, const value_type&) {



	}
	//template<typename Iter> // enable if
	//iterator insert(iterator, Iter, Iter);
	//template<typename Iter> // enable if
	//iterator_depth insert(iterator_depth, Iter, Iter);
	//template<typename Val> // enable if
	//iterator insert(iterator, _std initializer_list<Val>);
	//template<typename Val> // enable if
	//iterator_depth insert(iterator_depth, _std initializer_list<Val>);

	size_type size() const {
		 
		return last_v - first_v;

	}

private:

	void _float_down(iterator iter) {

		_pointer_type ptr = _unwrap_iter(iter);
		auto l = _left(ptr);
		auto r = _right(ptr);
		_pointer_type largest = *l > *r ? l : r;
		const size_type begin_last = size_type(_std pow(2, _std floor(_std log2(size()))) - 1);

		// float down while current node is smaller than a child and not in final row
		for (;  *largest > *ptr && (ptr - first_v < begin_last);) {
			largest = *l > *r ? l : r; // possible dereference error if after l and r get assigned
			_std swap(*ptr, *largest);
			ptr = largest;
			l = _left(ptr);
			r = _right(ptr);
		}		

	}

	void _float_up(iterator iter) {

		_pointer_type ptr = _unwrap_iter(iter);
		_pointer_type parent = first_v + size_type(_std floor((ptr - first_v) / 2));

		for (; *parent < *ptr && ptr != first_v;) {
			_std swap(*ptr, *parent);
			ptr = parent;
			parent = first_v + size_type(_std floor((ptr - first_v) / 2));
		}

	}

	void _build_heap() {

		const size_type sz = (last_v - first_v) / 2;
		iterator iter = iterator(first_v + sz);
		const iterator end_iter = end();
		
		for (; iter != end_iter; ++iter) {
			_float_up(iter);
		}
		
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

// output operator and functions
template<typename T, typename Alloc>
auto _max_len(const PriorityQueue<T, Alloc>& pq) {

	using str_size = _std string::size_type;

	str_size max_len = 0;
	_std stringstream ss;
	for (auto& elem : pq) {
		ss << elem;
		max_len = _std max<str_size>(max_len, ss.str().size());
		ss.str(_std string());
	}

	return max_len;

}

template<typename T>
auto _elem_len(const T& elem) {

	using str_size = _std string::size_type;

	_std stringstream ss;
	ss << elem;
	return ss.str().size();

}

// Meant for visualization, not optimized or completely type safe
template<typename T, typename Alloc>
_std ostream& operator<< (_std ostream& out, const PriorityQueue<T, Alloc> pq) {
	
	using PQueue = PriorityQueue<T, Alloc>;
	using size_type = typename PQueue::size_type;
	using const_iterator = typename PQueue::const_iterator;

	auto max_size = _max_len(pq); // output relative to longest element in pqueue
	const_iterator curr = pq.cbegin();
	size_type num_level = static_cast<size_type>(_std log2(pq.size()) + 1); // safe to cast to smalelr double
																			// since log2 make pq.size() much smaller
	double num_bottom_pairs = 0; // kind of iffy since this is at most pq.size() / 4
								 // for large sizes errors will occur

	// for number of level
	for (size_type i = num_level; i != 0; --i) {
		
		// get number of spaces
		num_bottom_pairs =  _std pow(2, i) / 8; // divide by 8 to avoid negative i
		// not as safe to use double since num spaces
		size_type num_spaces = static_cast<size_type>((3 * num_bottom_pairs * max_size) + (max_size * (num_bottom_pairs - 1)));

		// add to accumulator <out> for every element in <pq>
		for (size_type j = 0; j != size_type(_std pow(2, num_level - i)); ++j) {
			out << _std string(num_spaces, ' ');
			if (curr != pq.end()) {
				out << _std string(max_size - _elem_len(*curr), '_');
				out << *curr;
				++curr;
			}
			else {
				out << _std string(max_size, '_');
			}
			out << _std string(num_spaces, ' ');
			out << _std string(max_size, ' ');
		}

		out << '\n';

	}

	return out;

}

_end_dsa