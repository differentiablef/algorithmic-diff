#include <string>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include "symbolic.h"


#include <iostream>
using std::cout;
using std::endl;

#define DEBUG(x) \
    cout << "DEBUG: " << (#x) << " = " << (x) << endl;

namespace diff {
namespace sym {

// ostream '<<' extension for expression class
ostream &operator<<(ostream &os, const expression &a)
{
    std::string prod_sep = "*",
        sum_sep = " + ",
        sum_par = "()"; 
    
    switch(a.type)
    {
        //base cases
      case types::Empty:
          return os;
          
      case types::Constant:
          os << a.desc.value;
          return os;
          
      case types::Variable:
          os << (char)('a' + a.desc.id);// << a.desc.id;
          return os;

          //recursion
      case types::Sum:
      {
          bool not_first = false;
          cout << sum_par[0];
          for(auto p : a.child)
          {   
              if(not_first)
                  os << sum_sep;
              else
                  not_first = true;
              
              if(p->weight != 1)
                  os << p->weight << prod_sep;
              
              os << (*p); // recursive call
          }
          os << sum_par[1];
          return os;
      }
      
      case types::Product:
      {
          bool not_first = false;
          for(auto p : a.child)
          {
              if(not_first)
                  os << prod_sep;
              else
                  not_first = true;
              os << (*p); // recursive call
              if(p->weight != 1)
                  cout << "^" << p->weight;

          }
          return os;
      }
          
      case types::Tuple:
      case types::Apply:
          break;

    }

    return os;
}

// recursive compare expressions
bool operator<(const expression &le, const expression &re)
{
    
    if(le.type < re.type)
        return true;    
    if(le.type > re.type)
        return false;

    switch(le.type)
    {
      case types::Empty:
          return false;
              
      case types::Constant:
          return (le.desc.value < re.desc.value);
          
      case types::Variable:
          if((le.desc.id < re.desc.id) || \
             ((le.desc.id == re.desc.id) && (le.weight < re.weight)))
              return true;
          return false;
    
      case types::Apply:
      {
          auto n = strcmp(le.desc.name, re.desc.name);
          if(n != 0)
              return (n<0); 
      }
      case types::Tuple:
      case types::Sum:
      case types::Product:
      {
          bool tvalue, rvalue;
          auto m = std::min( le.child.size(), re.child.size() );          
          for(auto i=0; i < m; i++)
          {
              tvalue = *(le.child[i]) < *(re.child[i]); // recursion
              if(!tvalue) {
                  rvalue = *(re.child[i]) < *(le.child[i]); // recursion
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
          throw std::domain_error("expression type no found"); 
    }
    return false;
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

      case types::Apply:
      case types::Sum:
      case types::Product:
      case types::Tuple:
          for(auto p : this->child) {
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
    expression *p;
    
    // copy type and expo
    this->type   = t.type;
    this->weight = t.weight;
    this->desc   = t.desc;
   
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
          for(auto p : t.child)
              this->child.push_back(new expression(*p));
          break;
        
      default:
          break;
    }
}

// preform e*(*this) or e+(*this); collecting like terms and factors
void expression::absorb(expression &e)
{

    if(this->type != types::Sum && \
       this->type != types::Product)
        throw std::logic_error("not sum or product");

    
    if(this->type == e.type)
    {
        for(auto p : e.child)
            this->absorb(*p);

        return;
    }
    
    auto p = this->child.begin();

    if(e.type == types::Product)
        if(e.child.size() > 0)
            if(e.child[0]->type == types::Constant)
            {
                e.weight *= e.child[0]->desc.value;
                e.child.erase(e.child.begin());
            }
    
    for(; p!= this->child.end(); p++)
    {
        if((*p)->type < e.type)
            continue;

        if((*p)->type == e.type) {
            switch(e.type)
            {
              case types::Constant:
                  if( this->type == types::Sum )
                      (*p)->desc.value += e.desc.value;
                  else
                      (*p)->desc.value *= e.desc.value;
                  return;
                  
              case types::Variable:
                  if((*p)->desc.id == e.desc.id) {
                      (*p)->weight += e.weight;
                      return;
                  }

                  if((*p)->desc.id > e.desc.id) {
                      this->child.insert(p, new expression(e));
                      return;
                  }
                  break;
                  
              case types::Apply:
              case types::Sum:
                  if(!(e < *(*p))) {
                      if(!(*(*p) < e))
                          (*p)->weight += e.weight;
                      else
                          this->child.insert(p, new expression(e));
                     
                      return;
                  } break;

              case types::Product:
                  if(!(e < *(*p))) {
                      if(!(*(*p) < e))
                          (*p)->weight += e.weight;
                      else
                          this->child.insert(p, new expression(e));
                     
                      return;
                  }
            }
            continue;
        }

        break;
    }

    this->child.insert(p, new expression(e));

}

}; // symbolic
}; // diff


