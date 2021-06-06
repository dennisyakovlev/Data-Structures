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

// is a valid c++17+ iterator
template<typename Iter, typename = _std enable_if_t<_has_difference_type_v<Iter> &&
												    _has_value_type_v<Iter> &&
												    _has_pointer_v<Iter> &&
												    _has_reference_v<Iter> &&
												    _has_iterator_category_v<Iter>>>
struct _is_valid_iterator {};

// iterator tags
struct depth_iterator_tag {};

struct breadth_iterator_tag {};

// unwrap an interator to get raw pointer
// use to make sure all iterators inhert properly, have
// <_base> memeber declartion, and that the base
// iterator stores raw pointer as <elem>

// get base type of types
template<typename Iter, typename = void>
struct _get_base {
	using base_iter = Iter;
};

template<typename Iter>
struct _get_base<Iter, _std void_t<typename Iter::_base>> {
	using base_iter = typename _get_base<typename Iter::_base>::base_iter;
};

template<typename Iter> using _get_base_t = typename _get_base<Iter>::base_iter;

// get type of member variable
template<typename Iter, typename Member_Type>
constexpr Member_Type _get_memeber_type(Member_Type Iter::*) {}

template<typename Iter_Base>
struct _get_elem_type {
	using elem_type = decltype(_get_memeber_type(&Iter_Base::elem));
};

template<typename Iter> using _get_elem_type_t = typename _get_elem_type<Iter>::elem_type;

template<typename Iter, typename = void>
struct _has_elem : _std false_type {};

template<typename Iter>
struct _has_elem<Iter, _std void_t<decltype(&Iter::elem)>> : _std true_type {};

template<typename Iter> constexpr bool _has_elem_v = _has_elem<Iter>::value;

// enabled only if properly defined iterator type
// 1) iterator base stores variable to elements in container as "elem"
// 2) iterators inhert publicly from a base (does not have to be common base)
// 3) has the proper using members
template<typename Iter,
		 typename = _std enable_if_t<_has_elem_v<_get_base_t<Iter>>>,
		 typename = _std enable_if_t<_std is_constructible_v<_get_base_t<Iter>, Iter>>,
		 typename = _is_valid_iterator<Iter>>
	_get_elem_type_t<_get_base_t<Iter>> _unwrap_iter(Iter iter) {

	return (static_cast<_get_base_t<Iter>>(iter)).elem;

}

_end_dsa