
#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "util.h"
#include "symbolic.h"

namespace diff {
namespace sym {

// formatting related

const char *var_sym     = "v",   *var_par = "{}";
const char *tuple_sep   = ", ",  *tuple_par="[]";
const char *sum_sym     = " + ", *sum_par="()";
const char *product_sym = "*",   *product_par="";
const char *exponet_sym = "^";


// ostream '<<' extension for expression class
ostream &operator<<(ostream &os, const expression &a)
{
    bool not_first = false;
    switch(a.type)
    {
        //base cases
      case types::Empty:
          return os;
          
      case types::Constant:
          os << a.desc.value;
          return os;
          
      case types::Variable:
          os << var_par[0];
          if(a.name != NULL)
              os << a.name;
          else
              os << ""; //a.name;
          os <<  a.desc.id << var_par[1];
          return os;

          //recursion
      case types::Sum:
      {
          cout << sum_par[0];
          for(auto p : a.child)
          {
              if(not_first)
                  os << sum_sym;
              else
                  not_first = true;
              
              if(p->weight != 1)
                  os << p->weight << product_sym;
              os << (*p); // recursive call
          }
          os << sum_par[1];
          return os;
      }
      
      case types::Product:
      {
          for(auto p : a.child)
          {
              if(not_first)
                  os << product_sym;
              else
                  not_first = true;
              os << (*p); // recursive call
              if(p->weight != 1)
                  cout << exponet_sym << p->weight;

          }
          return os;
      }

      case types::Apply:
          if(a.name != NULL)
              os << a.name;
      case types::Tuple:
      {
          os << tuple_par[0];
          for(auto p : a.child)
          {
              if(not_first)
                  os << tuple_sep;
              else
                  not_first = true;
              os << (*p); // recursive call
          }
          os << tuple_par[1];
          return os;
      }

    }

    return os;
}

// recursively compare expressions
bool operator<(const expression &le, const expression &re)
{
    int n;
    
    // compare types
    if(le.type < re.type) return true;
    if(le.type > re.type) return false;

    // +assumption: le.type == re.type
    // compare names
    if(le.name == NULL && re.name != NULL) return true;
    if(le.name != NULL && re.name == NULL) return false;
    if(le.name != NULL && re.name != NULL) {
        auto n = strcmp(le.name, re.name);
        if(n != 0)
            return (n<0);
    }
    // +assumption: (le.name == re.name == NULL) or
    //                 (they are equivalent strings);
    
    switch(le.type)
    {
      case types::Empty: // empty expressions are considered equivalent
          return false; 
              
      case types::Constant: // constants are compared by value
          return (le.desc.value < re.desc.value);
          
      case types::Variable: // compared using lex order of (name, id, weight)
          if( le.desc.id > re.desc.id ) return false;
          if( le.desc.id < re.desc.id ) return true;
          if( le.weight > re.weight )   return true;
          return false;

      case types::Apply:
      case types::Tuple:
      case types::Sum:
      case types::Product:
      {   // compare using lex order on children
          bool tvalue, rvalue;
          auto m = std::min( le.child.size(), re.child.size() );
          for(auto i=0; i < m; i++) {
              tvalue = *(le.child[i]) < *(re.child[i]); // recursive call
              if(!tvalue) {
                  rvalue = *(re.child[i]) < *(le.child[i]); // recursive call
                  if(!rvalue)
                      continue;
                  else
                      return false;
              } else
                  return true;
          }
          return (le.child.size() > re.child.size());
      }
      
      default:
          throw std::runtime_error("expression type no found"); 
    }
    return false;
}

// sets type to types::Variable and id to n
void expression::set_id(index n)
{
    type = types::Variable;
    desc.id = n;
    weight = 1;
    child.clear();
}

// sets type to types::Apply and allocates enough child slots to set 'n'
void expression::set_arg(index n, expression &e)
{
    type = types::Apply;
    auto m = child.size();
    if(m <= n)
        child.insert(child.end(), 1+n-m, NULL);
    child[n] = &e;
}

expression expression::at(std::initializer_list<expression*> il)
{
    expression fval;
    
    if(this->type != types::Apply)
        throw std::logic_error("wrong type");
    
    fval.name = this->name;
    fval.type = this->type;
    fval.desc = this->desc;
        
    for(auto p : il)
        fval.child.push_back(p);

    return fval;
}


// clear the contents of the expression; deleting as you go
void expression::clear()
{
    switch(this->type)
    {
      case types::Empty:
      case types::Constant:
      case types::Variable:
          break;
          
      case types::Sum:     // class controlled children
      case types::Product: // ...
      case types::Tuple: // user controlled children
      case types::Apply: // ...
          for(auto p : this->child)
          {
              p->clear();
              delete p;
          }
          this->child.clear();
          break;
          
      default:
          break;
    }

    this->weight = 1;
}

// deep copy of expression
void expression::copy(const expression &t)
{
    // copy type and expo
    this->type   = t.type;
    this->weight = t.weight;
    this->desc   = t.desc;
    this->name   = t.name;
    this->copied = true;
    
    switch(this->type)
    {
      case types::Empty:
      case types::Constant:
      case types::Variable:
          break;

      case types::Sum:
      case types::Product:
      case types::Tuple:
      case types::Apply:
          for(auto p : t.child) {
              //auto q = std::make_shared<expression>(*p);
              auto q = new expression(*p);
              this->child.push_back(q);
          }
          break;
        
      default:
          break;
    }
}

// preform e*(*this) or e+(*this); collecting like terms and factors
expression &expression::absorp(const expression &e)
{
    if( type != types::Sum && \
        type != types::Product ) throw std::runtime_error("not sum or product");

    DEBUG(e);
    MARKER("ENTERING ABSORP FUNCTION:");
    if( type == e.type )
    {
        MARKER("equal types: absorping children");
        for(auto p : e.child)
            this->absorb(*p);
        MARKER("EXITING ABSORP FUNCTION");
        return (*this);
    }

    expression *q = new expression(e);
    if( q->type == types::Product )
    {
        if( q->child.size() == 0 )
            throw std::runtime_error("cannot absorp empty product");

        if( q->child[0]->type == types::Constant ) {
            if( q->child.size() == 1 ) {
                MARKER("product constant case: absorping only child");
                this->absorb(*(q->child[0]));
                q->clear();
                delete q;
                MARKER("EXITING ABSORP FUNCTION: product");
                return (*this);
            }
            else {
                MARKER("product constant case: absorping constant into weight");
                q->weight *= q->child[0]->desc.value;
                delete q->child[0];
                q->child.erase(q->child.begin());
            }
        }
    }

    if(child.size() == 0)
    {
        child.push_back(q);
        MARKER("EXITING ABSORP FUNCTION: first child");
        return (*this);
    }

    MARKER("processing children:");
    {
        auto ss = std::lower_bound(
            child.begin(), child.end(), q,
            [](auto a, auto b) {
                return ((*a) < (*b));
            });
        
        DEBUG(*q);
        if(ss != child.end())
            DEBUG(*(*ss));
    }
    auto p=child.begin();
    for(; p!=child.end(); p++)
    {
        if( e.type >  (*p)->type ) {
            MARKER("type > : continue");
            continue;
        }
        else if( e.type <  (*p)->type ) {
            MARKER("type < : insert");
            child.insert(p,q);
            MARKER("EXITING ABSORP FUNCTION: type");
            return (*this);
        }
        else if( e.type == (*p)->type ) {
            MARKER("type = : ");
            switch(e.type) {
              case types::Constant: {
                  if( type == types::Sum )
                      (*p)->desc.value+=q->desc.value;
                  else
                      (*p)->desc.value*=q->desc.value;
                  delete q;
                  MARKER("EXITING ABSORP FUNCTION: constant");
                  return (*this);
              }
              
              case types::Variable: {
                  const char *p0 = ( q->name == NULL ? var_sym : q->name ),
                             *p1 = ( (*p)->name == NULL ? var_sym : (*p)->name );

                  if( q->desc.id >  (*p)->desc.id ) {
                      MARKER("variable: id > : continue");
                      continue;
                  }
                  else if( q->desc.id == (*p)->desc.id ) {
                      auto n = strcmp(p0, p1);
                      if( n > 0 ) {
                          MARKER("variable: id = & name > : continue");
                          continue;
                      }
                      else if( n == 0 ) {
                          MARKER("variable: id = & name = : combined weights");
                          (*p)->weight += q->weight;
                          delete q;
                      }
                      else {
                          MARKER("variable: id = & name < : insert");
                          child.insert(p, q);
                      }
                  }
                  else {
                      MARKER("variable: id < : insert");
                      child.insert(p,q);
                  }
                  
                  MARKER("EXITING ABSORP FUNCTION: variable");
                  return (*this);
              }

              case types::Tuple:
              case types::Apply:
              case types::Sum:
              case types::Product:
                  if(!((*q) < *(*p))) {
                      if(!(*(*p) < (*q))) {
                          (*p)->weight += q->weight;
                          delete q;
                          MARKER("aggregate = : combined weight");
                      }
                      else {
                          MARKER("aggregate > : continue");
                          continue;
                      }
                  }
                  else {
                      MARKER("aggregate < : insert");
                      child.insert(p,q);
                  }
                  MARKER("EXITING ABSORP FUNCTION: aggregate");
                  return (*this);
                    
            }
        } 
    }

    child.push_back(q);
    MARKER("EXITING ABSORP FUNCTION: insert as last child");
    return (*this);
}

}; // symbolic
}; // diff


