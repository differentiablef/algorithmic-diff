// -*- C++ -*- C forwarding header.

#ifndef UTIL_H
#define UTIL_H

#include <iostream>

// a very useful template
template<class T>
std::ostream &operator<<(std::ostream &os, const std::initializer_list<T> &lst)
{
    bool first = true;
    os << "{";
    for(auto p : lst) {
        if(!first)
            os << ", ";
        else
            first = false;
        os << p;
    }
    os << "}";
    return os;
        
}

using std::cout;
using std::cerr;
using std::endl;

#define DEBUG(x) {                                                      \
        std::cout << "DEBUG: " << (#x) << " = " << (x) << std::endl; }  \
    
#define MARKER(x)                                   \
    std::cout << "MARKER: " << (x) << std::endl;



#endif /* UTIL_H */


