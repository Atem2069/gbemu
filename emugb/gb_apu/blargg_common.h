
// Sets up common environment for Shay Green's libraries.
//
// Don't modify this file directly; #define HAVE_CONFIG_H and put your
// configuration into "config.h".

// Copyright (C) 2004-2005 Shay Green.

#pragma once

// Source files have #include BLARGG_SOURCE_BEGIN at the beginning
#ifndef BLARGG_SOURCE_BEGIN
	#define BLARGG_SOURCE_BEGIN "blargg_source.h"
#endif

// uint8_t, int16_t, etc.
#include <cstdint>

// Common standard headers
#include <cstddef>
#include <cassert>
#include <limits>
