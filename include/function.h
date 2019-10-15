// -*- C++ -*- C forwarding header.

#ifndef FUNCTION_H
#define FUNCTION_H

#include <map>
#include <cmath>

#include "diff.h"
#include "variable.h"
#include "util.h"

// EXPERIMENTAL ////////////////////////////////////////////////////////////////

namespace diff {
namespace function {
// /////////////////////////////////////////////////////////////////////////////

class generic
{
  public: //constructors/deconstructors
    generic() {};
    virtual ~generic() {};
    
  public:
    // //// virtual interface //////////////////////////////////////////////////
    
    // store `val`, setup environment to compute derivatives, and return value of
    //     function at `val`
    virtual constant begin(constant val) {return val;};

    // nth call to `next` returns D[f, {x,n}] evaluated at `val`
    virtual constant next() {return 0;};

    // tear down anything used to compute `next`
    virtual void end() {};
};

// /////////////////////////////////////////////////////////////////////////////

typedef std::map<int, std::size_t> term;
typedef std::map<term, std::size_t> ps_expr;
typedef std::map<std::size_t, ps_expr> cps_expr;

// holds encoded versions of the general composition rule
extern std::map<int, cps_expr> comp_formula;

// Initialize Function Routines
void init(std::size_t N);

// /////////////////////////////////////////////////////////////////////////////

// computes and returns `f(x)` along with all the associated derivatives
//     demanded by `N`
template<std::size_t N, class num_type = constant>
variable<N, num_type> apply(generic *f, variable<N, num_type> &x) 
{
    debug::begin("apply");

    std::size_t n, m;
    variable<N, num_type> ext;
    num_type fval[N+1];
    num_type *val = &(x.value[0]);
    constant r0,r1,r2;
    
    // /////////////////////////////////////////////////////////////////////////
    // Begin building `ext`;

    //   let m denote the highest order for wich we have
    //      requested the value f^(m)(x.value[0]) from `f`.

    ext.held_constant = false;
    
    // /////////////////////////////////////////////////////////////////////////
    // For m < 5:
    //     use hardcoded formulas
    
    fval[0] = f->begin(x.value[0]);   // signal to f we are about to begin &
    ext.value[0] = (fval[0]);         // store value of f at x.value[0].

    if(N == 0) goto done;

    fval[1] = f->next();               // get f^(1)(x.value[0])
    ext.value[1] = (fval[1])*(val[1]); // compute first der.

    if(N == 1) goto done;

    fval[2] = f->next();               // get f^(2)(x.value[0])
    ext.value[2] = (fval[1])*(val[2])  // compute second der.
                 + (fval[2])*(val[1])*(val[1]); 

    if(N == 2) goto done;

    fval[3] = f->next();               // get f^(3)(x.value[0])
    ext.value[3] = (fval[1])*(val[3])  // compute third der.
                 + (fval[2])*3*(val[1])*(val[2])
                 + (fval[3])*(val[1])*(val[1])*(val[1]);

    if(N == 3) goto done;
    
    fval[4] = f->next();               // get f^(4)(x.value[0])
    ext.value[4] = (fval[1])*(val[4])  // compute fourth der.
                 + (fval[2])*((4*(val[1])*(val[3]))+(3*(val[2])*(val[2])))
                 + (fval[3])*6*(val[1])*(val[1])*(val[2])
                 + (fval[4])*(val[1])*(val[1])*(val[1])*(val[1]);

    if(N == 4) goto done;

    // /////////////////////////////////////////////////////////////////////////
    // For m >= 5:
    //     we use dynamically generated formulas calculated by `init`
    //
    for( m = 5; m <= N; m++ )
    {
        // Compute m'th derivative: ////////////////////////////////////////////
        debug::log("computing formula");
        debug::dump("m", m);
        fval[m] = f->next();           // get f^(m)(x.value[0])

        // the formulas are encoded as terms & coeffs, with terms'th
        //     grouped by their `f^(k)` factor;
        
        ext.value[m] = 0;

        // loop over terms and add contribution to `ext`
        for(auto t0 : comp_formula[m]) {
            debug::log("computing derivative");
            // compute coefficient of fval[t0.first]
            r0 = 0;
            
            // loop over coefficent terms adding to `r0`
            for(auto t1 : t0.second) {
                // compute product term's value
                debug::dump("term-coeff", t1.second);
                
                // initialize term with coefficient
                r1 = t1.second;
                // loop over factors
                for(auto p : t1.first) {
                    n = p.second;          // exponent
                    r2 = x.value[p.first]; // base

                    // add in multiplicative contribution
                    while(n > 0) {
                        if(n & 1)
                            r1 *= r2;
                        r2 *= r2;
                        n>>=1;
                    }
                }
                // add term's value to coefficient
                r0 += r1;
            }
            // add fval contribution to total
            ext.value[m] += (fval[t0.first])*(r0);
        }
    }
    
  done:
    debug::end("apply");
    return ext;
};

// sigmoid = exp(-log(1+exp(-x)))

// /////////////////////////////////////////////////////////////////////////////

// exponential
class exp : public generic
{ 
  private:
    constant x; // value
    constant d; // function/derivative at x
  public:
    ~exp() {};
    constant begin(constant value)
    {
        x = value;
        d = ::exp(x);
        return d;
    };
    constant next()
    {
        return d;
    };
    void end() { };
};

// natural log
class log : public generic
{
  private:
    constant x, n, s; // value, order, and sign
    constant d0, d1; // function/derivative at x
    
  public:
    ~log() {};
    constant begin(constant value)
    {
        x = value;
        n = 1;
        s = 1;
        d0 = 1/x;
        return ::log(x);
    };    
    constant next()
    {
        d1 = d0;
        if(n>1)
            s *= (-1);
        d0 *= n/x;
        n += 1;
        return s*d1;
    };    
    void end() { };
};

// cosine
class cos : public generic
{
  private:
    constant s, d0, d1, d2;

  public:
    ~cos() {};
    
    constant begin(constant value)
    {
        s = 1;
        d0 = ::cos(value);
        d1 = ::sin(value);
        return d0;
    };
    constant next()
    {
        s*=(-1);
        //swap d0/d1
        d2 = d1; d1 = d0; d0 =d2;
        return s*d0;
    };
    void end() { };
};

// sine
class sin : public generic
{
  private:
    constant s, d0, d1, d2;

  public:
    ~sin() { };
    
    constant begin(constant value)
    {
        s = (-1);
        d0 = ::cos(value);
        d1 = ::sin(value);
        return d1;
    };    
    constant next()
    {
        s *= (-1);
        //swap d0/d1
        d2 = d1; d1 = d0; d0 =d2;
        return s*d1;
    };
    void end() { };

};

}; // function
}; // diff

#endif /* FUNCTION_H */
