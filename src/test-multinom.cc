
#include<iostream>
#include<iterator>
#include<array>
#include<map>

#include "util.h"
#include "variable.h"
#include "symbolic.h"

using std::map;

//  Multinomial Expansion //////////////////////////////////////////////////////

using namespace diff::sym;

#define N0 3
#define N1 3

int  main
() {
    char ch;
    expression result(Sum), tmp(Product);
    expression x, y, xpow(Product), poly0(Sum), poly1(Sum), c(Constant);

    //  x.set_name("x");
    x.set_id(1);

    //c.set_name("1");
    c.desc.value = 1;
    poly0.absorp(c);
    for(auto i = 0; i < N0; i++)
    {
        xpow.absorp(x);
        poly0.absorp(xpow);
    }
    DEBUG(poly0);
    
    x.set_id(2);
    xpow.clear();
    poly1.absorp(c);
    for(auto i = 0; i < N1; i++)
    {
        xpow.absorp(x);
        poly1.absorp(xpow);
    }
    DEBUG(poly1);
    
    for(auto p0 : poly0.child)
        for(auto p1 : poly1.child)
            for(auto p2 : poly0.child)
            {
                auto a = {*p0, *p1, *p2};
                DEBUG(a);
                tmp.clear();
                tmp.absorp(*p0).absorp(*p1).absorp(*p2);
                result.absorp(tmp);
                DEBUG(result);

            }

    
/*
    function.type  = Apply;
    function.name  = "F";
    
    a1.type = Variable; a1.desc.id = 1;
    a2.type = Variable; a2.desc.id = 2;
    a3.type = Variable; a3.desc.id = 3;
    
    // fval = function.at({&a1,&a2,&a3});
    
    DEBUG(fval);
    
    prod.clear();
    prod.absorb(sum);

    a1.copy(sum);
    expression h = function.at({&x,&w});
    a2.copy(prod.absorb(h).absorb(h) );
    a3.copy(sum);


    DEBUG(fval);
*/  

    
}
