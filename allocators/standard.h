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

	Allocator();

	T* allocate(size_type);
	T* deallocate(T*, size_type);

};