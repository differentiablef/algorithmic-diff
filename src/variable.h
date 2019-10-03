// -*- C++ -*- C forwarding header.


#ifndef VARIABLE_H
#define VARIABLE_H

#include "diff.h"

namespace diff
{

// Variable ////////////////////////////////////////////////////////////////////

template<std::size_t N = 3, typename num_type = constant>
class variable
{
  public:
    typedef num_type state_vec[N+1];

  public: // public variables
    state_vec value;

  public: // constructors
    variable() {};
    variable(num_type val)
    {   (*this) = val;    };
    
    variable(state_vec &val)
    {   auto i = 0;
        for(; i <= N; i++)
            value[i] = val[i];
    };

  public: // assignment operators
    variable& operator=(const variable &r)
    {   auto i = 0;
        for(; i <= N; i++)
            this->value[i] = r.value[i];
        return (*this);
    };

    variable& operator=(const state_vec &r)
    {   auto i = 0;
        for(; i <= N; i++)
            this->value[i] = r[i];
        return (*this);
    };
    
    variable& operator=(num_type &val)
    {   auto i = 0;
        this->value[i++]=val;
        this->value[i++]=1;
        for(; i < N; i++)
            this->value[i]=0;
        return (*this);
    };

};

// Variable Operator Overloads /////////////////////////////////////////////////

#define __variable variable<N, num_type>

//  multiplication
template<std::size_t N, typename num_type = constant>
__variable operator*(const __variable &lhs, const __variable &rhs)
{
    __variable ext;
    unsigned long c1[N+3], c0[N+3],
        *p0=&c0[1], *p1=&c1[1], *t;

    // compute n'th value of ext using binomial-style
    //          formula for D[rhs * lhs, n]    
    for(auto n = 0; n <= N; n++)
    {   // initialize boundary coefficients
        p0[n]   = 1;
        p0[n+1] = p0[-1] = 0;

        // compute `ext.value[n]`
        ext.value[n] = 0; 
        for(auto i = 0; i <= n; i++)
        {   // add contribution from `D[rhs, n-i]*D[lhs, i]` term
            ext.value[n]+=
                (p0[i])*(lhs.value[n-i])*(rhs.value[i]);
            
            // calculate coeff. for next pass
            p1[i] = p0[i] + p0[i-1];
        }
        
        // swap p0 and p1
        t  = p0; p0 = p1; p1 = t;
    }
    
    return ext;
}

#define DIFF_COMPONENTWISE_OP(op)                                       \
    template<std::size_t N, typename num_type = constant>               \
    __variable operator op(const __variable &lhs, const __variable &rhs) \
    {   __variable agg;                                                 \
        for(auto i = 0; i <= N; i++)                                    \
            agg.value[i] = (lhs.value[i]) op (rhs.value[i]);            \
        return agg; }

#define DIFF_RIGHT_SCALAR_OP(op)                                        \
    template<std::size_t N, typename num_type = constant>               \
    __variable operator op(const __variable &lhs, const num_type &rhs)  \
    {   __variable agg;                                                 \
        for(auto i = 0; i <= N; i++)                                    \
            agg.value[i] = (lhs.value[i]) op (rhs);                     \
        return agg; }

#define DIFF_LEFT_SCALAR_OP(op)                                         \
    template<std::size_t N, typename num_type = constant>               \
    __variable operator op(const num_type &lhs, const __variable &rhs)  \
    {   __variable agg;                                                 \
        for(auto i = 0; i <= N; i++)                                    \
            agg.value[i] = (lhs) op (rhs.value[i]);                     \
        return agg; }

// component-wise addition/subtraction
DIFF_COMPONENTWISE_OP(+);
DIFF_COMPONENTWISE_OP(-);

// scalar multiplication/division
DIFF_LEFT_SCALAR_OP(*);
DIFF_RIGHT_SCALAR_OP(*);
DIFF_RIGHT_SCALAR_OP(/);

};

#endif /* VARIABLE_H */
