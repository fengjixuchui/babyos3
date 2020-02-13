/*
 *	babyos/kernel/cxx.cc
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


extern "C"
{
    int __cxa_atexit(void (*Destructor) (void *), void *Parameter, void *HomeDSO);
    void __cxa_finalize(void *);
    void __cxa_pure_virtual();
    void __stack_chk_guard_setup();
    void __attribute__((noreturn)) __stack_chk_fail();
    void _Unwind_Resume();
}

void *__dso_handle;
void *__stack_chk_guard(0);


namespace __cxxabiv1
{
    __extension__ typedef int __guard __attribute__((mode(__DI__)));

    extern "C"
    {
        int __cxa_guard_acquire(__guard *Guard)     { return ! *(char *) (Guard); }
        void __cxa_guard_release(__guard *Guard)    { *(char *) Guard = 1; }
        void __cxa_guard_abort(__guard *)           { }
    }
}


int __cxa_atexit(void (*) (void *), void *, void *)
{
    return 0;
}

void _Unwind_Resume()
{
}

void __cxa_finalize(void *)
{
}

void __cxa_pure_virtual()
{
}

void __stack_chk_guard_setup()
{
    unsigned char *Guard;
    Guard = (unsigned char *) &__stack_chk_guard;
    Guard[sizeof(__stack_chk_guard) - 1] = 255;
    Guard[sizeof(__stack_chk_guard) - 2] = '\n';
    Guard[0] = 0;
}

