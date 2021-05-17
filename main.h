#pragma once
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

#define _std  std::

template<typename List>
struct Node {

    using value_type = typename List::value_type;
    using pointer = typename List::pointer;
    using next = Node*;

    explicit Node(const value_type&);
    explicit Node(value_type&&);
    explicit Node(const Node&);
    explicit Node(Node&&);
    ~Node() = default;

    Node& operator= (const Node&);
    Node& operator= (Node&&);

    bool empty();



};

template<typename List>
bool operator== (const Node<List>&, const Node<List>&);
template<typename List>
bool operator!= (const Node<List>&, const Node<List>&);


template<typename T, typename Alloc>
class List;


template<typename List>
struct Base {

    using iterator_category = _std forward_iterator_tag;
    using difference_type = _std ptrdiff_t;
    using value_type = typename List::value_type;
    using pointer = typename _std allocator_traits<typename List::allocator_type>::pointer;
    using reference = typename List::reference;

};

template<typename List>
struct ConstListIterator : Base<List> {

    using base = Base<List>;
    using iterator_category = typename base::iterator_category;
    using difference_type = typename base::difference_type;
    using value_type = typename base::value_type;
    using pointer = typename base::pointer;
    using reference = typename base::reference;

    reference operator* ();
    ConstListIterator& operator++ ();
    ConstListIterator operator++ (int);
    pointer operator-> ();

};

template<typename List>
struct ListIterator : ConstListIterator<List> {

    using base = ConstListIterator<List>;
    using iterator_category = typename base::iterator_category;
    using difference_type = typename base::difference_type;
    using value_type = typename base::value_type;
    using pointer = typename base::pointer;
    using reference = typename base::reference;

    reference operator* ();
    ListIterator& operator++ ();
    ListIterator operator++ (int);
    pointer operator-> ();

};

template<typename List>
bool operator!= (Base<List>, Base<List>);





template<typename T, typename Alloc = _std allocator<T>>
class List {

public:

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using allocator_type = Alloc; //order matters???
    using iterator = ListIterator<List<T, Alloc>>;
    using const_iterator = ConstListIterator<List<T, Alloc>>;
    using difference_type = typename Base<List<T, Alloc>>::difference_type;
    using size_type = _std size_t;

    // Constructors
    List() {}
    List(const allocator_type&);
    List(const List&);
    List(const List&, const allocator_type&);
    List(List&&);
    List(List&&, const allocator_type&);
    List(size_type, const value_type&);
    List(size_type, const value_type&, const allocator_type&);
    template<typename Iter>
    List(Iter, Iter);
    template<typename Iter>
    List(Iter, Iter, const allocator_type&);
    List(_std initializer_list<T>);
    List(_std initializer_list<T>, const allocator_type&);
    ~List() {}

    List& operator= (_std initializer_list<T>);
    List& operator= (const List&);
    List& operator= (List&&);

    void assign(iterator, iterator);
    void assign(_std initializer_list<T>);
    void assign(size_type, const value_type&);
    iterator begin();
    const_iterator cbegin();
    iterator end();
    const_iterator cend();
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
    iterator insert(const_iterator, iterator, iterator);
    iterator insert(const_iterator, _std initializer_list<T>);
    size_type max_size();
    size_type size();
    void swap(List);

};

template<typename T, typename Alloc>
bool operator== (const List<T, Alloc>&, const List<T, Alloc>&);
template<typename T, typename Alloc>
bool operator!= (const List<T, Alloc>&, const List<T, Alloc>&);