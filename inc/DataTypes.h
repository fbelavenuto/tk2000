
#pragma once

/* Constants */
const int VIDEOWIDTH = 280;
const int VIDEOHEIGHT = 192;

/* Typedefs */
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

static_assert(alignof(byte) == 1, "byte not have 1 byte");
static_assert(alignof(word) == 2, "byte not have 2 bytes");
static_assert(alignof(dword) == 4, "byte not have 4 bytes");

