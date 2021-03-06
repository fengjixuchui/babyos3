/*
 *	babyos/lib/libc++/cxx.h
 *
 *  Copyright (C) <2020>  <Ruyi Liu>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


/*
 *  2020-03-11		created
 */


#ifndef _CXX_H_
#define _CXX_H_


#include "stddef.h"
#include "malloc.h"


inline void* operator new(size_t size)
{
    return malloc(size);
}

inline void operator delete(void* ptr)
{
    return free(ptr);
}

inline void operator delete(void* ptr, size_t)
{
    return free(ptr);
}

inline void* operator new[](size_t size)
{
    return malloc(size);
}

inline void operator delete[](void* ptr)
{
    return free(ptr);
}

inline void operator delete[](void* ptr, size_t)
{
    return free(ptr);
}

inline void* operator new(size_t, void* ptr)
{
    return ptr;
}

inline void* operator new[](size_t, void* ptr)
{
    return ptr;
}


#endif