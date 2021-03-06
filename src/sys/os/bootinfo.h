/*
 *	babyos/kernel/bootinfo.h
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
 *  2020-02-14		created
 */


#ifndef _BOOTINFO_H_
#define _BOOTINFO_H_


#include "babyos/types.h"


/* video info */
typedef struct video_info_s {
    uint16 video_mode;
    uint16 width;
    uint16 height;
    uint8  bits_per_pixel;
    uint8  memory_model;
    uint32 vram_base_addr;
} video_info_t;


/*
 *   Offset in Bytes	Name	           Description
 *   0	                BaseAddrLow        Low 32 Bits of Base Address
 *   4	                BaseAddrHigh	   High 32 Bits of Base Address
 *   8	                LengthLow		   Low 32 Bits of Length in Bytes
 *   12	                LengthHigh		   High 32 Bits of Length in Bytes
 *   16	                Type		       Address type of  this range.
 *   The BaseAddrLow and BaseAddrHigh together are the 64 bit BaseAddress of this range.
 *   The BaseAddress is the physical address of the start of the range being specified.
 *   The LengthLow and LengthHigh together are the 64 bit Length of this range.
 *   The Length is the physical contiguous length in bytes of a range being specified.

 *   The Type field describes the usage of the described address range as defined in the table below.

 *   Value	Pneumonic		        Description
 *   1	    AddressRangeMemory	    This run is available RAM usable by the operating system.
 *   2	    AddressRangeReserved	This run of addresses is in use or reserved
 *                                  by the system, and must not be used by the
 *                                  operating system.
 *   Other	Undefined		        Undefined - Reserved for future use.  Any
 *                                  range of this type must be treated by the
 *                                  OS as if the type returned was
 *                                  AddressRangeReserved.
 */
typedef struct address_range_s {
public:
	uint32	base_addr_low;
	uint32	base_addr_high;
	uint32	length_low;
	uint32	length_high;
    uint32  type;

    uint64 base_addr() {
        return ((uint64)base_addr_high << 32) + base_addr_low;
    }
    uint64 length() {
        return ((uint64)length_high << 32) + length_low;
    }
} __attribute__((packed, aligned(4))) address_range_t;


typedef struct memory_layout_s {
	uint32          num_of_range;
	address_range_t ranges[32];
} __attribute__((packed, aligned(4))) memory_layout_t;


class bootinfo_t {
public:
    bootinfo_t();
    ~bootinfo_t();

    void init();

    video_info_t* video_info();
    memory_layout_t* memory_layout();
    void* asc16_font();

private:
    video_info_t*    m_video_info;
    memory_layout_t* m_mem_layout;
};


#endif
