#pragma once
#include <type_traits>

#include <dsa_macros.h>

_begin_dsa

// has "difference_type" member
template<typename type, typename = void>
struct _has_difference_type : _std false_type {};

template<typename type>
struct _has_difference_type<type, 
							_std void_t<typename type::difference_type>> 
	: _std true_type {};

template<typename type>
constexpr bool _has_difference_type_v = _has_difference_type<type>::value;

// has "value_type" member
template<typename type, typename = void>
struct _has_value_type : _std false_type {};

template<typename type>
struct _has_value_type<type, 
					   _std void_t<typename type::value_type>> 
	: _std true_type {};

template<typename type>
constexpr bool _has_value_type_v = _has_value_type<type>::value;

// has "pointer" member
template<typename type, typename = void>
struct _has_pointer : _std false_type {};

template<typename type>
struct _has_pointer<type, 
					_std void_t<typename type::pointer>> 
	: _std true_type {};

template<typename type>
constexpr bool _has_pointer_v = _has_pointer<type>::value;

// has "reference" member
template<typename type, typename = void>
struct _has_reference : _std false_type {};

template<typename type>
struct _has_reference<type, 
					  _std void_t<typename type::reference>> 
	: _std true_type {};

template<typename type>
constexpr bool _has_reference_v = _has_reference<type>::value;

// check for member "pointer" of type that is not guaranteed to have pointer
struct _none_pointer {}; // never use this type

template<typename type, typename = void>
struct _pointer_struct {
	static_assert(_std is_same_v<type, _none_pointer>, "<type> is missing \"pointer\" member");
};

template<typename type>
struct _pointer_struct<type, _std void_t<typename type::pointer>> {
	using _pointer_type = typename type::pointer;
};

template<typename type>
using _pointer = typename _pointer_struct<type>::_pointer_type;

_end_dsa