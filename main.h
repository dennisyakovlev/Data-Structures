#pragma once
#include <cstddef>
#include <cstring>
#include <iterator>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

#define _std std::

#include <iostream>
#define print(x) std::cout << x << std::endl;

// traits ---------------------------------------
template<typename from, typename to>
using _rebind_pointer = typename _std pointer_traits<from>::template rebind<to>;

template<typename from, typename to>
using _rebind_allocator = typename _std allocator_traits<from>::template rebind_alloc<to>;

template<typename alloc>
using _std_alloc_traits = _std allocator_traits<alloc>;
//-----------------------------------------------

// check for member "pointer" of type not -------
// required to have pointer
struct _none_pointer {};

template<typename type, typename = void>
struct _pointer_struct {
	static_assert(_std is_same_v<type, _none_pointer>, "<type> is missing \"pointer\" member");
};

template<typename type>
struct _pointer_struct<type, _std void_t<typename type::pointer>> {
	using _pointer_type = typename type::pointer;
};

template<typename type>
using _pointer = typename _pointer_struct<type>::_pointer_type;
//-----------------------------------------------

template<typename type, typename void_pointer>
struct _node_store {

	using value_type = type;
	using pointer = _rebind_pointer<void_pointer, _node_store>;

	_node_store(value_type val) : data_v{ val }, next_v{ nullptr } {}
	_node_store(value_type val, pointer ptr) : data_v{ val }, next_v{ ptr } {}
	~_node_store() = default;
	
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
	//pointer operator-> ();

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
	//pointer operator-> ();

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

private: // functions for constructors

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

		_List_copy_construct(lis.cbegin(), lis.cend());

	}
	List(const List& lis, const allocator_type&) {

		_List_copy_construct(lis.cbegin(), lis.cend());

	}
	List(_std initializer_list<T> lis) {

		_create_head_to_tail(lis.begin(), lis.end());

	}
	List(_std initializer_list<T> lis, const allocator_type&) {

		_create_head_to_tail(lis.begin(), lis.end());

	}
	~List() {

		_deallocate_head_to_tail();

	}

	void assign(iterator, iterator);
	void assign(_std initializer_list<T>);
	void assign(size_type, const value_type&);
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
	void clear();
	template<typename... Args>
	iterator emplace(const_iterator, Args&&...);
	bool empty();
	iterator erease(const_iterator);
	iterator erease(const_iterator, const_iterator);
	allocator_type get_allocator();
	iterator insert(const_iterator, const value_type&);
	iterator insert(const_iterator, value_type&&);
	iterator insert(const_iterator, size_type, const value_type&);
	template<typename Iter>
	void insert(const_iterator pos, Iter start, Iter end);
	iterator insert(const_iterator, _std initializer_list<T>);
	size_type max_size();
	size_type size();
	void swap(List);


	_node_pointer _head;
	_node_pointer _tail;
	_alloc_node _al_node;
};