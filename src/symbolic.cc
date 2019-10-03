
#include "symbolic.h"

namespace diff {
namespace symbolic {

void expression::clear()
{
    switch(root.type) {
      case Empty:
      case Constant:
      case Variable:
          break;

      case Apply:
          auto *p = (expression*)root.rhs.ptr;
          p->clear();
          delete p;
          break;

      case Sum:
      case Product:
      case Tuple:
          for(auto p : *(root.lhs.exprs)) {
              p->clear();
              delete p;
          }
          delete root.lhs.exprs;
          break;
          
    }
}

void expression::copy(const expression &t) {
    expression *p;
    
    // copy root-type
    root.type = t.root.type;
    
    // copy contents of lhs & rhs
    switch(root.type) {
      case Empty:
          break;
          
      case Constant:
          root.lhs.value = t.root.lhs.value;
          root.rhs.ptr   = NULL;
          break;
          
      case Variable:
          root.lhs.id = t.root.lhs.id; // variable id
          root.rhs.id = t.root.rhs.id; // exponent
          break;

      case Sum:
      case Product:
      case Tuple:
          root.lhs.exprs =
              new std::list<expression*>();
          
          for(auto s : *(t.root.lhs.exprs)){
              p = new expression();
              p->copy(*s);
              
              root.lhs.exprs->push_back(p);
          }
          break;
              
      case Apply:
          root.lhs.ptr = t.root.lhs.ptr;
          
          p = new expression();
          p->copy(*((expression*)t.root.rhs.ptr));
          root.rhs.ptr = p;              
          break;

      default:
          break;
    }
}

}; // symbolic
}; // diff
