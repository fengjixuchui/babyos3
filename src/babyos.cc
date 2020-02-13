/*
 *	babyos/kernel/babyos.cc
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
 *  2020-02-13		created
 */

#include "babyos.h"
#include "x86.h"

static babyos_t babyos;
babyos_t* babyos_t::get_instance()
{
    return &babyos;
}

babyos_t::babyos_t()
{
}

babyos_t::~babyos_t()
{
}

void babyos_t::init()
{
    const char* welcome = "Welcome to babyos..\n";

    /* serial port */
    this->uart.early_init();
    uart.puts(welcome);

    /* VBE */
    this->vbe.init();
    rect_t rc = { 0, 0, vbe.width(), vbe.height() };
    vbe.fill_rectangle(rc, RGB(0x40, 0, 0x30));
    int i = 10;
    for (const char* p = welcome; *p != '\n'; p++) {
        vbe.draw_asc16(*p, i, 10, RGB(0xff, 0xff, 0xff));
        i += 8;
    }
}

void babyos_t::run()
{
    while (true) {
        halt();
    }
}