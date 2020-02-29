/*
 *	babyos/kernel/slab.cc
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
 *  2020-02-29		created
 */


#include "slab.h"
#include "babyos.h"
#include "cache.h"



kmem_cache_t::kmem_cache_t()
{
}

kmem_cache_t::~kmem_cache_t()
{
}

void kmem_cache_t::create(uint32 size)
{
    if (size > 256) {
        os()->panic("now kmem cache only support small object...");
    }

    m_slabs_partial.init();
    m_slabs_full.init();
    m_slabs_free.init();

    m_lock.init();
    m_objsize = size;
    m_gfporder = 0;

    estimate();
}

void kmem_cache_t::destroy()
{
    /* TODO: */
}

void* kmem_cache_t::alloc()
{
    uint64 flags;
    m_lock.lock_irqsave(flags);
    void* obj = alloc_one();
    m_lock.unlock_irqrestore(flags);

    return obj;
}

void kmem_cache_t::free(void* obj)
{
    uint64 flags;
    m_lock.lock_irqsave(flags);
    free_one(obj);
    m_lock.unlock_irqrestore(flags);
}

void kmem_cache_t::init_objs(slab_t* slab)
{
    uint32* free_table = (uint32 *) (slab + 1);
    uint32 i = 0;
    for (i = 0; i < m_num_per_slab; i++) {
        free_table[i] = i+1;
    }
    free_table[i-1] = slab_t::END;
}

void kmem_cache_t::grow()
{
    uint64 pa = os()->mm()->alloc_pages(m_gfporder);
    void* addr = P2V(pa);

    slab_t* slab = (slab_t *)addr;
    slab->m_color_off = L1_CACHE_ALIGN(m_num_per_slab*sizeof(uint32) + sizeof(slab_t));
    slab->m_mem = (uint8 *)addr + slab->m_color_off;
    slab->m_in_use = 0;
    slab->m_cache = this;

    init_objs(slab);

    m_slabs_free.add_tail(&slab->m_list_node);
}

slab_t* kmem_cache_t::get_slab()
{
    slab_t* slab = list_entry(m_slabs_partial.head(), slab_t, m_list_node);
    if (slab == NULL) {
        if (!m_slabs_free.empty()) {
            /* full -> partial */
            slab = list_entry(m_slabs_free.remove_head(), slab_t, m_list_node);
            m_slabs_partial.add_head(&slab->m_list_node);
        }
    }

    return slab;
}

void* kmem_cache_t::alloc_one()
{
    slab_t* slab = get_slab();
    if (slab == NULL) {
        grow();
    }

    slab = get_slab();
    return slab_alloc(slab);
}

void* kmem_cache_t::slab_alloc(slab_t* slab)
{
    slab->m_in_use++;
    void* obj = (uint8 *)slab->m_mem + slab->m_first_free*m_objsize;
    uint32* free_table = (uint32 *) (slab + 1);
    slab->m_first_free = free_table[slab->m_first_free];

    if (slab->m_first_free == slab_t::END) {
        /* partial -> full */
        m_slabs_partial.remove(&slab->m_list_node);
        m_slabs_full.add_head(&slab->m_list_node);
    }

    return obj;
}

void kmem_cache_t::estimate()
{
    uint32 base = sizeof(slab_t);
    uint32 extra = sizeof(uint32);
    uint32 total = PAGE_SIZE << m_gfporder;

    int i = 0;
    while (i*m_objsize + L1_CACHE_ALIGN(base + i*extra) <= total) {
        i++;
    }

    m_num_per_slab = i - 1;
}

void kmem_cache_t::free_one(void* obj)
{
    slab_t* slab = (slab_t *) ((uint64)obj & (~(PAGE_SIZE-1)));
    uint32 index = ((uint8 *)obj - (uint8 *)slab->m_mem) / m_objsize;

    uint32* free_table = (uint32 *) (slab + 1);
    free_table[index] = slab->m_first_free;
    slab->m_first_free = index;

    if (slab->m_in_use == m_num_per_slab) {
        /* full -> partial */
        m_slabs_full.remove(&slab->m_list_node);
        m_slabs_partial.add_head(&slab->m_list_node);
    }

    slab->m_in_use--;
    if (slab->m_in_use == 0) {
        /* partial -> free */
        m_slabs_partial.remove(&slab->m_list_node);
        m_slabs_free.add_head(&slab->m_list_node);
    }
}


void cache_sizes_t::init(uint64 size, kmem_cache_t* cache)
{
    m_size = size;
    m_cache = cache;
}
