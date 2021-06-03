#pragma once
#include <cstddef>

#include <dsa_macros.h>

_begin_dsa

struct SizeInit {

    SizeInit(_std size_t sz) : size(sz) {}

    const _std size_t size;    

};

_end_dsa