
#include <cmath>

#include "diff.h"
#include "variable.h"
#include "function.h"
#include "util.h"

// highest order der.
#define N 15

int main
() {

    // source of these is sympy
    double exp_ans[16] = {0.77880078307140487848, -0.77880078307140487848, -0.77880078307140487848,
                           3.8940039153570245034, 0.77880078307140487848, -31.930832105927599684,
                           24.142824275213552454, 359.02716099591765442, -697.02670084890735325,
                           -5047.4078750857752311, 17593.888490366109181, 83354.269011349388165,
                           -470419.81579940376105, -1530082.6404729816131, 13760997.851257478818,
                           29081316.081986006349};

    double log_ans[16] = {0.80000000000000004441, -0.64000000000000001332, -0.25599999999999994982,
                           3.6863999999999998991,  -9.3388800000000049550, -21.626879999999996329,
                           327.94214400000021215, -1109.8128384000010556, -7589.0058854400012933,
                           118566.06186700811668,  -402120.07118438737234, -6895156.8178741205484,
                           108657384.96866290271, -269521232.80340296030,  -12801877444.038904190,
                           198902096439.44113159};
    
    diff::variable<N> x(0.5), fval;
    debug::begin("composition test");

    // initialize function routines
    debug::log("initialize");
    diff::function::init( N );

    // /////////////////////////////////////////////////////////////////////////
    debug::log("exp(-x^2) test");

    // non-algebraic function container
    diff::function::exp f;
    
    x = 0.5;
    debug::dump("x", x);

    x = (-1.0) * x * x;
    debug::dump("y", x);
   
    fval = diff::function::apply(&f, x);
    debug::dump("f", fval);

    diff::constant norm2 = 0, norminf=0, r0;
    for(auto i = 0; i <= N; i++) {
        r0 = fabs(exp_ans[i]-fval.value[i]);
        norm2 += r0*r0;
        norminf = (norminf < r0? r0 : norminf);
    }
    norm2 = ::sqrt(norm2);
    debug::dump("  2-norm", norm2);
    debug::dump("inf-norm", norminf);

    // /////////////////////////////////////////////////////////////////////////
    debug::log("exp( -log(1+x^2) ) test");

    // non-algebraic function container
    diff::function::log g;
    
    x = 0.5;
    debug::dump("x", x);

    x = x*x + 1.0;
    debug::dump("y0", x);
    
    fval = diff::function::apply(&g, x);
    debug::dump("g", fval);

    fval = (-1.0) * fval;
    fval = diff::function::apply(&f, fval);
    debug::dump("f", fval);

    norm2 = 0; norminf=0;
    for(auto i = 0; i <= N; i++) {
        r0 = fabs(log_ans[i]-fval.value[i]);
        norm2 += r0*r0;
        norminf = (norminf < r0 ? r0 : norminf);
    }
    norm2 = ::sqrt(norm2);
    debug::dump(" l2-norm", norm2);
    debug::dump("inf-norm", norminf);

    debug::end("composition test");
    return 0;
}
