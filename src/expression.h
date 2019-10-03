// -*- C++ -*- C forwarding header.

#ifndef EXPRESSION_H
#define EXPRESSION_H

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
typedef std::map<term, constant>  expression; // group of terms to be combined

class expr_tree
{
  public:
    
    // node type:
    //      -1: empty
    //       0: numeric constant   (lhs ~ value,      rhs ~ NULL)
    //       1: variable           (lhs ~ id,         rhs ~ expo)
    //       2: list               (lhs ~ exprs,      rhs ~ NULL)
    //       3: apply function     (lhs ~ callable,   rhs ~ expr_tree *)
    //
    //       4: addition            (lhs ~ exprs,     rhs ~ NULL)
    //       6: multiplication      (lhs ~ exprs,     rhs ~ NULL)
    //       5: subtraction         (lhs ~ expr_tree*, rhs ~ expr_tree*)
    //       7: division            (lhs ~ expr_tree*, rhs ~ expr_tree*)
    
    struct node
    {
        char type;
        union {
            index id;
            constant value;
            std::size_t expo;
            list<expr_tree *> *exprs;
            void *ptr;
        } lhs, rhs;
    };

  private:
    struct node root;
    
  public:
    expr_tree() { node.type=-1; };
    expr_tree(const expr_tree &t)
    {  this->copy(t); }


  public:
    void copy(const expr_tree &t)
    {
        root.type = t.root.type;
        switch(t.root.type) {
          case -1:
              break;

          case 0:
              root.lhs.value = t.root.lhs.value;
              root.rhs.ptr   = NULL;
              break;

          case 1:
              root.lhs.id   = t.root.lhs.id;
              root.rhs.expo = t.root.rhs.expo;
              break;

          case 2:
              root.lhs.exprs = new std::list<expr_tree*>();
              for(auto s : *(t.root.lhs.exprs)) {
                  expr_tree *pn;

                  pn = new expr_tree();
                  pn->copy(*s);

                  root.lhs.exprs.push_back(pn);
              }
              break;
        }

    }
};

};

};



#endif /* EXPRESSION_H */


