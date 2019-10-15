// -*- C++ -*- C forwarding header.

#ifndef VARIABLE_H
#define VARIABLE_H

#include <stdexcept>
#include <iostream>

#include "diff.h"
#include "util.h"

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
    bool held_constant;

  public: // constructors
    variable() { held_constant = true; };
    variable(num_type val)
    {   held_constant = false;
        (*this) = val; };
    
    variable(const state_vec &val)
    {   auto i = 0;       
        for(; i <= N; i++)
            value[i] = val[i];
        held_constant = false;  };

    variable(const variable &val)
    {   auto i = 0;
        for(; i <= N; i++)
            value[i] = val.value[i];
        held_constant = val.held_constant;  }

  public: // assignment/update operators
    variable& operator+=(const variable &r)
    {
        this->value[0] += r.value[0];
        if(!r.held_constant)
            for(auto i = 1; i <= N; i++)
                this->value[i]+=r.value[i];
        held_constant &= r.held_constant;
        return (*this);
    };

    variable& operator*=(const variable &r)
    {   // TODO: Find a better way to do this...
        //    (which doesn't require fucking with already defined operators)
        variable t = (*this)*(r);
        for(auto i = 0; i <= N; i++)
            value[i] = t.value[i];
        return (*this);
    };

    variable& operator=(const variable &r)
    {   auto i = 0;
        for(; i <= N; i++)
            value[i] = r.value[i];
        held_constant = false;
        return (*this);
    };

    variable& operator=(const state_vec &r)
    {   auto i = 0;
        for(; i <= N; i++)
            value[i] = r[i];
        held_constant = false;
        return (*this);
    };
    
    variable& operator=(num_type &val)
    {   auto i = 0;
        value[i++]=val;
        value[i++]=(held_constant ? 0 : 1);
        for(; i <= N; i++)
            value[i]=0;
        return (*this);
    };

  public:
    void as_constant()
    { held_constant = true; };

    void as_variable()
    {   auto i = 1;
        value[i++] = 1;
        for(; i <= N; i++)
            value[i] = 0;
        held_constant = false;
    };

    void clear()
    {   auto i = 0;
        for(;i <= N; i++)
            value[i] = 0; }


  public: //friends
    template<std::size_t N0, typename numtype>
    friend std::ostream &operator<<(std::ostream &os, const variable<N0, numtype> &);
    
    template<std::size_t N0, typename numtype>
    friend variable<N0, numtype> operator *(const variable<N0, numtype> &lhs, const variable<N0, numtype> &rhs);
    
    template<std::size_t N0, typename numtype>
    friend variable<N0, numtype> operator +(const variable<N0, numtype> &lhs, const variable<N0, numtype> &rhs);

};


// ostream hook ////////////////////////////////////////////////////////////////

template<std::size_t N0, typename numtype>
std::ostream &operator<<(std::ostream &os, const variable<N0, numtype> &x)
{
    bool first = true;
    os << "[";
    for(auto i = 0; i <= N0; i++) {
        if(!first)
            os << ", ";
        else
            first = false;
        os << std::fixed << std::setprecision(5) << x.value[i];
    }
    os << "]";
    if(x.held_constant)
        os << "(const)";
    return os;
}

// Variable Operator Overloads /////////////////////////////////////////////////
#define __variable variable<N, num_type>

//  multiplication /////////////////////////////////////////////////////////////
template<std::size_t N, typename num_type = constant>
__variable operator*(const __variable &lhs, const __variable &rhs)
{
    __variable ext;
    if(lhs.held_constant && !(rhs.held_constant)) {
        for(auto i=0; i<=N; i++)
            ext.value[i] = rhs.value[i] * lhs.value[0];
        ext.held_constant = false;
        return ext;
    }
    else if(!(lhs.held_constant) && rhs.held_constant) {
        for(auto i=0; i<=N; i++)
            ext.value[i] = rhs.value[0] * lhs.value[i];
        ext.held_constant = false;
        return ext;
    }
    else if(lhs.held_constant && rhs.held_constant) {
        ext = (lhs.value[0])*(rhs.value[0]);
        return ext;
    }
    else {
        num_type c1[N+3], c0[N+3],
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
        
        ext.held_constant =false;
        return ext;
    }
}

// left & right scalar multiplication //////////////////////////////////////////
template<std::size_t N, typename num_type = constant>                   
__variable operator *(const __variable &lhs, const num_type &rhs)      
{
    __variable agg;        
    if(lhs.held_constant) {
        agg = (lhs.value[0])*(rhs);
        return agg;
    }
    else {
        for(auto i = 0; i <= N; i++)                                        
            agg.value[i] = (lhs.value[i]) * (rhs);
        agg.held_constant = false;
        return agg;
    }
}

template<std::size_t N, typename num_type = constant>                   
__variable operator *(const num_type &lhs, const __variable &rhs)      
{
    __variable agg;
    if(rhs.held_constant) {
        agg = (rhs.value[0])*(lhs);
        return agg;
    }
    else {
        for(auto i = 0; i <= N; i++)                                        
            agg.value[i] = (lhs) * (rhs.value[i]);
        agg.held_constant = false;
        return agg;
    }
}

// component-wise addition /////////////////////////////////////////////////////
template<std::size_t N, typename num_type = constant>                   
__variable operator +(const __variable &lhs, const __variable &rhs)
{
    if(lhs.held_constant && !rhs.held_constant) 
        return (lhs.value[0]) + (rhs);
    else if(!lhs.held_constant && rhs.held_constant)
        return (lhs) + (rhs.value[0]);
    else if(lhs.held_constant && rhs.held_constant) {
        __variable agg;
        agg = lhs.value[0] + rhs.value[0];
        return agg;
    }
    else {
        __variable agg;
        for(auto i = 0; i <= N; i++)
            agg.value[i] = (lhs.value[i]) + (rhs.value[i]);
        agg.held_constant = false;
        return agg;
    }
}


template<std::size_t N, typename num_type = constant>                   
__variable operator +(const num_type &lhs, const __variable &rhs)
{
    __variable agg(rhs);
    agg.value[0] += lhs;
    return agg;
}


template<std::size_t N, typename num_type = constant>                   
__variable operator +(const __variable &lhs, const num_type &rhs)
{
    __variable agg(lhs.value);
    agg.value[0] += rhs;
    return agg;
}

// compoentwise subtraction ////////////////////////////////////////////////////
template<std::size_t N, typename num_type = constant>                   
__variable operator -(const __variable &lhs, const __variable &rhs)
{
    if(lhs.held_constant && !rhs.held_constant) {
        __variable agg(rhs);
        agg.value[0] -= lhs.value[0];
        for( auto i = 0; i <= N; i++ )
            agg.value[i] *= (-1);
        return agg;
    }
    else if(!lhs.held_constant && rhs.held_constant) {
        __variable agg(lhs);
        agg.value[0] -= rhs.value[0];
        return agg;
    }
    else if(lhs.held_constant && rhs.held_constant) {
        __variable agg(0);
        agg.value[0] = lhs.value[0] - rhs.value[0];
        agg.value[1] = 0;
        agg.held_constant = true;
        return agg;
    }
    else {
        __variable agg;
        for(auto i = 0; i <= N; i++)
            agg.value[i] = (lhs.value[i]) - (rhs.value[i]);
        agg.held_constant = false;
        return agg;
    }
}

template<std::size_t N, typename num_type = constant>                   
__variable operator -(const num_type &lhs, const __variable &rhs)
{
    if(rhs.held_constant) {
        __variable agg;
        agg = lhs - rhs.value[0];
        return agg;
    }
    else {
        __variable agg(rhs);
        agg.value[0] = lhs - agg.value[0];
        for(auto i = 1; i <= N; i++ )
            agg.value[i] *= (-1);
        return agg;
    }
    
}

template<std::size_t N, typename num_type = constant>                   
__variable operator -(const __variable &lhs, const num_type &rhs)
{
    if(lhs.held_constant) {
        __variable agg;
        agg = lhs.value[0] - rhs;
        return agg;
    }
    else {
        __variable agg(lhs);
        agg.value[0] -= rhs;
        return agg;
    }
}

};

#endif /* VARIABLE_H */
