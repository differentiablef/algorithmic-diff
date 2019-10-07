
#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <algorithm>

#include "util.h"
#include "symbolic.h"

namespace diff {
namespace sym {

// formatting related

const char *var_sym     = "v",   *var_par = "{}";
const char *tuple_sep   = ", ",  *tuple_par="[]";
const char *sum_sym     = " + ", *sum_par="()";
const char *product_sym = "*",   *product_par="()";
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
          os << product_par[0];
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
          os << product_par[1];
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
          if( le.weight < re.weight )   return true;
          return false;

      case types::Apply:
      case types::Tuple:
      case types::Sum:
      case types::Product:
      {   // compare using lex order on children
          bool tvalue, rvalue;
          auto m = std::min( le.child.size(), re.child.size() );
          for(auto i=0; i < m; i++)
          {
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
          return (le.child.size() < re.child.size());
      }
      
      default:
          throw std::runtime_error("expression type not found"); 
    }
    return false;
}

// sets type to types::Constant and sets `desc.value` equal to `val`
void expression::set_value(constant val)
{
    type = types::Constant;
    desc.value = val;
    child.clear();
}

constant expression::get_value()
{
    if(type != types::Constant)
        throw std::runtime_error("non-constant type has no value");
    return desc.value;
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

expression &expression::get_arg(index n)
{
    if(type != types::Apply && type != types::Tuple)
        throw std::runtime_error("cannot get arguments of non-tuple types");
    if(n > child.size())
        throw std::out_of_range("argument slot # out of range");
    if(child[n] == NULL)
        throw std::runtime_error("argument not assigned a value");
    return *(child[n]);
}

expression expression::at(std::initializer_list<expression*> il)
{
    expression fval;
    
    if(this->type != types::Apply)
        throw std::logic_error("wrong type");
    
    fval.name = name;
    fval.type = type;
    fval.desc = desc;
        
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
          for(auto p : child)
          {
              p->clear();
              delete p;
          }
          child.clear();
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
    type   = t.type;
    weight = t.weight;
    desc   = t.desc;
    name   = t.name;
    copied = true;
    
    switch(type)
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
expression &expression::absorb(const expression &e)
{
    if( type != types::Sum && \
        type != types::Product ) throw std::runtime_error("not sum or product");

    debug::begin("absorb");
    
    if( type == e.type )
    {
        for(auto p : e.child) {
            debug::tag("child");
            this->absorb(*p);
        }
        
        debug::end("children");
        return (*this);
    }

    expression *q = new expression(e);
    if( q->type == types::Product )
    {
        if( q->child.size() == 0 ) {
            // empty products are taken to be `1`
            q->set_value(1); // makes q a types::Constant with value `1`;
            debug::log("replaced empty product with 1");
        } else if( q->child[0]->type == types::Constant ) {
            if( q->child.size() == 1 ) {
                constant eval;
                // store eval'd value of q
                eval = pow(q->child[0]->get_value(), q->child[0]->weight);

                // clear q's contents
                delete q->child[0];
                q->child.clear();

                // make q constant with value `t`
                // q->weight = 1;
                
                q->set_value(eval);
                debug::log("set value");
                debug::dump("weight", q->weight);
                debug::dump("value", q->get_value());

            }
            else {
                debug::log("combine constant & weight");
                q->weight *= pow(q->child[0]->get_value(), q->child[0]->weight);
                delete q->child[0];
                q->child.erase(q->child.begin());
                debug::dump("weight", q->weight);
            }
        }
    }

    if( q->type == types::Sum )
    {
        if( q->child.size() == 0 ) {
            // empty sums are taken to be `0`
            q->set_value(0); // makes q a types::Constant with value `0`;
            debug::log("replaced empty sum with 0");
        } else if( q->child.size() == 1 ) {
            if( q->child[0]->type == types::Constant ) {
                constant t;
                // store eval'd value of q
                t = (q->child[0]->get_value())*(q->child[0]->weight);

                // clear q's contents
                delete q->child[0];
                q->child.clear();

                // make q constant with value `t`
                //q->weight = 1;
                q->set_value(t);
                debug::log("set value");
                debug::dump("weight", q->weight);
                debug::dump("value", q->get_value());
            }
        }
    }

    if(this->child.size() == 0)
    {
        this->child.push_back(q);
        debug::end("first insert");
        return (*this);
    }
    
    // TODO: make this use binary-search    
    for(auto p=child.begin(); p!=child.end(); p++)
    {
        if( q->type > (*p)->type ) {
            debug::tag("type >");
            debug::log("continue");
            continue;
        }
        else if( q->type < (*p)->type ) {
            debug::tag("type <");
            
            child.insert(p,q);
            debug::end("insert");
            return (*this);
        }
        else if( q->type == (*p)->type ) {
            debug::tag("type =");
            switch(q->type) {
              case types::Constant: {
                  debug::tag("constant");
                  
                  if( type == types::Sum ) {
                      (*p)->desc.value+=(q->desc.value)*(q->weight);
                  } else {
                      (*p)->desc.value*=pow(q->desc.value, q->weight);
                  }
                  delete q;
                  debug::end("combined weights");
                  return (*this);
              }
              
              case types::Variable: {
                  debug::tag("variable");
                  const char *p0 = ( q->name == NULL ? var_sym : q->name ),
                             *p1 = ( (*p)->name == NULL ? var_sym : (*p)->name );

                  if( q->desc.id >  (*p)->desc.id ) {
                      debug::tag("id >");
                      debug::log("continue");
                      continue;
                  }
                  else if( q->desc.id == (*p)->desc.id ) {
                      debug::tag("id =");
                      auto n = strcmp(p0, p1);
                      if( n > 0 ) {
                          debug::tag("name >");
                          debug::log("continue");
                          continue;
                      }
                      else if( n == 0 ) {
                          debug::tag("name =");
                          (*p)->weight += q->weight;
                          delete q;
                          debug::end("combined weights");
                      }
                      else {
                          debug::tag("name <");
                          child.insert(p, q);
                          debug::end("insert");
                      }
                  }
                  else {
                      debug::tag("id <");
                      child.insert(p,q);
                      debug::end("insert");
                  }
                  return (*this);
              }

              case types::Tuple:
              case types::Apply:
              case types::Sum:
              case types::Product:
                  debug::tag("compound");
                  if(!((*q) < *(*p))) {
                      
                      if(!(*(*p) < (*q))) {
                          debug::tag("lex =");
                          
                          (*p)->weight += q->weight;
                          delete q;
                          debug::end("combined weights");
                      }
                      else {
                          debug::tag("lex >");
                          debug::log("continue");
                          continue;
                      }
                  }
                  else {
                      debug::tag("lex <");
                      child.insert(p,q);
                      debug::end("insert");
                  }
                  return (*this);
            }
        }
    }

    child.push_back(q);
    debug::end("push");
    return (*this);
}


// assign all leafs matching variable `var` with `expr`
bool expression::assign(expression &var, expression &expr)
{
    bool ret = false;
    if(var.type != types::Variable)
        throw std::runtime_error("cannot assign to non-variable type");

    debug::begin("assign");
    switch(type)
    {
      case types::Empty:
      case types::Constant:
          debug::end("no change");
          return false;

      case types::Variable:
      {
          debug::tag("variable");
          const char *p0 = (var.name == NULL ? var_sym : var.name),
              *p1 = (this->name == NULL ? var_sym : this->name);
        
          if((strcmp(p0, p1) == 0) && (this->desc.id == var.desc.id))
          {
              constant w = this->weight;
              this->copy(expr);
              this->weight = w;
              debug::end("changed");
              return true;
          }
          debug::end("unchanged");
          return false;
      }

      case types::Tuple:
      case types::Apply:
      {
          for(auto p : child)
              if(p != NULL)
                  ret |= p->assign(var, expr);
          debug::end("");
          return ret;
      }
      case types::Product:
      case types::Sum:
      {
          std::vector<expression*> tmp1;
          std::vector<expression*> tmp0;

          for(auto p : child)
          {
              debug::tag("child");
              if(p->assign(var, expr)) {
                  ret = true;
                  tmp1.push_back(p);
              } else {
                  tmp0.push_back(p);
              }
          }

          this->child = tmp0;
          for(auto q : tmp1)
          {
              debug::tag("update");
              this->absorb(*q);
              q->clear();
              delete q;
              
          }
          debug::end("");

          return ret;
          
         
      }
      default:
          return false;
    }
    
    return ret;
}

}; // symbolic
}; // diff


