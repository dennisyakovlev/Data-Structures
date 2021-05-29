#pragma once
#include <cstddef>
#include <memory>

#include <dsa_macros.h>

_begin_dsa

template<typename T>
class Allocator_Standard {
public:

	static_assert(!_std is_volatile_v<T>, "volatile type is ill-formed");
	static_assert(!_std is_const_v<T>, "const type is ill-formed");

	using value_type = T;
	using size_type = _std size_t;
	using difference_type = _std ptrdiff_t;

	Allocator_Standard() {}
	Allocator_Standard(const Allocator_Standard&) {}
	template<typename U>
	Allocator_Standard(const Allocator_Standard<U>&) {}
	~Allocator_Standard() = default;

	T* allocate(size_type n) {
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}
	void deallocate(T* ptr, size_type n) {
		::operator delete(ptr, n);
	}

};

template<typename T, typename Other>
bool operator== (const Allocator_Standard<T>& l, const Allocator_Standard<Other> r) {
	return true; // has no internal state, always equal
}

_end_dsa