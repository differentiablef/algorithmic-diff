// -*- C++ -*- C forwarding header.

#ifndef SYMBOLIC_H
#define SYMBOLIC_H

#include <iostream>
#include <map>
#include <vector>
#include <memory>

#include "diff.h"

namespace diff {
namespace sym { // symbolic manipulation

using std::vector;
using std::map;
using std::ostream;

typedef std::size_t               index;      // type used to index variables
typedef map<index, constant> assignment; // variable index -> value

// expression types
enum types { Empty, Constant, Variable,
             Tuple, Sum, Product,
             Apply };

    
// Expression Class ////////////////////////////////////////////////////////////

class expression
{
  public:

    // type dependent info
    union info {
        index id;
        const char *name;
        constant value;
    };

    // type:
    //       0: empty
    //       1: constant         (desc ~ value,   child ~ unused)
    //       2: variable         (desc ~ id,      child ~ unused)
    //       5: sum              (desc ~ unused,  child ~ used (sorted and weighted))
    //       6: product          (desc ~ unused,  child ~ used (sorted and weighted))
    //       3: apply            (desc ~ name,    child - used (unsorted))
    //       3: tuple            (desc ~ name,    child ~ used (unsorted))

    types type;         // type of expression
    info desc;          // type dependent descriptive info
    constant weight;    // context dependent weight

    // container holding sub-expressions
    vector<expression*> child;
    
  public: // constructors/destructors 
    expression() {
        this->desc.name = NULL;
        this->weight = 1; };
    
    expression(const types t){
        this->type = t;
        this->desc.name = NULL;
        this->weight = 1; };
    
    expression(const expression &t) { this->copy(t); }
    ~expression() { };
    
  public: // basic interface
    void clear();
    void copy(const expression &t);

  public: // sub-expression interface

    void absorb(expression &e); // sum & product
    void attach(const expression &e, index n); 

  public: // parse tree interface
    
    // replace all leafs of type Variable & id 'n'
    //     with a pointer to `expr`
    void assign(index n, expression &expr);
    
    
  public: // friends
    friend bool operator<(const expression &le, const expression &re);
    friend ostream & operator<<(ostream &os, const expression &a);
    
};

// /////////////////////////////////////////////////////////////////////////////

}; // symbolic
}; // diff



#endif /* SYMBOLIC_H */


