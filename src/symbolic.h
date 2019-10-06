// -*- C++ -*- C forwarding header.

#ifndef SYMBOLIC_H
#define SYMBOLIC_H

#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <initializer_list>
#include <string>

#include "diff.h"

namespace diff {
namespace sym { // symbolic manipulation

using std::vector;
using std::map;
using std::ostream;

// Defs  ///////////////////////////////////////////////////////////////////////

// Expression Types ////////////////////////////////////////////////////////////

enum types { Empty, Constant, Variable,
             Tuple, Sum, Product,
             Apply };

typedef std::size_t  index;      // type indexing variables

// Expression Class ////////////////////////////////////////////////////////////

class expression
{
  public:
    typedef std::shared_ptr<expression> child_type;
    
    // type dependent info
    union info
    {
        index id;
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

    const char *name;  // label 
    types type;         // type of expression
    info desc;          // type dependent descriptive info
    constant weight;    // context dependent weight
    bool copied;        // whether we are a copy of another expression
    // container holding sub-expressions
    vector<expression*> child;
    
  public: // constructors/destructors 
    expression() {
        this->copied = false;
        this->weight = 1;
        this->name  = NULL;  };
    
    expression(const types t) {
        this->copied = false;
        this->type  = t;
        this->name  = NULL;
        this->weight = 1; };
    
    expression(const expression &t) { this->copy(t); };
    
    ~expression() {
        if(this->copied ||\
           this->type == types::Product ||\
           this->type == types::Sum)
            this->clear();
    };
    
  public: // basic interface
    void        set_name(const char *name) { this->name = name; };
    const char *get_name()                 { return this->name; };

    void        copy(const expression &t);
    void        clear();

  public: // type-dependent interface

    // constant & variable
    void        set_value(constant val); // sets type to types::Constant
    constant    get_value();             // asserts: type == types::Constant
    void        set_id(index n);         // sets type to types::Variable
    index       get_id();                // asserts: type == types::Variable

    // apply-function
    void        set_args(std::initializer_list<expression*> il);
    void        set_arg(index n, expression &e);
    expression &get_arg(index n);

    // returns expression representing 'function' at `il`
    expression  at(std::initializer_list<expression*> il);

    // sum & product
    expression &absorp(const expression &e);   // asserts: type in {types::Sum, types::Product}
    
  public: // parse tree interface
    
    // replace all leafs equivalent to `var`
    //     with a pointer to `expr`
    // asserts: var.type == types::Variable
    bool assign(expression &var, expression &expr);
    
    
  public: // friends (comparison and printing) 
    friend bool      operator<(const expression &le, const expression &re);
    friend ostream & operator<<(ostream &os, const expression &a);


  public:
    expression &operator+=(const expression &rhs)
    {
        if(this->type == types::Sum)
            return this->absorp(rhs);
        else {
            expression *e;
            if(rhs.type == types::Sum) {
                e = new expression(rhs);
                e->absorp(*this);
            } else {
                e = new expression(Sum);
                e->absorp(*this);
                e->absorp(rhs);
            }
            this->clear();
            this->copy(*e);
            delete e;
            return (*this);
        }
        
    }

    expression &operator*=(const expression &rhs)
    {
        if(this->type == types::Product)
            return this->absorp(rhs);
        else {
            expression *e;
            if(rhs.type == types::Product) {
                e = new expression(rhs);
                e->absorp(*this);
            } else {
                e = new expression(Product);
                e->absorp(*this);
                e->absorp(rhs);
            }
            this->clear();
            this->copy(*e);
            delete e;
            return (*this);
        }
    }
    expression operator+(const expression &rhs)
    {
        if(this->type == types::Sum)
            return expression(*this).absorp(rhs);
        if(rhs.type == types::Sum)
            return expression(rhs).absorp(*this);
    }
    
};


// because I don't feel like looking for this spelling error.
#define absorb absorp 

// /////////////////////////////////////////////////////////////////////////////

}; // symbolic
}; // diff



#endif /* SYMBOLIC_H */


