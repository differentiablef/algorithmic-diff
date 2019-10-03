// -*- C++ -*- C forwarding header.

#ifndef SYMBOLIC_H
#define SYMBOLIC_H

#include <map>
#include <list>

#include "diff.h"

namespace diff
{
// namespace for symbolic calculations
namespace symbolic {

typedef std::size_t               index;      // type used to index variables
typedef std::map<index, constant> assignment; // variable index -> value

typedef std::map<index, index>    term;       // variable index -> op index
//typedef std::map<term, constant>  expression; // group of terms to be combined


// Expression Eval Tree ////////////////////////////////////////////////////////

class expression
{
  public:
    enum node_type
        { Empty, Constant, Variable, Apply, Tuple, Sum, Product};
    
    // node type:
    //       0: empty
    //       1: numeric constant   (lhs ~ value,      rhs ~ NULL)
    //       2: variable           (lhs ~ id,         rhs ~ expo (id))
    //       3: apply function     (lhs ~ callable,   rhs ~ expr_tree *)
    //
    //       4: tuple               (lhs ~ exprs,     rhs ~ NULL)
    //       5: addition            (lhs ~ exprs,     rhs ~ NULL)
    //       6: multiplication      (lhs ~ exprs,     rhs ~ NULL)
    //
    //       7: subtraction         (lhs ~ expr_tree*, rhs ~ expr_tree*)
    //       8: division            (lhs ~ expr_tree*, rhs ~ expr_tree*)
    
    struct node
    {
        node_type type;
        union {
            index id;
            constant value;
            std::list<expression *> *exprs;
            void *ptr;
        } lhs, rhs;
    };


  private:
    struct node root;
    
  public:
    expression() { root.type = Empty; };
    expression(const expression &t) {  this->copy(t); };
    ~expression() { };
    
  public:
    void clear();
    void copy(const expression &t);
    
};

}; // symbolic
}; // diff



#endif /* SYMBOLIC_H */


