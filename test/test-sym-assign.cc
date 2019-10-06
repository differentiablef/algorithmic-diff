
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
    poly0 += c;
    for(auto i = 0; i < N0; i++)
    {
        ;
        DEBUG(poly0);
        DEBUG(xpow);
        xpow *= x;
        poly0 += xpow;
        DEBUG(poly0);
        DEBUG(xpow);
    }
    DEBUG(poly0);
    
    x.set_id(2);
    xpow.clear();
    poly1 += c;
    for(auto i = 0; i < N1; i++)
    {
        xpow *= x;
        poly1 += xpow;
    }
    DEBUG(poly1);
    
    for(auto p0 : poly0.child)
        for(auto p1 : poly1.child)
            for(auto p2 : poly0.child)
            {
                auto a = {*p0, *p1, *p2};
                DEBUG(a);
                
                tmp.clear();
                for(auto t : a)
                    tmp *= t;
                
                result += tmp;
                DEBUG(result);

            }


    result.assign(x, c);
    DEBUG(result);
}
