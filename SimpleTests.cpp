#include "SortedBtree.hpp"
#include <ranges>

static const int Start_1 = 0 ;
static const int End_1 = 2000 ;
static const int Start_2 = -100;
static const int End_2 = -400 ;

static Node<int> NumbersTree ;


void range_insert(int start , int end)
{
    
    for ( int elem : std::views::iota(start , end ))
    NumbersTree.push(elem) ;
    return;
}

void find_range(int start ,int end)
{
    for ( int key : std::views::iota(start,end))
    {
       auto result = NumbersTree.Find(key) ;
       if(result.is_valid()) std::cout << result.get_value() << " " ;
    }
    std::cout << std:: endl ;
    return;
}

void range_insert_2(int start , int end)
{
    
    for ( int elem = start ; elem > end ; --elem)
    NumbersTree.push(elem) ;
    return;
}


int main()
{
    // Ascending range insert
    range_insert(Start_1,End_1);
    std::cout << "Btree now have 0-2000 elements" << std :: endl ;
    // print Tree
    NumbersTree.print(std::cout);
     // checking for  first insertion
    std::cout<< "Finding first 0-2000
        elements " << std::endl ;
    find_range(Start_1,End_1);
  
    // checking reset method
    NumbersTree.reset();
    std::cout << "Now Tree is Empty" <<std::endl ;
    NumbersTree.print(std::cout);

    // number descending insert
    for(int i = Start_2 ; i > End_2 ; --i)
    NumbersTree.push(i);
    // print Tree
    std::cout << " new Tree " << std::endl;
    NumbersTree.print(std::cout);
    
    return 0 ;
}
