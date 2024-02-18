#include "SortedBtree.hpp"
#include <ranges>

static const int Start_1 = 0 ;
static const int End_1 = 3000 ;
static const int Start_2 = 0;
static const int End_2 = -400 ;

static Node<int> NumbersTree ;


void range_insert(int start , int end ,bool toUpp)
{
    if(toUpp)
    for ( int elem = start ; elem < end ; ++elem)
    NumbersTree.push(elem) ;
    else
    for ( int elem = start ; elem > end ; --elem)
    NumbersTree.push(elem) ;
}

void find_range(int start ,int end)
{
    for ( int key = start ; key < end ; ++ key)
    {
       auto result = NumbersTree.Find(key) ;
       if(result.is_valid()) std::cout << result.get_value() << " " ;
    }
    std::cout << std:: endl ;
    return;
}

int main()
{
 
 range_insert(Start_1,End_2,true);
 range_insert(Start_2,End_2,false);

NumbersTree.print(std::cout);

find_range(End_2,End_1);
    
    return 0 ;
}
