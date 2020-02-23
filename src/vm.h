/*
 *	babyos/kernel/vm.h
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
 *  2020-02-17		created
 */



#ifndef _VM_H_
#define _VM_H_

#include "types.h"
#include "traps.h"
#include "page.h"


#define USER_VM_SIZE		(0xc0000000)
#define VM_UNMAPPED_BASE	(USER_VM_SIZE / 3)
#define USER_PTRS_PER_PGD   (USER_VM_SIZE / PGDIR_SIZE)


/* protect flags */
#define PROT_NONE           0x0       /* page can not be accessed */
#define PROT_READ           0x1       /* page can be read */
#define PROT_WRITE          0x2       /* page can be written */
#define PROT_EXEC           0x4       /* page can be executed */

/* map flags */
#define MAP_FIXED           0x10      /* Interpret addr exactly */

/* vm_flags.. */
#define VM_READ		        0x0001
#define VM_WRITE	        0x0002
#define VM_EXEC		        0x0004

#define VM_STACK	        0x0177


class vmm_t;
typedef struct vm_area_s {
	uint64		m_start;
	uint64		m_end;
	uint64		m_page_prot;
	uint64		m_flags;
	struct vm_area_s*	m_next;
} vm_area_t;


class vmm_t {
public:
	void init();
	int32 copy(const vmm_t& vmm);

	uint64 do_mmap(uint64 addr, uint64 len, uint32 prot, uint32 flags);
	int32 do_munmap(uint64 addr, uint64 len);

	/* Look up the first VMA which satisfies  addr < vm_end,  NULL if none. */
	vm_area_t* find_vma(uint64 addr);
	vm_area_t* find_vma(uint64 addr, vm_area_t*& prev);

	uint32 insert_vma(vm_area_t* vma);
	uint32 remove_vma(vm_area_t* vma, vm_area_t* prev);
	uint64 get_unmapped_area(uint64 len);
	uint32 do_page_fault(trap_frame_t* frame);

	pml4e_t* get_pml4_table();
	void set_pml4_table(pml4e_t* pml4_table);

    void   release();

    static uint64 va_to_pa(void* va);
    static void map_pages(pml4e_t *pml4_table, void *va, uint64 pa, uint64 size, uint32 perm);


private:
	int32 copy_vma(vm_area_t* mmap);
	int32 do_protection_fault(vm_area_t* vma, uint64 addr, bool write);
    void  make_pte_write(void* va);
    int32 expand_stack(vm_area_t* vma, uint64 addr);

    void   free_page_range(uint64 start, uint64 end);
    void   send_sig_segv();

	pml4e_t* copy_pml4_table(pml4e_t* pml4_table);
	pdpe_t*  copy_pdp_table(pdpe_t* pdp_table);
	pde_t*   copy_pd_table(pde_t* pd_table);
	pte_t*   copy_page_table(pte_t* page_table);

    void free_pml4_table();
	void free_pdp_table(pdpe_t* pdp_table);
	void free_pd_table(pde_t* pd_table);
	void free_page_table(pte_t* page_table);

    static pdpe_t* get_pdp_table(pml4e_t* pml4_table, void* v);
    static pde_t*  get_pd_table(pdpe_t* pdp_table, void* v);
    static pte_t*  get_page_table(pde_t* pd_table, void* v);

private:
	vm_area_t*	m_mmap;
    pml4e_t*    m_pml4_table;

	///////
	uint64		m_code_start, m_code_end;
	uint64		m_data_start, m_data_end;
	uint64		m_brk_start, m_brk;
	uint64		m_stack_start;
	uint64		m_arg_start, m_arg_end;
	uint64		m_env_start, m_env_end;
};

#endif
