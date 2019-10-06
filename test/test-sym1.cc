
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

//  NN Example /////////////////////////////////////////////////////////////////

using namespace diff::sym;

#define N 4
#define M 3

int  main
() {
    expression prod(Product);
    expression F[N][M+1];
    expression x[N], w[N][N][M], y[N][M], b[N][M];


    for(auto i = 0; i < N; i++)
    {
        x[i].set_name("x");
        x[i].set_id(i);
        
        F[i][0].set_name("F");
        F[i][0].set_arg(0, x[i]);
    }

    // layers
    for(auto k = 1; k < M; k++) 
    {
        // input:  < F[i][k-1]: i=0...N >
        // output: < F[i][k]:   i=0...N >
        for(auto i = 0; i < N; i++)
        {
            // activation function
            F[i][k].set_name("F");
            
            // bias term
            b[i][k].set_name("b");
            b[i][k].set_id(k + 10*i);

            // inner product term
            y[i][k].set_name("y");
            y[i][k].type = Sum;
            y[i][k].absorp(b[i][k]);
            for(auto j = 0; j < N; j++)
            {
                // entry in weight 'tensor'
                w[j][i][k].set_name("w");
                w[j][i][k].set_id(k+10*i+100*j);
                
                // w_jik * F(...k-1) part of inner product
                prod.set_name("prod");
                prod.clear();
                prod.absorp(w[j][i][k]).absorp(F[j][k-1]);
                DEBUG(prod);
                y[i][k].absorp(prod);


            }

            // apply activation function to result
            F[i][k].set_arg(0, y[i][k]);
        }
        
        
    }

    for(auto k = 0; k < M; k++)
        for(auto i = 0; i < N; i++)
            DEBUG(F[i][k]);
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
