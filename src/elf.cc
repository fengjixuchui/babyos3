/*
 *	babyos/kernel/elf.h
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
 *  2020-02-18		created
 */



#include "elf.h"
#include "babyos.h"
#include "string.h"
#include "math.h"

static int32 read_file_from(int fd, void* buffer, uint64 offset, uint64 size)
{
    if (os()->fs()->do_seek(fd, offset) < 0) {
        return -1;
    }

    if (os()->fs()->do_read(fd, buffer, size) != (int64)size) {
        return -1;
    }

    return 0;
}

static int32 load_elf_binary(elf64_hdr_t* elf, int fd)
{
    pml4e_t* pml4_table = current->m_vmm.get_pml4_table();
    uint32 file_offset = elf->e_phoff;
    for (int i = 0; i < elf->e_phnum; i++) {
        /* read prog_hdr */
        elf64_phdr_t ph;
        if (read_file_from(fd, &ph, file_offset, sizeof(elf64_phdr_t)) != 0) {
            return -1;
        }

        /* check prog hdr type */
        if (ph.p_type != PT_LOAD) {
            continue;
        }

        void* vaddr = (void*) (ph.p_vaddr & PAGE_MASK);
        uint64 offset = ph.p_vaddr - (uint64)vaddr;
        uint64 len = PAGE_ALIGN(ph.p_memsz + ((uint64)vaddr & (PAGE_SIZE-1)));

        /* mmap */
        int64 ret = current->m_vmm.do_mmap((uint64) vaddr,
                                           len,
                                           PROT_READ | PROT_WRITE | PROT_EXEC,
                                           MAP_FIXED);
        if (ret < 0) {
            return -1;
        }

        /* alloc mem and do map pages */
        uint64 pa = os()->buddy()->alloc_pages(math_t::log(2, len / PAGE_SIZE));
        vmm_t::map_pages(pml4_table, vaddr, pa, len, PTE_W | PTE_U);

        /* read data */
        uint8* va = (uint8 *) PA2VA(pa);
        if (read_file_from(fd, va+offset, ph.p_offset, ph.p_filesz) != 0) {
            return -1;
        }

        if (ph.p_memsz > ph.p_filesz) {
            memset(va+offset+ph.p_filesz, 0, ph.p_memsz - ph.p_filesz);
        }

        file_offset += sizeof(elf64_phdr_t);
    }

    return 0;
}

int32 elf_t::load(trap_frame_t* frame, const char* path)
{
    int ret = 0;

    /* 1. open file */
    int fd = os()->fs()->do_open(path, file_t::MODE_RDWR);
    if (fd < 0) {
        os()->console()->kprintf(RED, "BUG on open file %s!\n", path);
        ret = -1;
        goto end;
    }

    /* 2. read elf from hard disk */
    elf64_hdr_t elf;
    if (os()->fs()->do_read(fd, &elf, sizeof(elf64_hdr_t)) != sizeof(elf64_hdr_t)) {
        ret = -1;
        goto end;
    }

    /* 3. check if it's a valid elf file */
    if (*((uint32 *)elf.e_ident) != ELF_MAGIC) {
        os()->console()->kprintf(RED, "BUG on elf format!\n");
        ret = -1;
        goto end;
    }

    /* 4. load elf binary */
    if (load_elf_binary(&elf, fd) != 0) {
        os()->console()->kprintf(RED, "BUG on load elf binary!\n");
        ret = -1;
        goto end;
    }

    /* 5. set eip */
    frame->rip = (uint64)(elf.e_entry);

end:
    /* 6. close file */
    os()->fs()->do_close(fd);

    return ret;
}
