#include <iostream>
#include <concepts>
#include <compare>
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
concept comparable = std::three_way_comparable<T> 
                    && std::default_initializable<T> ;

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
        inline T get_value() const  { return m_value ;}
        bool is_valid() const noexcept { return m_isValid ;}
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
 // Node Default ctor
    Node()
        : m_this_node(std::array<Leaf<T>, MAX_degree>{})
        , m_prev(nullptr)
        , m_next_l(nullptr)
        , m_next_m(nullptr)
        , m_next_b(nullptr)
        , m_helper(nullptr)
        , m_values_count(Empty)
        , m_is_node (false)
    {}

//  ctor for child nodes
       Node(Node<T>* parent)
        : m_this_node(std::array<Leaf<T>, MAX_degree>{})
        , m_prev(parent)
        , m_next_l(nullptr)
        , m_next_m(nullptr)
        , m_next_b(nullptr)
        , m_helper(nullptr)
        , m_values_count(Empty)
        , m_is_node (false)
    {}

// Node destructor reset all pointers and all values in root
  
    ~Node() noexcept = default ;
    
    void operator= (Node<T>& another)
    {
        std::swap(m_this_node,another.m_is_node);
        std::swap(m_values_count,another.m_values_count);
        if(another.m_is_node)
        {
            m_next_l = std::exchange(another.m_next_l,nullptr);
            m_next_l->prev = this ;
            m_next_m = std::exchange(another.m_next_m,nullptr);
            if ( m_next_m) m_next_m->prev = this ;
            m_helper = std::exchange(another.m_helper,nullptr);
            if(m_helper) m_helper->prev = this ;
            m_next_b = std::exchange(another.m_next_b,nullptr);
            m_next_b->prev = this ;
            m_is_node = true ;
        }
    }

public:

// public push method for user
    void push(T o_value)
    {
        if ( m_is_node )  
             search_place( o_value );   
        else 
            if(push_here(o_value))
            reconstruction();

    }
// printing all Btree and tagging the nodes
 
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
            if(m_is_node) std::cout<< "---" << "Node" ;
            output <<"---"<< m_values_count << std::endl;
    }
 
// find method with key use by value parameter
 
     Leaf<T> Find(const T& key) 
{
    if(m_this_node[::Last] == key) return m_this_node[::Last];
    if(m_this_node[::Middle].is_valid() && m_this_node[::Middle] == key)
        return m_this_node[::Middle];
    if(m_is_node)
    {
        if(m_this_node[::Last] < key)   
            return m_next_b->Find(key);
        else if(m_next_m && m_this_node[::Middle] < key) 
            return m_next_m->Find(key);
        else    
            return m_next_l->Find(key);
        }
    }
    return Leaf<T>() ;
}
// Clear method

void reset()
{
    m_next_l.reset();
    m_next_m.reset();
    m_next_b.reset();
    m_helper.reset();
    m_is_node = false ; 
    if (!m_prev) for (auto& leaf : m_this_node) leaf.reset();
    node_update();
}


private:
  
    inline bool is_full() const { return MAX_degree == m_values_count ;}
    inline void node_update() { std::sort(m_this_node.begin(),m_this_node.end()); count_update();}
    inline void create_middle_node() { m_next_m = std::make_unique<Node<T>>(this); }
    inline void create_helper() { m_helper= std::make_unique<Node<T>>(this); }
    void create_left_and_right() 
    {
        m_next_l = std::make_unique<Node<T>>(this);
        m_next_b = std::make_unique<Node<T>>(this);
        m_is_node = true ;
    }
    void count_update() 
    {
        m_values_count = 0 ;
        for(const auto& leaf : m_this_node)
        if(leaf.m_isValid) ++m_values_count;
    }
    inline void set_prev()
    {
        m_next_l->m_prev = this ;
        if(m_next_m) m_next_m->m_prev = this ;
        m_next_b->m_prev = this ;
    }
    // searching right way to insert new element
  
    void search_place(T o_value) 
    {
         int right_way{Empty} ;
        for(const auto& leaf : m_this_node)
            if(leaf.m_isValid)
        (   leaf < o_value  ||  leaf == o_value)    ? ++right_way : --right_way ;
               
        if(right_way < Way) m_next_l->push(o_value);
        else if(right_way > Way) m_next_b->push(o_value);
        else if(right_way == Way) m_next_m->push(o_value);
    }
   
    // node line push and return counting is full
   
    bool push_here(Leaf<T> o_value)
    {
         m_this_node[First] = o_value ;
         node_update() ;
         return is_full() ;
    }

// choosing how to reconstruct node
    void reconstruction()
    {
        if(!m_prev ) // location is root
        divide_equally();
        else if (m_prev->m_values_count == ::Unar) // creating middle node
        m_prev->create_a_middle();
        else // dividing this node in 4 
        m_prev->reserve_node();
    }

// m_root reconstruct with two nodes

    void divide_equally()
    {
       if(!m_is_node)
            simple_dividing() ;
        else
            node_dividing() ;
    }

 // First dividing is too Easy
 
    void simple_dividing()
    {
        create_left_and_right();
        m_next_l->push_here(std::exchange(m_this_node[::First],Leaf<T>()));
        m_next_b->push_here(std::exchange(m_this_node[::Last],Leaf<T>()));
        node_update();
        return ;
    }

 // dividing with available nodes
 
    void node_dividing()
    {

     auto temp_little = std::make_unique<Node<T>>(this);
     auto temp_bigger = std::make_unique<Node<T>>(this);
    temp_little->push_here(std::exchange(m_this_node[::First],Leaf<T>()));
    temp_bigger->push_here(std::exchange(m_this_node[::Last],Leaf<T>()));
    node_update();

    temp_little->create_left_and_right();
    temp_bigger->create_left_and_right();

    temp_little->m_next_l = std::exchange(m_next_l ,nullptr);
    temp_little->m_next_b = std::exchange(m_next_m ,nullptr);
    temp_bigger->m_next_l = std::exchange(m_next_b,nullptr);
    temp_bigger->m_next_b = std::exchange(m_helper,nullptr);
    m_next_l = std::exchange(temp_little,nullptr) ;
    m_next_l->set_prev();
    m_next_b = std::exchange(temp_bigger,nullptr) ;
    m_next_b->set_prev();
    }

// create_a_middle_function

    void create_a_middle()
{
    create_middle_node();
    if(m_next_b->is_full())
    {
        push_here(std::exchange(m_next_b->m_this_node[::Middle],Leaf<T>()));
        m_next_m->push_here(std::exchange(m_next_b->m_this_node[::First],Leaf<T>()));
        m_next_b->node_update();
        if(m_next_b->m_is_node)
        {
            m_next_m->create_left_and_right();
            m_next_m->m_next_l = std::exchange(m_next_b->m_next_l,std::make_unique<Node<T>>(m_next_b->m_next_b->m_prev));
            m_next_m->m_next_b = std::exchange(m_next_b->m_next_m,nullptr);
            m_next_b->m_next_l = std::exchange(m_next_b->m_next_b,std::make_unique<Node<T>>(m_next_b->m_next_l->m_prev));
            m_next_b->m_next_b = std::exchange(m_next_b->m_helper,nullptr);

        }

    }
    else if(m_next_l->is_full())
    {
        push_here(std::exchange(m_next_l->m_this_node[::Middle],Leaf<T>()));
        m_next_m->push_here(std::exchange(m_next_l->m_this_node[::Last],Leaf<T>()));
        m_next_l->node_update();
        if(m_next_l->m_is_node)
        {
            m_next_m->create_left_and_right();
            m_next_m->m_next_l = std::exchange(m_next_l->m_next_b,std::make_unique<Node<T>>(m_next_l->m_next_l->m_prev));
            m_next_m->m_next_b = std::exchange(m_next_l->m_helper,nullptr);
            m_next_l->m_next_b = std::exchange(m_next_l->m_next_m,nullptr);
        }

    }
}

// create a m_helper to divide m_prev

    void reserve_node()
    {
        create_helper();
        if(m_next_b->is_full())
        {
            push_here(std::exchange(m_next_b->m_this_node[::Middle],Leaf<T>()));
            m_helper->push_here(std::exchange(m_next_b->m_this_node[::Last],Leaf<T>()));
            m_next_b->node_update();
            if(m_next_b->m_is_node)
            {
                m_helper->create_left_and_right();
                m_helper->m_next_l = std::exchange(m_next_b->m_next_b,std::make_unique<Node<T>>(m_next_b->m_next_l->m_prev)) ;
                m_helper->m_next_b = std::exchange(m_next_b->m_helper,nullptr);
                m_helper->set_prev();
                m_next_b->m_next_b = std::exchange(m_next_b->m_next_m,nullptr);
            }
            
        }
       else if(m_next_l->is_full())
        {
            push_here(std::exchange(m_next_l->m_this_node[::Middle],Leaf<T>()));
            node_swap_l();
            m_next_m->push_here(std::exchange(m_next_l->m_this_node[::Last],Leaf<T>()));
            m_next_l->node_update();
            if(m_next_l->m_is_node)
            {
                m_next_m->create_left_and_right();
                m_next_m->m_next_l = std::exchange(m_next_l->m_next_b,std::make_unique<Node<T>>(m_next_l->m_next_b->m_prev)) ;
                m_next_m->m_next_b = std::exchange(m_next_l->m_helper,nullptr) ;
                m_next_m->set_prev();
                m_next_l->m_next_b = std::exchange(m_next_l->m_next_m,nullptr);
            }
        }
        else if(m_next_m ->is_full())
        {
            push_here(std::exchange(m_next_m->m_this_node[::Middle],Leaf<T>()));
            m_helper->push_here(std::exchange(m_next_m->m_this_node[::Last],Leaf<T>()));
            std::swap(m_next_b,m_helper);
            m_next_m->node_update();
            if(m_next_m->m_is_node)
            {
                m_next_b->create_left_and_right();
                m_next_b->m_next_l = std::exchange(m_next_m->m_next_b,std::make_unique<Node<T>>(m_next_m->m_next_l->m_prev)) ;
                m_next_b->m_next_b = std::exchange(m_next_m->m_helper,nullptr);
                m_next_b->set_prev();
                m_next_m->m_next_b = std::exchange(m_next_m->m_next_m,nullptr);
            }
            
        }
        reconstruction(); 
    }

 
 // use this method to change bitmasks pointers
 
    void node_swap_l()
    {
        std::swap(m_next_b,m_helper);
        std::swap(m_next_m,m_next_b);
    }

private:
    std::array<Leaf<T>, MAX_degree> m_this_node; // elements in this node
    Node<T>* m_prev; // parent
    std::unique_ptr<Node<T>> m_next_l; // little node
    std::unique_ptr<Node<T>> m_next_m; // middle node
    std::unique_ptr<Node<T>> m_next_b; // greater node
    std::unique_ptr<Node<T>> m_helper; // helper use when need to reserve node
    unsigned m_values_count ; // valid elements count
    bool m_is_node ; // if this is a node or not 
};

