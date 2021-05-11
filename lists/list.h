#pragma once
#include <iterator>
#include <memory>
#include <utility>
#include "../nodes/singly.h"
#include "list_iter.h"

#include <iostream>

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

	List() : alloc{} {
		new_end();
		new_node(alloc, first, end_v, T{}, end_v);
	}

	List(std::initializer_list<T> lis) : alloc{} {
		new_end();

		auto last = end_v;
		node_ptr curr = nullptr;
		for (auto iter = std::rbegin(lis); iter != std::rend(lis); ++iter) {
			new_node(alloc, curr, last, *iter, last);
			last = curr;
		}

		first = curr;
	}

	auto begin() {
		return ListIterator<List>(first);
	}
	auto end() {
		return ListIterator<List>(end_v);
	}

	auto& ahlie() {
		return first;
	}

	auto& mhm() {
		return end_v;
	}

private:

	void alloc_single(node_ptr& ptr, Alloc& allocator, alloc_void hint = alloc_void{}) {
		ptr = alloc_t::allocate(allocator, 1, hint);
	}

	template<typename... Args>
	void construct_single(Alloc& allocator, node_ptr& ptr, const Args... args) {
		alloc_t::construct(allocator, ptr, args...);
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