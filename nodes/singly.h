#pragma once
#include <utility>

// logically a node should not be able to modify another node

// Note: careful with constructor - removing the const on the
// pointer. Should not change what <v_next> points to or will
// get undefined behaviour.

template<typename T>
struct Node {

	using data_t = T;                      

	Node() : v_data{}, v_next { nullptr } {}
	Node(const data_t data, const Node* const next) : v_data{ std::move(data) }, 
												v_next{ const_cast<const Node*>(next) } {}

	bool is_empty() const {
		return v_next == nullptr;
	}

	const Node* const& next() const {
		return v_next;
	}
	const Node*& next() {
		return const_cast<const Node*&>(static_cast<const Node&>(*this).next());
	}

	const data_t& data() const {
		return v_data;
	}
	data_t& data() {
		return const_cast<data_t&>(static_cast<const Node&>(*this).data());
	}

	void set_next(const Node* const in) {
		v_next = const_cast<const Node*>(in);
	}

	void set_data(const data_t& data) {
		v_data = data;
	}

private:

	data_t v_data;
	const Node* v_next;
};