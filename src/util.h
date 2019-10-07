// -*- C++ -*- C forwarding header

#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <experimental/source_location>

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

/* colors taken from https://stackoverflow.com/a/30304782 */
#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define FRED(x) KRED << x << RST
#define FGRN(x) KGRN << x << RST
#define FYEL(x) KYEL << x << RST
#define FBLU(x) KBLU << x << RST
#define FMAG(x) KMAG << x << RST
#define FCYN(x) KCYN << x << RST
#define FWHT(x) KWHT << x << RST

#define BOLD "\x1B[1m"
#define UNDL "\x1B[4m"

#define DEBUG(x) {                                                      \
        std::string _x = __FILE__;  _x.resize(15);                      \
        std::cout << std::right << std::setw(15) << FGRN("DEBUG") << ":" \
                  << std::setw(15) << _x << ":"                         \
                  << std::left << std::setw(4) << __LINE__<< ":"        \
                  << std::right << std::setw(20) << FRED(#x) << " = " << (x) << std::endl; }
    
#define MARKER(x) {                                                     \
        std::string _x = __FILE__; _x.resize(15);                       \
        std::cout << std::right << std::setw(15) << FBLU("MARKER") << ":" \
                  << std::setw(15) << _x << ":"                         \
                  << std::left << std::setw(4) << __LINE__              \
                  << (x) << std::endl; } 
#ifdef _DEBUG_
namespace debug
{
// types
typedef std::experimental::source_location context;

// message passing
extern std::vector<std::string> stack;
extern std::vector<std::string> tags;


template<class T, class T0>
void begin_msg(T &label, T0 &attr, const context &loc)
{
    std::string f = loc.file_name();
    auto p = f.rfind("/");
    if(p != std::string::npos)
        f = f.substr(p+1, f.size() - p);
    f += ":" + std::to_string(loc.line());
    std::cout
        << std::right << attr << std::setw(8) << label << RST << ":"
        << std::right << KMAG << loc.function_name() << RST << ":"
        << std::right << std::setw(15) << f << ":";
    
    if(stack.size() > 0) {
        std::cout  << KRED;
        for( auto p : stack )
            std::cout << std::setw(1) << p;
        std::cout << RST << ":";
    }

    if(tags.size() > 0) {
        std::cout << KCYN;
        for(auto t : tags)
            std::cout << KCYN << t << RST << ":";
        tags.clear();
    }
}

template<class T>
void log(T &message, const context &loc = context::current())
{
    begin_msg("INFO", KBLU, loc);
    std::cout << FCYN(message) << std::endl; 
    return;
}

template<class T, class M>
void dump(M &ident, const T &objs, const context &loc = context::current())
{
    begin_msg("DEBUG", KGRN, loc);
    std::cout << std::left << KGRN << ident << RST << ":="
              << KWHT << objs << RST << std::endl;
    return;
}

template<class T>
void tag(T &message)
{
    tags.push_back(message);
    return;
}

template<class T>
void begin(T &message, const context &loc = context::current())
{
    stack.push_back("*");
    begin_msg("BEGIN", KRED, loc);
    std::cout // << std::left << FRED(std::setw(5) << "BEGIN")
              <<  FYEL(message) << std::endl; 
    
}

template<class T>
void end(T &message, const context &loc = context::current())
{    
    begin_msg("END", KRED, loc);
    std::cout //<< std::left << FRED(std::setw(5) << "END")
              << FYEL(message) << std::endl; 
    stack.pop_back();
    return;
}
// debug::log
// debug::dump
// debug::marker
}

#else
namespace debug
{
typedef std::experimental::source_location context;
template<class T, class M>
inline void dump(M &m, const T &o, const context &l = context::current()) { return; }
template<class T>
inline void log(T &m, const context &l = context::current()) {return;};
template<class T>
inline void tag(T &m, const context &l = context::current()) {return;};
template<class T>
inline void end(T &m, const context &l = context::current()) {return;};
template<class T>
inline void begin(T &m, const context &l=context::current()) {return;};
}
#endif /*_DEBUG_*/

#endif /* UTIL_H */


