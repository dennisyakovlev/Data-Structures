#pragma once
#include <algorithm>
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

template<typename type, typename = void>
struct _has_difference_type : _std false_type {};

template<typename type>
struct _has_difference_type<type, 
							_std void_t<typename type::difference_type>> 
	: _std true_type {};

template<typename type>
constexpr bool _has_difference_type_v = _has_difference_type<type>::value;

template<typename type, typename = void>
struct _has_value_type : _std false_type {};

template<typename type>
struct _has_value_type<type, 
					   _std void_t<typename type::value_type>> 
	: _std true_type {};

template<typename type>
constexpr bool _has_value_type_v = _has_value_type<type>::value;

template<typename type, typename = void>
struct _has_pointer : _std false_type {};

template<typename type>
struct _has_pointer<type, 
					_std void_t<typename type::pointer>> 
	: _std true_type {};

template<typename type>
constexpr bool _has_pointer_v = _has_pointer<type>::value;

template<typename type, typename = void>
struct _has_reference : _std false_type {};

template<typename type>
struct _has_reference<type, 
					  _std void_t<typename type::reference>> 
	: _std true_type {};

template<typename type>
constexpr bool _has_reference_v = _has_reference<type>::value;

template<typename type, typename = void>
struct _has_iterator_category : _std false_type {};

template<typename type>
struct _has_iterator_category<type, 
							 _std void_t<typename type::iterator_category>> 
	: _std true_type {};

template<typename type>
constexpr bool _has_iterator_category_v = _has_iterator_category<type>::value;

template<typename Iter, typename = _std enable_if_t<_has_difference_type_v<Iter> &&
												    _has_value_type_v<Iter> &&
												    _has_pointer_v<Iter> &&
												    _has_reference_v<Iter> &&
												    _has_iterator_category_v<Iter>>>
struct _is_valid_iterator {};

// check for member "pointer" of type that is
// not guaranteed to have pointer
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

		_List_copy_construct(lis.cbegin(), lis.cend());

	}
	
	List(const List& lis, const allocator_type&) {

		_List_copy_construct(lis.cbegin(), lis.cend());

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

	void _create_head_to_tail(const value_type& val, size_type size) {

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

		_create_head_to_tail(value_type{}, size);

	}
	
	explicit List(size_type size, const allocator_type&) {

		_create_head_to_tail(value_type{}, size);

	}
	
	explicit List(const value_type& val, size_type size) {

		_create_head_to_tail(val, size);

	}
	
	explicit List(const value_type& val, size_type size, const allocator_type&) {

		_create_head_to_tail(val, size);
		
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