#pragma once

#include <dsa_macros.h>

_begin_dsa

template<typename Integer_Integral_Type>
Integer_Integral_Type exp_unsigned_int(Integer_Integral_Type base, Integer_Integral_Type exp) {

	Integer_Integral_Type res = 1;
	while (exp != 0) {
		if (exp % 2 == 1) {
			res = base * res;
		}
		exp /= 2;
		base *= base;

	}

	return res;
}

_end_dsa