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

// rebind traits --------------------------------
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

template<typename Alloc>
struct _node_mem {

	using _alloc_traits = _std_alloc_traits<Alloc>;

	using value_type = typename _alloc_traits::value_type;
	using pointer = typename _alloc_traits::pointer;
	using allocator_type = typename _alloc_traits::allocator_type;

	static pointer _allocate(allocator_type& a) {
		return _alloc_traits::allocate(a, 1);
	}

	template<typename... init_args>
	static void _construct(allocator_type& a, pointer ptr, init_args&&... args) {
		_alloc_traits::construct(a, ptr, args...);
	}

	static void _destroy(allocator_type& a, pointer ptr) {
		_alloc_traits::destroy(a, ptr);
	}

	static void _deallocate(allocator_type& a, pointer ptr) {
		_alloc_traits::deallocate(a, ptr, 1);
	}
};

template<typename List, typename Alloc>
struct ConstListIterator {

	using iterator_category = _std forward_iterator_tag;
	using difference_type = _std ptrdiff_t;
	using value_type = typename List::value_type;
	using pointer = _pointer<_std_alloc_traits<Alloc>>;
	using reference = typename List::reference;
	
	using _alloc_void_pointer = typename _std_alloc_traits<Alloc>::void_pointer;
	using _node = _node_store<value_type, Alloc>;
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

template<typename List, typename Alloc>
struct ListIterator : ConstListIterator<List, Alloc> {

	using base = ConstListIterator<List, Alloc>;
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

template<typename List, typename Alloc>
bool operator!= (ConstListIterator<List, Alloc> l, ConstListIterator<List, Alloc> r) {
	return l.ptr != r.ptr;
}


template<typename T, typename Alloc = std::allocator<T>>
class List {

public:

	using value_type = T;
	using reference = value_type&;
	using allocator_type = Alloc;
	using const_reference = const value_type&;
	using iterator = ListIterator<List, allocator_type>;
	using const_iterator = ConstListIterator<List, allocator_type>;
	using size_type = _std size_t;

	using _alloc_traits = _std_alloc_traits<allocator_type>;
	using _alloc_void_pointer = typename _alloc_traits::void_pointer;
	using _node = _node_store<value_type, _alloc_void_pointer>;
	using _node_pointer = _pointer<_node>;
	using _alloc_node = _rebind_allocator<allocator_type, _node>;
	using _alloc_traits_node = _std_alloc_traits<_alloc_node>;

	using difference_type = typename const_iterator::difference_type; // should be after _node_pointer

	List(_std initializer_list<T> lis, allocator_type&) {
		auto iter = lis.begin();
		auto end = lis.end();

		auto curr = _alloc_traits_node::allocate(_al_node, 1);
		auto ahead = _alloc_traits_node::allocate(_al_node, 1);
		_alloc_traits_node::construct(_al_node, curr, *iter, ahead);
		++iter;

		_head = curr;

		for (; iter != end; ++iter) {
			curr = ahead;
			ahead = _alloc_traits_node::allocate(_al_node, 1);
			_alloc_traits_node::construct(_al_node, curr, *iter, ahead);
		}

		_tail = ahead;
	}
	~List() {
		auto temp = _head;
		for (; _head != _tail; _head = temp) {
			temp = _head->next_v;
			_alloc_traits_node::destroy(_al_node, _head);
			_alloc_traits_node::deallocate(_al_node, _head, 1);
		}

		_alloc_traits_node::deallocate(_al_node, _tail, 1);

	}

/*
private:

	void _init_head_tail() {
		tail = nullptr;
		head = nullptr;
	}

	template<typename Iter>
	void _create_from_range(allocator_type& al_data, _alloc_node& al_node, 
							Iter begin, Iter end, _node_pointer& into_start, _node_pointer& into_end) {
		auto curr = _alloc_single(al_node);
		auto ahead = _alloc_single(al_node);

		auto ptr = _alloc_single(al_data); // allocate for data type of list
		_construct_single(al_data, ptr, *begin); // construct data type of list
		++begin;

		_construct_single(al_node, curr, ptr, ahead); // construct curr

		into_start = curr;
		for (; begin != end; ++begin) {
			curr = ahead;
			ahead = _alloc_single(al_node);

			ptr = _alloc_single(al); // allocate for data type of list
			_construct_single(al_data, ptr, *begin); // construct data type of list

			_construct_single(al_node, curr, ptr, ahead);
		}
		into_end = ahead;

	}

	void _deallocate_all() {
		_node_pointer next = nullptr;
		while (head != tail) {
			next = head->next_v;

			_destroy_single(al, head->data_v);
			_deallocate_single(al, head->data_v);

			_destroy_single(al_node, head);
			_deallocate_single(al_node, head);

			head = next;
		}
		_destroy_single(al_node, tail);
		_deallocate_single(al_node, tail);
	}

public:

	List() {}
	List(allocator_type& a) {}
	List(_std initializer_list<T> lis) {
		_init_head_tail();
		_create_from_range(al, al_node, lis.begin(), lis.end(), head, tail);
	}
	List(_std initializer_list<T> lis, allocator_type& alloc) {
		_init_head_tail();
		_create_from_range(alloc, al_node, lis.begin(), lis.end(), head, tail);
	}
	~List() {
		_deallocate_all();
	}

	template<typename alloc_t>
	typename _node_mem<alloc_t>::pointer _alloc_single(alloc_t& a) {
		return _node_mem<alloc_t>::_allocate(a);
	}

	template<typename alloc_t, typename... init_args>
	void _construct_single(alloc_t& a, typename _node_mem<alloc_t>::pointer ptr, init_args&&... args) {
		_node_mem<alloc_t>::template _construct(a, ptr, args...);
	}

	template<typename alloc_t>
	void _destroy_single(alloc_t& a, typename _node_mem<alloc_t>::pointer ptr) {
		_node_mem<alloc_t>::_destroy(a, ptr);
	}

	template<typename alloc_t>
	void _deallocate_single(alloc_t& a, typename _node_mem<alloc_t>::pointer ptr) {
		_node_mem<alloc_t>::_deallocate(a, ptr);
	}

	_node_pointer head;
	_node_pointer tail;
	allocator_type al;
	_alloc_node al_node;
*/


	_node_pointer _head;
	_node_pointer _tail;
	_alloc_node _al_node;
};