#include <iostream>
#include <concepts>
#include <string>
#include <memory>
static const unsigned MAX_degree = 3 ;
/*
this concept used to avoid creating class Btree
from types that do not meet the requirements
*/
template< typename T>
concept comparable = 
requires (T a , T b)
{
    T();
    {a < b} -> std::convertible_to<bool> ;
    {a > b} -> std::convertible_to<bool> ;
    {a == b}-> std::convertible_to<bool> ;
    {a != b}-> std::convertible_to<bool> ;   
};
template< comparable T>
class Node;
/*
class leaf is wrapper for any comparable type .
This class only give simple methods to work with <T> type in Btree
*/
template< comparable T>
class Leaf
{
    public:
        
        Leaf()
            : m_isValid(false)
            , m_value(T())
        { }
        Leaf(T o_value)
            : m_isValid(true)
            , m_value(o_value)
        { }
        Leaf(const Leaf<T>& o)
            : m_isValid(o.m_isValid)
            , m_value(o.m_value)
        { }
        virtual ~Leaf() noexcept = default ;
        inline void operator=(T o_value){m_isValid = true ; m_value = o_value ;}
        void operator= (Leaf<T> o) {m_isValid = o.m_isValid ; m_value = o.m_value ;}
    public:
        inline bool operator<(const T& o_value)const {return m_value < o_value ;}
        inline bool operator>(const T& o_value)const {return m_value > o_value ;}
        inline bool operator==(const T& o_value)const {return m_value==o_value ;}
        inline bool operator!=(const T& o_value)const {return m_value!=o_value ;}
        inline bool operator< (const Leaf<T>& oth)const {return m_value < oth.m_value;}
        inline bool operator> (const Leaf<T>& oth)const {return m_value > oth.m_value;}
        inline bool operator==(const Leaf<T>& oth)const {return m_value==oth.m_value ;}
        inline bool operator!=(const Leaf<T>& oth)const {return m_value!=oth.m_value ;}   
        inline void reset() { m_isValid = false ; m_value = T() ;}
    public:  
        friend class Node<T>;
        friend bool operator<(const T& o_value , const Leaf<T>& this_leaf)
        { return o_value < this_leaf.m_value ;}
        friend bool operator>(const T& o_value , const Leaf<T>& this_leaf)
        { return o_value > this_leaf.m_value ;}
        friend bool operator==(const T& o_value , const Leaf<T>& this_leaf)
        { return o_value == this_leaf.m_value ;}
        friend bool operator!=(const T& o_value , const Leaf<T>& this_leaf)
        { return o_value != this_leaf.m_value ;}
        friend std::ostream& operator<< (std::ostream& output ,const Leaf<T>& this_leaf)
        { 
            output << (this_leaf.m_isValid ? std::to_string(this_leaf.m_value) : "[empty]" );
            return output ;
        }
    private:
            bool m_isValid ;
            T m_value ;
};
/*
class Node is a data structure in which elements with a limited number
and which can have connections with both 
-Higher (parent) branches
-Lower (child) branches
*/
template<comparable T>
class Node
{
public:
    Node()
        : m_this_node(std::array<Leaf<T>, MAX_degree>{})
        , m_prev(nullptr)
        , m_next_l(nullptr)
        , m_next_b(nullptr)
    {}

    Node(Node<T>* parent)
        : m_this_node(std::array<Leaf<T>, MAX_degree>{})
        , m_prev(parent)
        , m_next_l(nullptr)
        , m_next_b(nullptr)
    {}

    ~Node()
    {
        m_next_l.reset();
        m_next_b.reset();
    if (!m_prev) for (auto& leaf : m_this_node) leaf.reset();
    }

public:
    void print(std::ostream&) ;
    bool is_full() const;
    void sorting();
    void separator();
    void push_inline(T o_value);

private:
    std::array<Leaf<T>, MAX_degree> m_this_node;
    std::unique_ptr<Node<T>> m_prev;
    std::unique_ptr<Node<T>> m_next_l;
    std::unique_ptr<Node<T>> m_next_b;
};

template<comparable T>
void Node<T>::print(std::ostream& output)
    {
        if (m_next_l)
            m_next_l->print(output);
        if (m_next_b)
            m_next_b->print(output);
        for (const auto& Leaf : m_this_node)
            output << Leaf << " ";
        output << std::endl;
    }

template<comparable T>
bool Node<T>::is_full() const
{
    for (const auto& leaf : m_this_node)
        if (!leaf.m_isValid)
            return false;
    return true;
}

template<comparable T>
void Node<T>::sorting()
{
    auto smallest = m_this_node.begin();
    for (auto it = m_this_node.begin(); it != m_this_node.end(); ++it)
    if (!it->m_isValid || (smallest->m_isValid && *it < *smallest))
        {
            std::swap(*smallest, *it);
            smallest = it;
        }
    
}

template<comparable T>
void Node<T>::separator()
{
    m_next_l = std::make_unique<Node<T>>(this);
    for(int i = 0 ; i < m_this_node.size()/2 ;++i)
    std::swap(m_next_l->m_this_node[i],m_this_node[i]);
    m_next_b = std::make_unique<Node<T>>(this);
    for(int i = m_this_node.size()/2 + 1 ; i < m_this_node.size();++i)
    std::swap(m_next_b->m_this_node[i],m_this_node[i]);  
}

template<comparable T>
void Node<T>::push_inline(T o_value)
{
    for (auto& leaf : m_this_node)
        if (!leaf.m_isValid)
        {
            leaf = o_value;
            break;
        }
    sorting();
 /*   if (is_full() && !m_prev)
        separator();
    test for Btree function push*/
}




int main()
{
    Leaf<int> a(5);
    Leaf<int> b ;
    Node<int> c ;
    c.push_inline(3);
    c.push_inline(2);
    c.push_inline(4);
    c.print(std::cout);

    
    return 0 ;
}

