#include <iostream>
#include <concepts>
#include <string>
#include <memory>
static constexpr unsigned MAX_degree{3} ;
static constexpr unsigned Empty{0};
static constexpr int Way{0};
static constexpr unsigned Unar{1};
static constexpr unsigned Binar{2};
static constexpr unsigned First{0};
static constexpr unsigned Middle{1};
static constexpr unsigned Last{2};
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
        inline bool operator< (const Leaf<T>& oth)const 
        {
            if(this->m_isValid && oth.m_isValid) return m_value < oth.m_value;
            return m_isValid ? false : true ;
        }
        inline bool operator> (const Leaf<T>& oth)const 
        {
            if(this->m_isValid && oth.m_isValid) return m_value > oth.m_value;
            return (m_isValid) ? true : false ;
        }
        inline bool operator==(const Leaf<T>& oth)const 
        {
            if(m_isValid && oth.m_isValid) return m_value == oth.m_value;
            return (!m_isValid && !oth.m_isValid) ? true : false ;
        }
        inline bool operator!=(const Leaf<T>& oth)const
         {
            if(m_isValid && oth.m_isValid) return m_value != oth.m_value;
            return (!m_isValid && !oth.m_isValid) ? false : true;
         }   
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
        , m_next_m(nullptr)
        , m_next_b(nullptr)
        , m_values_count(Empty)
        , m_is_node (false)
    {}
    Node(Node<T>* parent)
        : m_this_node(std::array<Leaf<T>, MAX_degree>{})
        , m_prev(parent)
        , m_next_l(nullptr)
        , m_next_m(nullptr)
        , m_next_b(nullptr)
        , m_values_count(Empty)
        , m_is_node (false)
    {}
    ~Node()
    {
        m_next_l.reset();
        m_next_m.reset();
        m_next_b.reset();
        
    if (!m_prev) for (auto& leaf : m_this_node) leaf.reset();
    }
public:
    inline bool is_full() const { return MAX_degree == m_values_count ;}
    inline void sorting() { std::sort(m_this_node.begin(),m_this_node.end());}
    inline void create_basic_notes()
    {
        m_next_l = std::make_unique<Node<T>>(this);
        m_next_b = std::make_unique<Node<T>>(this);
        m_is_node = true ;
    }
    inline void create_middle() { m_next_m = std::make_unique<Node<T>>(this); }
    void fill_the_middle()
    {
        create_middle();
        if(m_next_l->m_values_count == Binar)
        {
            m_next_m->push_here(std::exchange(m_next_l->m_this_node[Last],Leaf<T>()));
            m_next_l->count_update() ;
        }
        else
        {
            m_next_m->push_here(std::exchange(m_next_b->m_this_node[First] ,Leaf<T>()));
            m_next_b->count_update() ;
        }
    }
    void count_update() 
    {
        m_values_count = 0 ;
        for(const auto& leaf : m_this_node)
        if(leaf.m_isValid) ++m_values_count;
    }
    void print(std::ostream& output) const
    {
        if (m_next_l)
            m_next_l->print(output);
        if (m_next_m)
            m_next_m->print(output);
        if (m_next_b)
            m_next_b->print(output);
        for (const auto& Leaf : m_this_node)
            output << Leaf << " ";
        output << std::endl;
    }

    bool push_here(Leaf<T> o_value)
    {
        for (auto& leaf : m_this_node)
            if (!leaf.m_isValid)
            {
                leaf = o_value;
               break;
            }
    ++m_values_count;
    sorting();
    return is_full() ;
    }
    
    int way_to_insert( const Leaf<T>& o_value) const
    {
        int count{Empty} ;
        for(const auto& leaf : m_this_node)
        if(leaf.m_isValid)
        {
            if(leaf<o_value) ++count ;
            if(leaf>o_value) --count ;
        }
        return count ;
    }
    
    
    void redistribution(Leaf<T> Middle)
    {
        push_here(Middle);
        if(is_full())
        {
            if(!m_prev) divide();
            else
            {
                auto temp = std::exchange(m_this_node[::Middle],Leaf<T>());
                m_prev->redistribution(temp) ;
            }
        }
        else fill_the_middle() ;
        count_update();
    }
    void divide()
{
    auto left = std::make_unique<Node<T>>(this);
    left->push_here(std::exchange(m_this_node[First], Leaf<T>()));
    auto right = std::make_unique<Node<T>>(this);
    right->push_here(std::exchange(m_this_node[Last], Leaf<T>()));

    if (m_is_node && m_next_l && m_next_b)
    {
        left->create_basic_notes();
        right->create_basic_notes();

        if (m_next_l->m_values_count == Middle)
        {
            left->m_next_l = std::move(m_next_l);
            auto temp = std::exchange(left->m_next_l->m_this_node[Last], Leaf<T>());
            left->m_next_b->push_here(temp);
            left->m_next_l->count_update();
            right->m_next_l = std::exchange(m_next_m, nullptr);
            right->m_next_b = std::exchange(m_next_b, nullptr);
        }
        else if (m_next_b->m_values_count == Middle)
        {
            left->m_next_l = std::exchange(m_next_l, nullptr);
            left->m_next_b = std::exchange(m_next_m, nullptr);
            right->m_next_b = std::exchange(m_next_b, nullptr);
            auto temp = std::exchange(right->m_next_b->m_this_node[Last], Leaf<T>());
            right->m_next_l->push_here(temp);
            right->m_next_b->count_update();
        }
        else
        {
            left->m_next_l = std::exchange(m_next_l, nullptr);
            left->m_next_b->push_here(std::exchange(right->m_next_m->m_this_node[First], Leaf<T>()));
            right->m_next_l->push_here(std::exchange(right->m_next_m->m_this_node[Last], Leaf<T>()));
            right->m_next_b = std::exchange(m_next_b, nullptr);
        }
    }

    std::exchange(m_next_l, std::move(left));
    std::exchange(m_next_b, std::move(right));

    count_update();
    m_is_node = true;
    sorting();
    return;
}


    void push(T o_value)
    {
        if(m_is_node)
        {
            int right_way = way_to_insert(o_value);
            if(right_way < Way) m_next_l->push(o_value);
            if(right_way > Way) m_next_b->push(o_value);
            if(right_way == Way) m_next_b->push(o_value);
        }
       else
         if(push_here(o_value))
         {
            if(m_prev)
            {
                auto temp = std::exchange(m_this_node[Middle],Leaf<T>());
                count_update() ;
                m_prev->redistribution(temp) ;
            }
            else divide() ;
         }
         else return ;
    }
private:
    std::array<Leaf<T>, MAX_degree> m_this_node;
    std::unique_ptr<Node<T>> m_prev;
    std::unique_ptr<Node<T>> m_next_l;
    std::unique_ptr<Node<T>> m_next_m;
    std::unique_ptr<Node<T>> m_next_b;
    unsigned m_values_count ;
    bool m_is_node ;
};

int main()
{
    Node<int> c ;
   
   
    c.push(1);
    c.push(2);
    c.push(3);
    c.push(4);
    c.push(5);
    c.push(6);
    c.push(7);
    
    

    

    c.print(std::cout);


    return 0 ;
}
