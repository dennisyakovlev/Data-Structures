#pragma once
#include <cstddef>
#include <cstring>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <iostream>
#define _std  std::
#define print(x) std::cout << x << std::endl;

template<typename T>
struct _singly_list_node {

    using value_type = T;
    using next = _singly_list_node*;

    _singly_list_node(const value_type& val) : data_v{ val }, next_v{ nullptr } {}
    _singly_list_node(const value_type& val, next ptr) : data_v{ val }, next_v{ ptr } {}
    _singly_list_node(value_type&& val) : data_v{ _std move(val) }, next_v{ nullptr } {}
    _singly_list_node(value_type&& val, next ptr) : data_v{ _std move(val) }, next_v{ ptr } {}

    value_type data_v;
    next next_v;

};

template<typename T>
bool operator== (const _singly_list_node<T>& l, const _singly_list_node<T>& r) {
    return (l.next_v == r.next_v) && (l.data_v == r.data_v);
}
template<typename T>
bool operator!= (const _singly_list_node<T>& l, const _singly_list_node<T>& r) {
    return !(l == r);
}

template<typename List>
struct ConstListIterator {

    using iterator_category = _std forward_iterator_tag;
    using difference_type = _std ptrdiff_t;
    using value_type = typename List::value_type;
    using pointer = typename _std allocator_traits<typename List::allocator_type>::pointer;
    using reference = typename List::reference;
    using next = typename List::node_pointer;

    ConstListIterator() : ptr{ nullptr } {}
    ConstListIterator(next node_adr) : ptr{ node_adr } {}

    reference operator* () const {
        return ptr->data_v;
    }
    ConstListIterator& operator++ () {
        ptr = ptr->next_v;
        return *this;
    }
    ConstListIterator operator++ (int) {
        ConstListIterator temp = *this;
        ++(*this);
        return temp;
    }
    //pointer operator-> ();

    next ptr;

};

template<typename List>
struct ListIterator : ConstListIterator<List> {

    using base = ConstListIterator<List>;
    using iterator_category = typename base::iterator_category;
    using difference_type = typename base::difference_type;
    using value_type = typename base::value_type;
    using pointer = typename base::pointer;
    using reference = typename base::reference;

    using base::base;

    reference operator* () {
        return const_cast<reference>(base::operator*());
    }
    ListIterator& operator++ () {
        return static_cast<ListIterator&>(base::operator++());
    }
    ListIterator operator++ (int) {
        ListIterator temp = *this;
        base::operator++ ();
        return temp;
    }
    //pointer operator-> ();

};

template<typename List>
bool operator!= (ConstListIterator<List> l, ConstListIterator<List> r) {
    return l.ptr != r.ptr;
}



template<typename T, typename U, typename = void>
struct _both_same : _std false_type {};

template<typename T, typename U>
struct _both_same<T, U, _std is_same<T, U>> : _std true_type {};

template<typename T, typename U> constexpr bool _both_same_v = _both_same<T, U>::value;

template<typename T1, typename T2, 
         typename Alloc1, typename Alloc2, 
         typename = _std enable_if<_both_same_v<T1, T2> && _both_same_v<Alloc1, Alloc2>>>
struct _list_same {};

template<typename T, typename Alloc = _std allocator<_singly_list_node<T>>>
class List {

public:

    using value_type = T;
    using node_type = _singly_list_node<value_type>;
    using node_pointer = node_type*;
    using allocator_type = Alloc;
    using reference = T&;
    using const_reference = const T&;
    using iterator = ListIterator<List>;
    using const_iterator = ConstListIterator<List>;
    using difference_type = typename ConstListIterator<List>::difference_type;
    using size_type = _std size_t;

    List() : head{ nullptr }, tail{ nullptr }, allocator{} {}
    List(const List& rhs) {
        _construct_range(head, tail, rhs.cbegin(), rhs.cend());
    }
    /*
    List(List&& r) : size_v{ r.size_v }, allocator{} {
        head = r.head;
        tail = r.tail;
        r.head = nullptr;
        r.tail = nullptr;
    }
    */
    template<typename T2, typename Alloc2 = std::allocator<T2>>
    List(List<T2, Alloc2>&& r) {

    }
    List(size_type, const value_type&);
    template<typename Iter>
    List(Iter, Iter);
    List(_std initializer_list<T> lis) {
        _construct_range(head, tail, lis.begin(), lis.end());
    }
    ~List() {}

    List& operator= (_std initializer_list<T>);
    List& operator= (const List&);
    List& operator= (List&&);

    void assign(iterator, iterator);
    void assign(_std initializer_list<T>);
    void assign(size_type, const value_type&);
    iterator begin() {
        return iterator(head);
    }
    const_iterator cbegin() const {
        return const_iterator(head);
    }
    iterator end() {
        return iterator(tail);
    }
    const_iterator cend() const {
        return const_iterator(tail);
    }
    void clear();
    template<typename... Args>
    iterator emplace(const_iterator, Args&&...);
    bool empty();
    iterator erease(const_iterator);
    iterator erease(const_iterator, const_iterator);
    allocator_type get_allocator();
    iterator insert(const_iterator, const value_type&);
    iterator insert(const_iterator, value_type&&);
    iterator insert(const_iterator, size_type, const value_type&);
    template<typename Iter>
    void insert(const_iterator pos, Iter start, Iter end) {
        
        node_pointer curr, ahead;
        _create_new_start(curr, ahead, start);
        _construct_iterate_range(curr, ahead, start, end);

        auto temp = pos.ptr->next_v;
        pos.ptr->next_v = curr;
        curr->next_v = temp;

    }
    iterator insert(const_iterator, _std initializer_list<T>);
    size_type max_size();
    size_type size();
    void swap(List);

private:

    node_pointer _allocate_single() {
        return _std allocator_traits<allocator_type>::allocate(allocator, 1);
    }
    
    template<typename... Args>
    void _construct_single(node_pointer ptr, Args... args) {
        _std allocator_traits<allocator_type>::construct(allocator, ptr, args...);
    }

    template<typename Iter>
    void _create_new_start(node_pointer& current, node_pointer& ahead, Iter& i) {
        current = _allocate_single();
        ahead = _allocate_single();
        _construct_single(current, *i, ahead);
        ++i;
    }

    template<typename Iter>
    void _construct_iterate_range(node_pointer& curr, node_pointer& ahead, Iter start, Iter end) {
        for (; start != end; ++start) {
            curr = ahead;
            ahead = _allocate_single();
            _construct_single(curr, *start, ahead);
        }
    }

    template<typename IterFrom>
    void _construct_range(node_pointer& into_start, node_pointer& into_end,
                          IterFrom from_start, IterFrom from_end) {
        node_pointer curr, ahead;
        _create_new_start(curr, ahead, from_start);

        into_start = curr;
        _construct_iterate_range(curr, ahead, from_start, from_end);
        into_end = ahead;
    }


    void _destroy_single(node_pointer ptr) {
        _std allocator_traits<allocator_type>::destroy(allocator, ptr);
    }

    void _deallocate_single(node_pointer ptr) {
        _std allocator_traits<allocator_type>::deallocate(allocator, ptr, 1);
    }

    void _deallocate_all() {
        for (; head != tail; ++head) {
            _destroy_single(head);
            _deallocate_single(head);
        }
    }

    void _deallocate_range(const_iterator, const_iterator) {

    }

    node_pointer head;
    node_pointer tail;
    Alloc allocator;
    size_type size_v;

};

template<typename T>
List(List<T>&&)->List<T>;

template<typename T, typename Alloc>
bool operator== (const List<T, Alloc>&, const List<T, Alloc>&);
template<typename T, typename Alloc>
bool operator!= (const List<T, Alloc>&, const List<T, Alloc>&);