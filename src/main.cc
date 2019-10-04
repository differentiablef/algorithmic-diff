
#include<iostream>
#include<iterator>
#include<array>
#include<map>

#include "variable.h"
#include "symbolic.h"

using std::cout;
using std::endl;

#define DEBUG(x) \
    cout << "DEBUG: " << (#x) << " = " << (x) << endl;

using std::map;

// Testing /////////////////////////////////////////////////////////////////////
#define N 9

using namespace diff::sym;

void dump_expr(expression &a)
{
    switch(a.type)
    {
      case types::Empty:
          return;
          
      case types::Constant:
          cout << a.desc.value;
          return;
          
      case types::Variable:
          cout << "(v" << a.desc.id << ")";
          return;

      case types::Sum: {
          bool not_first = false;
          cout << '[';
          for(auto p : a.child)
          {   
              if(not_first)
                  cout << "+";
              else
                  not_first = true;
              
              if(p->weight != 1)
                  cout << p->weight;
              dump_expr(*p);
          }
          cout << ']';
      } break;
      case types::Product:{
          bool not_first = false;
          for(auto p : a.child)
          {
              if(not_first)
                  cout << "";
              else
                  not_first = true;
              
              dump_expr(*p);

              if(p->weight != 1)
                  cout << "^" << p->weight;

          }
      } break;
          
      case types::Tuple:
      case types::Apply:
          break;

    }
}

int main
() {
    expression *p1;
    expression x, w, z, sum, prod, fun;

    sum.type = Sum;
    prod.type = Product;
    fun.type = Apply;
    fun.desc.name = "F";

    x.type = Variable;
    w.type = Variable;
    z.type = Variable;
    x.desc.id = 3;
    w.desc.id = 4;
    z.desc.id = 5;
    
    sum.absorb(x);
    sum.absorb(w);
    sum.absorb(z);
    
    z.type = Constant;
    z.desc.value = 1;
    for(auto i = 0; i < 3; i++ )
    {
        x.desc.id = (i % 3);
        w.desc.id = (i % 2);
        z.desc.value *= 2;
        
        prod.clear();
        
        prod.absorb(sum);
        prod.absorb(x);
        prod.absorb(w);
        prod.absorb(z);
        sum.absorb(prod);
    }

    z.desc.value = 11;
    sum.absorb(z);

    cout << sum << endl;

    
}
