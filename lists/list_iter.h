#pragma once

template<typename List_T>
class ListIterator {
public:

	using node_ptr = typename List_T::node_ptr;
	using data_t = typename List_T::data_t;

	ListIterator(node_ptr node_ptr) : ptr{ node_ptr } {}

	data_t& operator*() {
		return ptr->data();
	}

	ListIterator& operator++() {
		ptr = ptr->next();
		return *this;
	}
	ListIterator operator++(int) {
		return ++(*this);
	}

	bool operator==(const ListIterator rhs) {
		return ptr == rhs.ptr;
	}
	bool operator!=(const ListIterator rhs) {
		return ptr != rhs.ptr;
	}

	node_ptr ptr;

};