// Copyright (c) 2020 FBLabs
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#pragma once

/* Defines */


/* Constants */

constexpr int VIDEOWIDTH{ 280 };
constexpr int VIDEOHEIGHT{ 192 };
constexpr double MS_TO_RUN{ 50.0 };

/* Typedefs */

using byte = unsigned char;
using word = unsigned short;
using dword = unsigned int;

/* Asserts */

static_assert(alignof(byte) == 1, "byte not have 1 byte");
static_assert(alignof(word) == 2, "word not have 2 bytes");
static_assert(alignof(dword) == 4, "dword not have 4 bytes");
