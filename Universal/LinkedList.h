/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

template<typename T>
class LinkedListNode {
public:
    LinkedListNode();

    void set_previous(T*);
    void set_next(T*);

    T* previous() const;
    T* next() const;
};

template<typename T>
inline LinkedListNode<T>::LinkedListNode()
{
    set_previous(0);
    set_next(0);
}

template<typename T>
inline void LinkedListNode<T>::set_previous(T* previous)
{
    static_cast<T*>(this)->m_previous = previous;
}

template<typename T>
inline void LinkedListNode<T>::set_next(T* next)
{
    static_cast<T*>(this)->m_next = next;
}

template<typename T>
inline T* LinkedListNode<T>::previous() const
{
    return static_cast<const T*>(this)->m_previous;
}

template<typename T>
inline T* LinkedListNode<T>::next() const
{
    return static_cast<const T*>(this)->m_next;
}

template<typename T>
class LinkedList {
public:
    LinkedList() { }

    size_t is_empty() const { return m_size == 0; };
    size_t size() const { return m_size; };

    T* head() const { return m_head; }
    T* tail() const { return m_tail; }

    void add_last(T*);
    void add_first(T*);
    T* remove(T*);

private:
    size_t m_size { 0 };
    T* m_head { nullptr };
    T* m_tail { nullptr };
};

template<typename T>
inline void LinkedList<T>::add_first(T* node)
{
    if (m_head == nullptr) {
        m_head = node;
        m_tail = node;
        node->set_previous(0);
        node->set_next(0);
    } else {
        m_head->set_previous(node);
        node->set_next(m_head);
        node->set_previous(0);
        m_head = node;
    }

    m_size++;
}

template<typename T>
inline void LinkedList<T>::add_last(T* node)
{
    if (m_tail == nullptr) {
        m_head = node;
        m_tail = node;
        node->set_previous(0);
        node->set_next(0);
    } else {
        m_tail->set_next(node);
        node->set_previous(m_tail);
        node->set_next(0);
        m_tail = node;
    }

    m_size++;
}

template<typename T>
inline T* LinkedList<T>::remove(T* node)
{
    if (node->previous() != nullptr) {
        node->previous()->set_next(node->next());
    } else {
        m_head = node->next();
    }

    if (node->next() != nullptr) {
        node->next()->set_previous(node->previous());
    } else {
        m_tail = node->previous();
    }

    m_size--;
    return node;
}
