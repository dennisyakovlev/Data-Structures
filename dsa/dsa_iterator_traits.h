#pragma once
#include <type_traits>

#include <dsa_macros.h>
#include <dsa_type_traits.h>

_begin_dsa

template<typename type, typename = void>
struct _has_iterator_category : _std false_type {};

template<typename type>
struct _has_iterator_category<type, 
							 _std void_t<typename type::iterator_category>> 
	: _std true_type {};

template<typename type>
constexpr bool _has_iterator_category_v = _has_iterator_category<type>::value;

template<typename Iter, typename = _std enable_if_t<_has_difference_type_v<Iter> &&
												    _has_value_type_v<Iter> &&
												    _has_pointer_v<Iter> &&
												    _has_reference_v<Iter> &&
												    _has_iterator_category_v<Iter>>>
struct _is_valid_iterator {};

_end_dsa