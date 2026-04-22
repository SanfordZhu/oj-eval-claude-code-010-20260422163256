#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;

        node() : data(nullptr), prev(nullptr), next(nullptr) {}
        node(const T &val) : data(new T(val)), prev(nullptr), next(nullptr) {}
        ~node() { delete data; }
    };

protected:
    node *head;  // sentinel node: head->next is first element, head->prev is last element
    size_t len;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        ++len;
        return cur;
    }
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        --len;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
    private:
        node *ptr;
        const list *container;
    public:
        iterator() : ptr(nullptr), container(nullptr) {}
        iterator(node *p, const list *c) : ptr(p), container(c) {}

        /**
         * iter++
         */
        iterator operator++(int) {
            if (ptr == container->head) throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        /**
         * ++iter
         */
        iterator & operator++() {
            if (ptr == container->head) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int) {
            if (ptr->prev == container->head) throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        /**
         * --iter
         */
        iterator & operator--() {
            if (ptr->prev == container->head) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        /**
         * *it
         * remember to throw if iterator is invalid
         */
        T & operator *() const {
            if (ptr == container->head) throw invalid_iterator();
            return *(ptr->data);
        }
        /**
         * it->field
         * remember to throw if iterator is invalid
         */
        T * operator ->() const {
            if (ptr == container->head) throw invalid_iterator();
            return ptr->data;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
        bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
        bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }

        node *get_node() const { return ptr; }
        const list *get_container() const { return container; }
    };
    /**
     * has same function as iterator, just for a const object.
     * should be able to construct from an iterator.
     */
    class const_iterator {
    private:
        const node *ptr;
        const list *container;

        friend class iterator;
    public:
        const_iterator() : ptr(nullptr), container(nullptr) {}
        const_iterator(const node *p, const list *c) : ptr(p), container(c) {}
        const_iterator(const iterator &it) : ptr(it.get_node()), container(it.get_container()) {}

        const_iterator operator++(int) {
            if (ptr == container->head) throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        const_iterator & operator++() {
            if (ptr == container->head) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        const_iterator operator--(int) {
            if (ptr->prev == container->head) throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        const_iterator & operator--() {
            if (ptr->prev == container->head) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        const T & operator *() const {
            if (ptr == container->head) throw invalid_iterator();
            return *(ptr->data);
        }
        const T * operator ->() const {
            if (ptr == container->head) throw invalid_iterator();
            return ptr->data;
        }
        bool operator==(const iterator &rhs) const { return ptr == rhs.get_node(); }
        bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
        bool operator!=(const iterator &rhs) const { return ptr != rhs.get_node(); }
        bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
    };
    /**
     * Constructs
     * Atleast two: default constructor, copy constructor
     */
    list() : head(new node()), len(0) {
        head->prev = head;
        head->next = head;
    }
    list(const list &other) : head(new node()), len(0) {
        head->prev = head;
        head->next = head;
        for (node *cur = other.head->next; cur != other.head; cur = cur->next) {
            push_back(*(cur->data));
        }
    }
    /**
     * Destructor
     */
    virtual ~list() {
        clear();
        delete head;
    }
    /**
     * Assignment operator
     */
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (node *cur = other.head->next; cur != other.head; cur = cur->next) {
            push_back(*(cur->data));
        }
        return *this;
    }
    /**
     * access the first / last element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const {
        if (empty()) throw container_is_empty();
        return *(head->next->data);
    }
    const T & back() const {
        if (empty()) throw container_is_empty();
        return *(head->prev->data);
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() { return iterator(head->next, this); }
    const_iterator cbegin() const { return const_iterator(head->next, this); }
    /**
     * returns an iterator to the end.
     */
    iterator end() { return iterator(head, this); }
    const_iterator cend() const { return const_iterator(head, this); }
    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const { return len == 0; }
    /**
     * returns the number of elements
     */
    virtual size_t size() const { return len; }

    /**
     * clears the contents
     */
    virtual void clear() {
        while (head->next != head) {
            node *tmp = head->next;
            head->next = tmp->next;
            delete tmp;
        }
        head->prev = head;
        len = 0;
    }
    /**
     * insert value before pos (pos may be the end() iterator)
     * return an iterator pointing to the inserted value
     * throw if the iterator is invalid
     */
    virtual iterator insert(iterator pos, const T &value) {
        node *pos_node = pos.get_node();
        if (pos_node == nullptr || pos.get_container() != this) throw invalid_iterator();
        node *new_node = new node(value);
        insert(pos_node, new_node);
        return iterator(new_node, this);
    }
    /**
     * remove the element at pos (the end() iterator is invalid)
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid
     */
    virtual iterator erase(iterator pos) {
        if (empty()) throw container_is_empty();
        node *pos_node = pos.get_node();
        if (pos_node == nullptr || pos_node == head || pos.get_container() != this) throw invalid_iterator();
        node *next_node = pos_node->next;
        erase(pos_node);
        delete pos_node;
        return iterator(next_node, this);
    }
    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        node *new_node = new node(value);
        new_node->prev = head->prev;
        new_node->next = head;
        head->prev->next = new_node;
        head->prev = new_node;
        ++len;
    }
    /**
     * removes the last element
     * throw when the container is empty.
     */
    void pop_back() {
        if (empty()) throw container_is_empty();
        node *tmp = head->prev;
        head->prev = tmp->prev;
        tmp->prev->next = head;
        delete tmp;
        --len;
    }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        node *new_node = new node(value);
        new_node->next = head->next;
        new_node->prev = head;
        head->next->prev = new_node;
        head->next = new_node;
        ++len;
    }
    /**
     * removes the first element.
     * throw when the container is empty.
     */
    void pop_front() {
        if (empty()) throw container_is_empty();
        node *tmp = head->next;
        head->next = tmp->next;
        tmp->next->prev = head;
        delete tmp;
        --len;
    }
    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        if (len <= 1) return;
        // Allocate raw memory and copy-construct elements
        T *arr = static_cast<T *>(::operator new(len * sizeof(T)));
        node *cur = head->next;
        for (size_t i = 0; i < len; ++i) {
            new (arr + i) T(*(cur->data));
            cur = cur->next;
        }
        std::function<bool(const T&, const T&)> cmp = [](const T &a, const T &b) { return a < b; };
        sjtu::sort(arr, arr + len, cmp);
        cur = head->next;
        for (size_t i = 0; i < len; ++i) {
            *(cur->data) = arr[i];
            cur = cur->next;
        }
        for (size_t i = 0; i < len; ++i) {
            arr[i].~T();
        }
        ::operator delete(arr);
    }
    /**
     * merge two sorted lists into one (both in ascending order)
     * compare with operator< of T
     * container other becomes empty after the operation
     * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
     * the order of equivalent elements of *this and other does not change.
     * no elements are copied or moved
     */
    void merge(list &other) {
        if (this == &other || other.empty()) return;
        if (empty()) {
            // Move all nodes from other to this
            head->next = other.head->next;
            head->prev = other.head->prev;
            head->next->prev = head;
            head->prev->next = head;
            len = other.len;
            other.head->next = other.head;
            other.head->prev = other.head;
            other.len = 0;
            return;
        }
        node *this_cur = head->next;
        node *other_cur = other.head->next;
        while (other_cur != other.head) {
            if (this_cur == head) {
                // Append all remaining other nodes
                node *next_other = other_cur->next;
                // Detach other_cur from other
                other_cur->prev = head->prev;
                other_cur->next = head;
                head->prev->next = other_cur;
                head->prev = other_cur;
                ++len;
                other_cur = next_other;
            } else if (!(*(this_cur->data) < *(other_cur->data)) && !(*(other_cur->data) < *(this_cur->data))) {
                // equal: this elements precede other elements
                node *next_other = other_cur->next;
                // Insert other_cur before this_cur
                other_cur->prev = this_cur->prev;
                other_cur->next = this_cur;
                this_cur->prev->next = other_cur;
                this_cur->prev = other_cur;
                ++len;
                other_cur = next_other;
            } else if (*(other_cur->data) < *(this_cur->data)) {
                // other_cur < this_cur, insert before this_cur
                node *next_other = other_cur->next;
                other_cur->prev = this_cur->prev;
                other_cur->next = this_cur;
                this_cur->prev->next = other_cur;
                this_cur->prev = other_cur;
                ++len;
                other_cur = next_other;
            } else {
                this_cur = this_cur->next;
            }
        }
        other.head->next = other.head;
        other.head->prev = other.head;
        other.len = 0;
    }
    /**
     * reverse the order of the elements
     * no elements are copied or moved
     */
    void reverse() {
        if (len <= 1) return;
        node *cur = head;
        do {
            node *tmp = cur->next;
            cur->next = cur->prev;
            cur->prev = tmp;
            cur = tmp;
        } while (cur != head);
    }
    /**
     * remove all consecutive duplicate elements from the container
     * only the first element in each group of equal elements is left
     * use operator== of T to compare the elements.
     */
    void unique() {
        if (len <= 1) return;
        node *cur = head->next;
        while (cur->next != head) {
            if (*(cur->data) == *(cur->next->data)) {
                node *dup = cur->next;
                dup->prev->next = dup->next;
                dup->next->prev = dup->prev;
                delete dup;
                --len;
            } else {
                cur = cur->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
