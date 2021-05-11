#pragma once
#include <iterator>
#include <memory>
#include <utility>
#include "../nodes/singly.h"

#include <iostream>

template<class T, 
	typename Node_T = Node<T>, 
	typename Alloc = std::allocator<Node_T>>
class List {
public:

	using node_ptr = Node_T*;
	using alloc_t = std::allocator_traits<Alloc>;
	using alloc_void = typename alloc_t::const_void_pointer;

	List() : alloc{} {
		new_end();
		new_node(alloc, first, end, T{}, end);
	}

	List(std::initializer_list<T> lis) : alloc{} {
		new_end();

		auto last = end;
		node_ptr curr = nullptr;
		for (auto iter = std::rbegin(lis); iter != std::rend(lis); ++iter) {
			new_node(alloc, curr, last, *iter, last);
			last = curr;
		}

		first = curr;
	}

	auto& ahlie() {
		return first;
	}

	auto& mhm() {
		return end;
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
		alloc_single(end, alloc);
		construct_single(alloc, end);
	}

	template<typename... Args>
	void new_node(Alloc& allocator, node_ptr& ptr, alloc_void hint = alloc_void{}, const Args... args) {
		alloc_single(ptr, allocator, hint);
		construct_single(allocator, ptr, args...);
	}

	node_ptr end;
	node_ptr first;
	Alloc alloc;

};