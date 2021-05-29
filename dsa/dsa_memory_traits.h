#pragma once
#include <memory>

#include <dsa_macros.h>

_begin_dsa

template<typename from, typename to>
using _rebind_pointer = typename _std pointer_traits<from>::template rebind<to>;

template<typename from, typename to>
using _rebind_allocator = typename _std allocator_traits<from>::template rebind_alloc<to>;

template<typename alloc>
using _std_alloc_traits = _std allocator_traits<alloc>;

_end_dsa