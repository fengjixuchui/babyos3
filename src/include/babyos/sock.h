/*
 *	babyos/include/sock.h
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
 *  2020-03-12		created
 */


#ifndef _SOCK_H_
#define _SOCK_H_


class sock_addr_t {
public:
    unsigned short m_family;        /* address family, AF_xxx */
};


typedef enum address_family_e {
    AF_LOCAL = 1,
    AF_INET = 2,
    AF_MAX,
} address_family_t;


#endif