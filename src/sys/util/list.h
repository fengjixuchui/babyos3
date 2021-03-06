/*
 *	babyos/kernel/list.h
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


#ifndef _LIST_H_
#define _LIST_H_


#include "babyos/types.h"
#include "spinlock.h"
#include "pool.h"


template<typename T>
class list_node_t {
public:
    void init(const T& data) {
        m_data = data;
        m_prev = nullptr;
        m_next = nullptr;
    }

public:
    T            m_data;
	list_node_t* m_next;
    list_node_t* m_prev;
};


template<typename T>
class list_t {
public:
    class iterator {
        friend class list_t;

    public:
        iterator() {
            m_ptr = nullptr;
        }
        iterator(list_node_t<T>* ptr) {
            m_ptr = ptr;
        }

        iterator& operator ++() {
            m_ptr = m_ptr->m_next;
            return *this;
        }
        iterator& operator --() {
            m_ptr = m_ptr->m_prev;
        }
        iterator operator ++(int) {
            list_t::iterator it(m_ptr);
            m_ptr = m_ptr->m_next;
            return it;
        }
        iterator operator --(int) {
            list_t::iterator it(m_ptr);
            m_ptr = m_ptr->m_prev;
            return it;
        }
        bool operator ==(const iterator &it) {
            return m_ptr == it.m_ptr;
        }
        bool operator !=(const iterator &it) {
            return m_ptr != it.m_ptr;
        }
        iterator& operator = (iterator it) {
            m_ptr = it.m_ptr;
            return *this;
        }
        T& operator * () {
            return m_ptr->m_data;
        }

    private:
        list_node_t<T>* m_ptr;
    };

    list_t<T>() {
        m_head = nullptr;
        m_tail = nullptr;
        m_pool = nullptr;
        m_size = 0;
    }

    ~list_t<T>() {
        list_node_t<T>* p = m_head;
        while (p != nullptr) {
            list_node_t<T>* d = p;
            p = p->m_next;
            free_node(d);
        }
        m_head = m_tail = nullptr;
        m_size = 0;
    }

    void init(object_pool_t* pools) {
        m_head = nullptr;
        m_tail = nullptr;
        m_size = 0;

        uint32 node_size = sizeof(list_node_t<T>);
        if (node_size > SMALL_POOL_SIZE) {
            m_big_pool.init(node_size);
            m_pool = &m_big_pool;
        }
        else {
            m_pool = &pools[node_size];
        }
    }

    list_t<T>& operator = (const list_t<T>& list) {
        m_head = nullptr;
        m_tail = nullptr;
        m_size = 0;

        uint32 node_size = sizeof(list_node_t<T>);
        if (node_size > SMALL_POOL_SIZE) {
            m_big_pool.init(node_size);
            m_pool = &m_big_pool;
        }
        else {
            m_pool = list.m_pool;
        }

        const list_node_t<T>* p = list.m_head;
        while (p != nullptr) {
            push_back(p->m_data);
            p = p->m_next;
        }
        return *this;
    }

    list_node_t<T>* alloc_node(const T& data) {
        list_node_t<T>* node = (list_node_t<T> *) m_pool->alloc_from_pool();
        node->init(data);
        return node;
    }

    void free_node(list_node_t<T>* node) {
        m_pool->free_object((void *) node);
    }

    bool empty() {
        return m_size == 0;
    }

    bool push_front(const T& data) {
        list_node_t<T>* node = alloc_node(data);
        if (node == nullptr) {
            return false;
        }
        if (m_head == nullptr && m_tail == nullptr) {
            m_head = m_tail = node;
        }
        else {
            node->m_next = m_head;
            m_head->m_prev = node;
            m_head = node;
        }
        m_size++;
        return true;
    }

    bool push_back(const T& data) {
        list_node_t<T>* node = alloc_node(data);
        if (node == nullptr) {
            return false;
        }
        if (m_head == nullptr && m_tail == nullptr) {
            m_head = m_tail = node;
        }
        else {
            m_tail->m_next = node;
            node->m_prev = m_tail;
            m_tail = node;
        }
        m_size++;
        return true;
    }

    bool pop_back() {
        if (m_size == 0) {
            return false;
        }
        list_node_t<T> *del = m_tail;
        m_size--;
        if (m_size == 0) {
            m_head = m_tail = nullptr;
        }
        else {
            m_tail = m_tail->m_prev;
            m_tail->m_next = nullptr;
        }
        free_node(del);
        return true;
    }

    bool pop_front() {
        if (m_size == 0) {
            return false;
        }

        list_node_t<T> *del = m_head;
        m_size--;
        if (m_size == 0) {
            m_head = m_tail = nullptr;
        }
        else {
            m_head = m_head->m_next;
            m_head->m_prev = nullptr;
        }
        free_node(del);
        return true;
    }

    list_t<T>::iterator insert(list_t<T>::iterator &it, const T& data) {
        /* insert before end, just push back */
        if (it.m_ptr == nullptr) {
            push_back(data);
            return list_t::iterator(m_tail);
        }

        /* insert before front */
        if (it.m_ptr->m_prev == nullptr) {
            push_front(data);
            return list_t::iterator(m_head);
        }

        /* insert middle */
        list_node_t<T>* node = alloc_node(data);
        if (node == nullptr) {
            return list_t::iterator(nullptr);
        }

        node->m_prev = it.m_ptr->m_prev;
        node->m_next = it.m_ptr;
        it.m_ptr->m_prev->m_next = node;
        it.m_ptr->m_prev = node;

        m_size++;
        return list_t::iterator(node);
    }

    list_t<T>::iterator erase(list_t<T>::iterator &it) {
        list_t<T>::iterator ret = it;

        /* error erase */
        if (m_size == 0) {
            return ret;
        }

        m_size--;

        /* no node left */
        if (m_size == 0) {
            m_head = m_tail = nullptr;
            free_node(it.m_ptr);
            return list_t::iterator(nullptr);
        }

        /* erase head */
        if (m_head == it.m_ptr) {
            m_head = m_head->m_next;
            free_node(it.m_ptr);
            return list_t::iterator(m_head);
        }

        /* erase tail */
        if (m_tail == it.m_ptr) {
            m_tail = m_tail->m_prev;
            m_tail->m_next = nullptr;
            free_node(it.m_ptr);
            return list_t::iterator(m_tail);
        }

        /* erase middle node */
        if (it.m_ptr->m_prev != nullptr) {
            it.m_ptr->m_prev->m_next = it.m_ptr->m_next;
        }
        if (it.m_ptr->m_next != nullptr) {
            it.m_ptr->m_next->m_prev = it.m_ptr->m_prev;
        }

        ret.m_ptr = it.m_ptr->m_next;
        free_node(it.m_ptr);

        return ret;
    }

    iterator find(const T& data) {
        list_t<T>::iterator it = begin();
        while (it != end() && *it != data) {
            it++;
        }

        return it;
    }

    iterator begin() {
        return list_t::iterator(m_head);
    }
    iterator end() {
        return list_t::iterator(nullptr);
    }

    uint32 size() {
        return m_size;
    }

private:
    list_node_t<T>*     m_head;
    list_node_t<T>*     m_tail;
    uint32              m_size;
    object_pool_t*      m_pool;
    object_pool_t       m_big_pool;
};


#endif

