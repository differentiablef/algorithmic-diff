
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

#define N0 5
#define N1 2
#define N2 4
int  main
() {
    char ch;
    expression result(Sum), tmp(Product);
    expression x, y, tmp0, xpow(Product), poly0(Sum), poly1(Sum), poly2(Sum), c(Constant);

    //  x.set_name("x");
    x.set_id(1);
    c.set_value(1);
    poly0 += c;
    for(auto i = 0; i < N0; i++)
    {
        xpow *= x;
        poly0 += xpow;
    }
    debug::dump("poly0", poly0);
    
    x.set_id(2);
    xpow.clear();
    for(auto i = 0; i <= N1; i++)
    {
        poly1 += xpow;
        xpow *= x;
    }
    debug::dump("poly1", poly1);
    
    x.set_id(3);
    poly2 += c;
    xpow.clear();
    for(auto i = 0; i < N2; i++)
    {
        xpow *= x;
        poly2 += xpow;
    }
    debug::dump("poly2", poly2);
    for(auto p0 : poly0.child)
        for(auto p1 : poly1.child)
            for(auto p2 : poly2.child)
            {
                auto a = {*p0, *p1, *p2};
                debug::dump("params", a);
                
                tmp.clear();
                for(auto t : a)
                    tmp *= t;
                
                result += tmp;
                debug::dump("result", result);

            }

    tmp0.copy(result);//save for later use

    debug::begin("assign {1}=1");
    x.set_id(1);
    result.assign(x, c);
    debug::dump("result", result);
    debug::end("assign {1}");

    debug::begin("assign {2}=2");
    x.set_id(2);
    c.set_value(2);
    result.assign(x, c);
    debug::dump("result", result);
    debug::end("assign {2}=2");

    result.clear();
    result.copy(tmp0); // restore orig val
    debug::begin("assign {2}={1}");
    x.set_id(2);
    y.set_id(1);
    result.assign(x, y);
    debug::dump("result", result);
    debug::end("assign {2}={1}");

}
