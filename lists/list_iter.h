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
	using base = ConstListIterator<List_T>;

	ListIterator(node_ptr node_ptr) : ConstListIterator<List_T>{ node_ptr } {}

	data_t& operator*() {
		return const_cast<data_t&>(base::operator*());
	}

	ListIterator& operator++() {
		return static_cast<ListIterator&>(base::operator++());
	}
	ListIterator operator++(int) {
		return ++(*this);
	}

	bool operator==(const ListIterator rhs) const {
		return static_cast<base>(*this) == static_cast<base>(rhs);
	}
	bool operator!=(const ListIterator rhs) const {
		return static_cast<base>(*this) != static_cast<base>(rhs);
	}

};