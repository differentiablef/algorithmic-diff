
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

int  main
() {
    expression fval;
    expression x, w, z, sum, prod, function, a1, a2, a3;
    
    x.type    = Variable;
    x.name    = "x";
    w.type    = Variable;
    w.name    = "w";
    z.type    = Variable;

    sum.type  = Sum;
    prod.type = Product;
    
    z.type = Constant;
    z.desc.value = 1;

    for(auto i = 0; i < 5; i++ )
    {
        x.desc.id = i;
        w.desc.id = i;
        
        prod.clear();
        prod.absorb(x);
        prod.absorb(w);
        sum.absorb(prod);
    }

    cout << sum << endl;

    function.type  = Apply;
    function.name  = "F";
    
    a1.type = Variable; a1.desc.id = 1;
    a2.type = Variable; a2.desc.id = 2;
    a3.type = Variable; a3.desc.id = 3;
    
    fval = function.at({&a1,&a2,&a3});
    
    DEBUG(fval);
    
    prod.clear();
    prod.absorb(sum);

    a1.copy(sum);
    expression h = function.at({&x,&w});
    a2.copy(prod.absorb(h).absorb(h) );
    a3.copy(sum);


    DEBUG(fval);
    

    
}
