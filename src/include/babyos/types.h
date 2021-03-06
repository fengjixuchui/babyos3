/*
 *	babyos/kernel/types.h
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
 *  2020-01-20		created
 */


#ifndef _TYPES_H_
#define _TYPES_H_


typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;
typedef unsigned long   uint64;
typedef char            int8;
typedef short           int16;
typedef int             int32;
typedef long            int64;


typedef uint64 size_t;
typedef int64  ssize_t;
typedef size_t off_t;

typedef int    pid_t;
typedef int    uid_t;
typedef int64  ptrdiff_t;


typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned long   uint64_t;
typedef char            int8_t;
typedef short           int16_t;
typedef int             int32_t;
typedef long            int64_t;


typedef uint32_t       u_int32_t;
typedef uint16_t       u_int16_t;
typedef uint8_t        u_int8_t;
typedef uint64_t       u_int64_t;


#define NULL nullptr



#endif
