/*
 *	babyos/kernel/socket.cc
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
 *  2020-02-26		created
 */



#include "socket.h"
#include "babyos.h"


int32 socket_t::create(uint32 family, uint32 type, uint32 protocol)
{
    m_family    = family;
    m_type      = type;
    m_protocol  = protocol;
    m_state     = SS_UNCONNECTED;

    return 0;
}

