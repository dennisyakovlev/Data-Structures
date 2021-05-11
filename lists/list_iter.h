#pragma once

template<typename List_T>
class ConstListIterator {
public:

	using node_ptr = typename List_T::node_ptr;
	using data_t = typename List_T::data_t;

	ConstListIterator(node_ptr node_ptr) : ptr{ node_ptr } {}

	const data_t& operator*() const {
		return ptr->data();
	}

	ConstListIterator& operator++() {
		ptr = ptr->next();
		return *this;
	}
	ConstListIterator operator++(int) {
		return ++(*this);
	}

	bool operator==(const ConstListIterator rhs) const {
		return ptr == rhs.ptr;
	}
	bool operator!=(const ConstListIterator rhs) const {
		return ptr != rhs.ptr;
	}

	node_ptr ptr;

};

template<typename List_T>
class ListIterator : public ConstListIterator<List_T> {
public:

	using node_ptr = typename List_T::node_ptr;
	using data_t = typename List_T::data_t;

	ListIterator(node_ptr node_ptr) : ConstListIterator<List_T>{ node_ptr } {}

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