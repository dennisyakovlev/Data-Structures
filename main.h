#pragma once
#include <cstddef>
#include <cstring>
#include <iterator>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

#define _std std::

template<typename T>
struct _node_store {

	using value_type = T;
	using pointer = value_type*;
	using node = _node_store;
	using _node_pointer = node*;

	_node_store(pointer ptr) : data_v{ ptr }, next_v{ nullptr } {}
	_node_store(pointer ptr, _node_pointer nxt) : data_v{ ptr }, next_v{ nxt } {}
	~_node_store() = default;
	
	pointer data_v;
	_node_pointer next_v;
};

template<typename Alloc>
struct _node_mem {

	using _alloc_traits = std::allocator_traits<Alloc>;

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

template<typename List>
struct ConstListIterator {

	using iterator_category = _std forward_iterator_tag;
	using difference_type = _std ptrdiff_t;
	using value_type = typename List::value_type;
	using pointer = typename _std allocator_traits<typename List::allocator_type>::pointer;
	using reference = typename List::reference;
	
	using _next = typename List::_node_pointer;

	
	ConstListIterator() : ptr{ nullptr } {}
	ConstListIterator(_next node_adr) : ptr{ node_adr } {}

	reference operator* () const {
		return *(ptr->data_v);
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
	using reference = T&;
	using const_reference = const T&;
	using iterator = ListIterator<List>;
	using const_iterator = ConstListIterator<List>;
	using size_type = _std size_t;
	using allocator_type = Alloc;

	using _node_pointer = _node_store<T>*;
	using _alloc_traits = std::allocator_traits<allocator_type>;
	using _alloc_node = typename _alloc_traits::template rebind_alloc<_node_store<T>>;

	using difference_type = typename ConstListIterator<List>::difference_type; // should be after _node_pointer

private:

	template<typename Iter>
	void _create_from_range(Iter begin, Iter end) {
		auto curr = _alloc_single(al_node);
		auto ahead = _alloc_single(al_node);

		auto ptr = _alloc_single(al); // allocate for data type of list
		_construct_single(al, ptr, *begin); // construct data type of list
		++begin;

		_construct_single(al_node, curr, ptr, ahead); // construct curr

		head = curr;
		for (; begin != end; ++begin) {
			curr = ahead;
			ahead = _alloc_single(al_node);

			ptr = _alloc_single(al); // allocate for data type of list
			_construct_single(al, ptr, *begin); // construct data type of list

			_construct_single(al_node, curr, ptr, ahead);
		}
		tail = ahead;
	}

public:

	List() {}
	List(allocator_type& a) {}
	List(_std initializer_list<T> lis) : head{ nullptr }, tail{ nullptr } {
		_create_from_range(lis.begin(), lis.end());
	}
	~List() {
		
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

};
