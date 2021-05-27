#pragma once
#include <cstddef>
#include <memory>

#define STD std::

template<typename T>
class Allocator {
public:

	static_assert(!STD is_volatile_v<T>, "volatile type is ill-formed");
	static_assert(!STD is_const_v<T>, "const type is ill-formed");

	using value_type = T;
	using size_type = STD size_t;
	using difference_type = STD ptrdiff_t;

	Allocator() {}
	Allocator(const Allocator&) {}
	template<typename U>
	Allocator(const Allocator<U>&) {}
	~Allocator() = default;

	T* allocate(size_type n) {
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}
	void deallocate(T* ptr, size_type n) {
		::operator delete(ptr, n);
	}

};

template<typename T, typename Other>
bool operator== (const Allocator<T>& l, const Allocator<Other> r) {
	return true; // has no internal state, always equal
}