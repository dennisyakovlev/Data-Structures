#pragma once
#include <iterator>
#include <memory>
#include <utility>
#include "../nodes/singly.h"
#include "list_iter.h"

template<class T, 
	typename Node_T = Node<T>, 
	typename Alloc = std::allocator<Node_T>>
class List {
public:

	using data_t = T;
	using node_ptr = Node_T*;
	using alloc_t = std::allocator_traits<Alloc>;
	using alloc_void = typename alloc_t::const_void_pointer;
	using iterator = ListIterator<List>;
	using const_iterator = ConstListIterator<List>;

	List() : alloc{} {
		new_end();
		new_node(alloc, first, end_v, T{}, end_v);
	}
	List(std::initializer_list<T> lis) : alloc{} {
		first = construct_range(alloc, lis.begin(), lis.end());
	}
	template<typename Iter>
	List(Iter start, Iter ending) : alloc{} {
		first = construct_range(alloc, start, ending);
	}

	iterator begin() {
		return ListIterator<List>(first);
	}
	const_iterator begin() const {
		return ConstListIterator<List>(first);
	}
	iterator end() {
		return ListIterator<List>(end_v);
	}
	const_iterator end() const {
		return ConstListIterator<List>(end_v);
	}

private:

	void alloc_single(node_ptr& ptr, Alloc& allocator, alloc_void hint = alloc_void{}) {
		ptr = alloc_t::allocate(allocator, 1, hint);
	}

	template<typename... Args>
	void construct_single(Alloc& allocator, node_ptr& ptr, const Args... args) {
		alloc_t::construct(allocator, ptr, args...);
	}

	template<typename Iter>
	node_ptr construct_range(Alloc& allocator, Iter start, Iter ending) {
		node_ptr n = nullptr;
		if (start == ending) {
			new_end();
			return end_v;
		}

		auto data = *start;
		auto next = construct_range(allocator, ++start, ending);
		new_node(alloc, n, next, data, next);
		return n;
	}

	void new_end() {
		alloc_single(end_v, alloc);
		construct_single(alloc, end_v);
	}

	template<typename... Args>
	void new_node(Alloc& allocator, node_ptr& ptr, alloc_void hint = alloc_void{}, const Args... args) {
		alloc_single(ptr, allocator, hint);
		construct_single(allocator, ptr, args...);
	}

	node_ptr end_v;
	node_ptr first;
	Alloc alloc;

};