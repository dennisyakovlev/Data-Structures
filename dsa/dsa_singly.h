#pragma once
#include <assert.h>
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <functional>
#include <iterator>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

#include <dsa_macros.h>
#include <dsa_iterator_traits.h>
#include <dsa_memory_traits.h>
#include <dsa_type_traits.h>

_begin_dsa

template<typename type, typename void_pointer>
struct _node_store {

	using value_type = type;
	using pointer = _rebind_pointer<void_pointer, _node_store>;

	_node_store(value_type val) : data_v{ val }, next_v{ nullptr } {}

	_node_store(value_type val, pointer ptr) : data_v{ val }, next_v{ ptr } {}
	
	value_type data_v;
	pointer next_v;

};

template<typename List>
struct ConstListIterator {

	using _list_alloc = typename List::allocator_type;

	using iterator_category = _std forward_iterator_tag;
	using difference_type = _std ptrdiff_t;
	using value_type = typename List::value_type;
	using pointer = _pointer<_std_alloc_traits<_list_alloc>>;
	using reference = typename List::reference;
	
	using _alloc_void_pointer = typename _std_alloc_traits<_list_alloc>::void_pointer;
	using _node = _node_store<value_type, _alloc_void_pointer>;
	using _next = _rebind_pointer<_alloc_void_pointer, _node>;

	ConstListIterator() : ptr{ nullptr } {}
	ConstListIterator(_next node_adr) : ptr{ node_adr } {}

	reference operator* () const {

		return ptr->data_v;

	}

	ConstListIterator& operator++ () {

		ptr = ptr->next_v;
		return *this;

	}

	ConstListIterator operator++ (int) {

		ConstListIterator temp = *this;
		++(*this);
		return temp;

	}

	pointer operator-> () {

		return _std addressof(ptr->data_v);

	}

	_next ptr;
	
};

template<typename List>
struct ListIterator : ConstListIterator<List> {

	using base = ConstListIterator<List>;
	using iterator_category = typename base::iterator_category;
	using difference_type = typename base::difference_type;
	using value_type = typename base::value_type;
	using pointer = typename base::pointer;
	using reference = typename base::reference;

	using base::base;

	reference operator* () {

		return const_cast<reference>(base::operator*());

	}

	ListIterator& operator++ () {

		return static_cast<ListIterator&>(base::operator++());

	}

	ListIterator operator++ (int) {

		ListIterator temp = *this;
		base::operator++ ();
		return temp;

	}

	pointer operator-> () {

		return base::operator->();

	}

};

template<typename List>
bool operator!= (ConstListIterator<List> l, ConstListIterator<List> r) {

	return l.ptr != r.ptr;

}

template<typename T, typename Alloc = std::allocator<T>>
class List {

public:

	using value_type = T;
	using reference = value_type&;
	using allocator_type = Alloc;
	using const_reference = const value_type&;
	using const_iterator = ConstListIterator<List>;
	using iterator = ListIterator<List>;
	using size_type = _std size_t;

	using _alloc_traits = _std_alloc_traits<allocator_type>;
	using _alloc_void_pointer = typename _alloc_traits::void_pointer;
	using _node = _node_store<value_type, _alloc_void_pointer>;
	using _node_pointer = _pointer<_node>;
	using _alloc_node = _rebind_allocator<allocator_type, _node>;
	using _alloc_traits_node = _std_alloc_traits<_alloc_node>;
	using _alloc_node_pointer = _pointer<_std_alloc_traits<_alloc_traits_node>>;

	using difference_type = typename const_iterator::difference_type; // should be after _node_pointer

private:

	template<typename Iter>
	void _create_head_to_tail(Iter begin, Iter end) {

		auto curr = _alloc_traits_node::allocate(_al_node, 1);
		auto ahead = _alloc_traits_node::allocate(_al_node, 1); // to set next value for curr
		_alloc_traits_node::construct(_al_node, curr, *begin, ahead);
		++begin;

		_head = curr;

		for (; begin != end; ++begin) {
			curr = ahead;
			ahead = _alloc_traits_node::allocate(_al_node, 1);
			_alloc_traits_node::construct(_al_node, curr, *begin, ahead);
		}

		_tail = ahead;

	}

	void _deallocate_head_to_tail() {

		auto temp = _head;
		for (; _head != _tail; _head = temp) {
			temp = _head->next_v;
			_alloc_traits_node::destroy(_al_node, _head);
			_alloc_traits_node::deallocate(_al_node, _head, 1);
		}

		_alloc_traits_node::deallocate(_al_node, _tail, 1); // _tail not constructed

	}

public:

	List() {}
	
	List(const allocator_type&) {}

private:

	void _List_copy_construct(const_iterator begin, const_iterator end) {

		_deallocate_head_to_tail();
		_create_head_to_tail(begin, end);

	}

public:

	List(const List& lis) {

		if ((this != _std addressof(lis)) && !lis.empty()) {
			_List_copy_construct(lis.cbegin(), lis.cend());
		}
		else {
			_head = nullptr;
			_tail = nullptr;
		}

	}
	
	List(const List& lis, const allocator_type&) {

		if ((this != _std addressof(lis)) && !lis.empty()) {
			_List_copy_construct(lis.cbegin(), lis.cend());
		}
		else {
			_head = nullptr;
			_tail = nullptr;
		}

	}

private:

	template<typename Iter>
	void _List_range_construct(Iter begin, Iter end) {

		_create_head_to_tail(begin, end);

	}

public:

	List(_std initializer_list<T> lis) {

		_List_range_construct(lis.begin(), lis.end());

	}
	
	List(_std initializer_list<T> lis, const allocator_type&) {

		_List_range_construct(lis.begin(), lis.end());

	}
	
	template<typename Iter, typename = _is_valid_iterator<Iter>>
	List(Iter start, Iter end) {

		_List_range_construct(start, end);

	}
	
	template<typename Iter, typename Alloc_New, typename = _is_valid_iterator<Iter>>
	List(Iter start, Iter end, const Alloc_New&) {

		_List_range_construct(start, end);

	}

private:

	void _List_move(List& lis) {
		_head = lis._head;
		_tail = lis._tail;
	}

public:

	List(List&& lis) {

		_List_move(lis);

	}
	
	List(List&& lis, const allocator_type&) {

		_List_move(lis);

	}

private:

	void _create_head_to_tail_val(const value_type& val, size_type size) {

		auto curr = _alloc_traits_node::allocate(_al_node, 1);
		auto ahead = _alloc_traits_node::allocate(_al_node, 1); // to set next value for curr
		_alloc_traits_node::construct(_al_node, curr, val, ahead);
		--size;

		_head = curr;

		for (; size != 0; --size) {
			curr = ahead;
			ahead = _alloc_traits_node::allocate(_al_node, 1);
			_alloc_traits_node::construct(_al_node, curr, val, ahead);
		}

		_tail = ahead;

	}

public:

	explicit List(size_type size) {

		_create_head_to_tail_val(value_type{}, size);

	}
	
	explicit List(size_type size, const allocator_type&) {

		_create_head_to_tail_val(value_type{}, size);

	}
	
	explicit List(const value_type& val, size_type size) {

		_create_head_to_tail_val(val, size);

	}
	
	explicit List(const value_type& val, size_type size, const allocator_type&) {

		_create_head_to_tail_val(val, size);
		
	}

	~List() {

		_deallocate_head_to_tail();

	}

	template<typename Iter, typename = _is_valid_iterator<Iter>>
	void assign(Iter begin, Iter end) {

		_deallocate_head_to_tail();
		_List_range_construct(begin, end);

	}

	void assign(_std initializer_list<T> lis) {

		_deallocate_head_to_tail();
		_List_range_construct(lis.begin(), lis.end());

	}

	void assign(const value_type& val, size_type size) {

		_deallocate_head_to_tail();
		_create_head_to_tail(val, size);

	}
	
	iterator begin() {
		return iterator(_head);
	}
	
	const_iterator begin() const {
		return const_iterator{ _head };
	}
	
	const_iterator cbegin() const {
		return const_iterator{ _head };
	}
	
	iterator end() {
		return iterator(_tail);
	}
	
	const_iterator end() const {
		return const_iterator(_tail);
	}
	
	const_iterator cend() const {
		return const_iterator(_tail);
	}
	
	void clear() {

		_deallocate_head_to_tail();

	}

	template<typename... Args>
	iterator emplace(const_iterator into, Args&&... args) {

		auto node = _alloc_traits_node::allocate(_al_node, 1);
		_alloc_traits_node::construct(_al_node, node, value_type(_std forward<Args>(args)...), into.ptr->next_v);

		into.ptr->next_v = node;

		return iterator{ node };

	}
	
	bool empty() const {

		return _head == _tail;

	}
	
private:

	iterator _deallocate_range(_node_pointer begin, _node_pointer end) {

		auto original_start = begin;
		begin = begin->next_v;

		auto temp = begin;
		for (; begin != end; begin = temp) {
			temp = begin->next_v;
			_alloc_traits_node::destroy(_al_node, begin);
			_alloc_traits_node::deallocate(_al_node, begin, 1);
		}

		original_start->next_v = end;

		return iterator{ end };
	}

public:

	iterator erease(const_iterator iter) { // ereases element after iter

		return _deallocate_range(iter.ptr, iter.ptr->next_v->next_v);

	}
	
	iterator erease(const_iterator start, const_iterator end) { // ereases range (start, end)

		return _deallocate_range(start.ptr, end.ptr);

	}

	allocator_type get_allocator() {

		return _al_node;

	}

private:

	template<typename Iter>
	iterator _create_into_range_safe(const_iterator into, Iter begin, Iter end) {

		assert(!(this->empty() && !(begin != end)) && "cant insert into end iterator");
		
		iterator iter;
		if (begin != end) {
			if (this->empty()) {
				_create_head_to_tail(begin, end);
				iter = iterator{ _head };
			}
			else {
				iter = _create_into_range(into, begin, end);
			}

		}

		return iter;
		
	}

	template<typename Iter>
	iterator _create_into_range(const_iterator into, Iter begin, Iter end) {

		auto curr = _alloc_traits_node::allocate(_al_node, 1);
		auto ahead = _alloc_traits_node::allocate(_al_node, 1); // to set next value for curr
		_alloc_traits_node::construct(_al_node, curr, *begin, ahead);
		++begin;

		auto original_start = curr;

		for (; begin != end; ++begin) {
			curr = ahead;
			ahead = _alloc_traits_node::allocate(_al_node, 1);
			_alloc_traits_node::construct(_al_node, curr, *begin, ahead);
		}

		auto temp = into.ptr->next_v;
		into.ptr->next_v = original_start;
		curr->next_v = temp;

		return iterator{original_start};
	}

	iterator _create_into_range_val_safe(const_iterator into, const value_type& val, size_type size) {

		iterator iter;
		if (this->empty()) {
			_create_head_to_tail_val(val, size);
			iter = iterator{ _head };
		}
		else {
			iter = _create_into_range_val(into, val, size);
		}
		
		return iter;

	}

	iterator _create_into_range_val(const_iterator into, const value_type& val, size_type size) {

		auto curr = _alloc_traits_node::allocate(_al_node, 1);
		auto ahead = _alloc_traits_node::allocate(_al_node, 1); // to set next value for curr
		_alloc_traits_node::construct(_al_node, curr, val, ahead);
		--size;

		auto original_start = curr;

		for (; size != 0; --size) {
			curr = ahead;
			ahead = _alloc_traits_node::allocate(_al_node, 1);
			_alloc_traits_node::construct(_al_node, curr, val, ahead);
		}

		auto temp = into.ptr->next_v;
		into.ptr->next_v = original_start;
		curr->next_v = temp;

		return iterator{ original_start };
	}

public:

	iterator insert(const_iterator into, const value_type& val) { // inserts after into

#ifdef INSERT_CHECK
		return _create_into_range_val_safe(into, val, 1);
#else
		return _create_into_range_val(into, val, 1);
#endif 
	}

	iterator insert(const_iterator into, const value_type& val, size_type size) { // inserts after into

#ifdef INSERT_CHECK
		return _create_into_range_val_safe(into, val, size);
#else
		return _create_into_range_val(into, val, size);
#endif 
	}

	template<typename Iter, typename = _is_valid_iterator<Iter>>
	iterator insert(const_iterator into, Iter start, Iter end) { // inserts after into

#ifdef INSERT_CHECK
		return _create_into_range_safe(into, start, end);
#else
		return _create_into_range(into, start, end);
#endif 
	}
	
	iterator insert(const_iterator into, _std initializer_list<T> lis) { // inserts after into

#ifdef INSERT_CHECK
		return _create_into_range_safe(into, lis.begin(), lis.end());
#else
		return _create_into_range(into, lis.begin(), lis.end());
#endif 
	}

	size_type size() const {
		
		size_type sz = 0;
		auto temp = this->cbegin();
		for (; temp != this->cend(); ++temp) {
			++sz;
		}

		return sz;

	}
	
	template<typename Alloc_New>
	void swap(List<value_type, Alloc_New>& other) {

		_std swap(_head, other._head);
		_std swap(_tail, other._tail);

	}

	template<typename, typename>
	friend class List;

private:

	_node_pointer _head;
	_node_pointer _tail;
	_alloc_node _al_node;

};

template<typename Iter>
List(Iter, Iter)->List<typename Iter::value_type>;

template<typename Iter, typename Alloc_New>
List(Iter, Iter, Alloc_New)->List<typename Iter::value_type, Alloc_New>;

template<typename Type, typename Alloc1, typename Alloc2>
bool _compare_equals(const List<Type, Alloc1>& l, const List<Type, Alloc2>& r, _std function<bool(Type, Type)> func) {

	auto left_iter = l.begin();
	auto right_iter = r.begin();

	for (; (left_iter != l.end()) && (right_iter != r.end()); ++left_iter, ++right_iter) {
		if (!func(*left_iter, *right_iter)) {
			return false;
		}
	}

	if ((left_iter != l.end()) || (right_iter != r.end())) {
		return false;
	}

	return true;

}

template<typename Type, typename Alloc1, typename Alloc2>
bool operator== (const List<Type, Alloc1>& l, const List<Type, Alloc2>& r) {

	return _compare_equals(l, r,
		_std function(
			[](Type left, Type right) -> bool { 
				return left == right; 
			}
		)
	);

}

template<typename Type, typename Alloc1, typename Alloc2>
bool operator!= (const List<Type, Alloc1>& l, const List<Type, Alloc2>& r) {

	return !(l == r);

}

template<typename Type, typename Alloc1, typename Alloc2>
bool _compare_inequality(const List<Type, Alloc1>& l, const List<Type, Alloc2>& r, _std function<bool(Type, Type)> func) {

	auto left_iter = l.begin();
	auto right_iter = r.begin();

	for (; (left_iter != l.end()) && (right_iter != r.end()); ++left_iter, ++right_iter) {
		if (!func(*left_iter, *right_iter)) {
			return false;
		}
	}

	return true;

}

template<typename Type, typename Alloc1, typename Alloc2>
bool operator<= (const List<Type, Alloc1>& l, const List<Type, Alloc2>& r) {

	return _compare_inequality(l, r,
		_std function(
			[](Type left, Type right) -> bool { 
				return left <= right; 
			}
		)
	);

}

template<typename Type, typename Alloc1, typename Alloc2>
bool operator>= (const List<Type, Alloc1>& l, const List<Type, Alloc2>& r) {

	return _compare_inequality(l, r,
		_std function(
			[](Type left, Type right) -> bool { 
				return left >= right;
			}
		)
	);

}

template<typename Type, typename Alloc1, typename Alloc2>
bool operator< (const List<Type, Alloc1>& l, const List<Type, Alloc2>& r) {

	return _compare_inequality(l, r,
		_std function(
			[](Type left, Type right) -> bool {
				return left < right;
			}
		)
	);

}

template<typename Type, typename Alloc1, typename Alloc2>
bool operator> (const List<Type, Alloc1>& l, const List<Type, Alloc2>& r) {

	return _compare_inequality(l, r,
		_std function(
			[](Type left, Type right) -> bool {
				return left > right;
			}
		)
	);	

}

_end_dsa