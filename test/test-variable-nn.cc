
#include <iostream>
#include <iomanip>
#include <iterator>
#include <array>
#include <map>
#include <numeric>
#include <random>
#include <valarray>

#include "variable.h"
#include "symbolic.h"
#include "util.h"

using std::cout;
using std::endl;

using std::map;

// Testing /////////////////////////////////////////////////////////////////////
#define N 4 // input size
#define M 4 // number of layers
#define H 10 // order of derv
std::size_t sizes[M+1] = {N, 6, 8, 6, 2}; // size of each layer

int main
() {
    std::mt19937 gen(0);
    std::uniform_real_distribution<float> unifrom_rand(-0.85, 0.85);
    diff::variable<H> input[N] = {-0.80, 0.55, 0.35, 0.15};
    diff::variable<H> weight[M][8][8];
    diff::variable<H> bias[M+1][8];
    diff::variable<H> output[M+1][8];

    /*
    for(auto i = 0; i < N; i++ )
        input[i].as_variable();
    */
    
    // allocate and initialize weights with unifromly random vals 
    for(auto k = 0; k < M; k++) {
        // rows
        for(auto i = 0; i < sizes[k]; i++) {
            // cols
            for(auto j = 0; j < sizes[k+1]; j++) {
                weight[k][i][j].clear();
                weight[k][i][j] = unifrom_rand(gen);
                weight[k][i][j].as_constant();
            }
        }
        
        for( auto j = 0; j < sizes[k+1]; j++ ) {
            bias[k][j].clear();
            bias[k][j] = unifrom_rand(gen);
            bias[k][j].as_constant();
        }

        for(auto j = 0; j < sizes[k+1]; j++ )
            output[k+1][j].clear();
    }

    
    for(auto v = 0; v < N; v++ ) {

        debug::dump("variable", v);
        debug::dump("layer", 0);
        for(auto i = 0; i < N; i++) {
            output[0][i] = input[i];
            if(i == v)
                output[0][v].as_variable();
            else
                output[0][i].as_constant();
            debug::dump("input", output[0][i]);
        }

        
        // calculate output for hidden layers
        for(auto k = 0; k < M; k++) {
            debug::dump("layer", k+1);
            
            for(auto j = 0; j < sizes[k+1]; j++) {
                output[k+1][j].clear();
                for(auto i = 0; i < sizes[k]; i++) {
                    output[k+1][j] += (output[k][i]) * (weight[k][i][j]);
                }
                output[k+1][j] += bias[k][j];
                // TODO: apply real activation function
                output[k+1][j] = (output[k+1][j]) * (output[k+1][j]);

                debug::dump("output", output[k+1][j]);
            }
        }
    }
        
}


 /*

  [1;0;]
  [1;1;(1,1)]
  [1;2;(1,2)]+[1;1;(2,1)]
  
  [1;3;(1,3)]+[2;2;(1,1),(2,1)]
             +[1;2;(1,1),(2,1)]+[1;1;(3,1)]
  ---------------------------------------------------------------
 =[1;3;(1,3)]+[3;2;(1,1),(2,1)]+[1;1;(3,1)]
  
  [1;4;(1,4)]+[3;3;(1,2),(2,1)]
             +[3;3;(1,2),(2,1)]+[3;2;(2,2)]+[3;2;(1,1),(3,1)]
                                           +[1;2;(1,1),(3,1)]+[1;1;(4,1)]
  ---------------------------------------------------------------
 =[1;4;(1,4)]+[6;3;(1,2),(2,1)]+[3;2;(2,2)]+[4;2;(1,1),(3,1)]+[1;1;(4,1)]

 
  [1;5;(1,5)]+[ 4;4;(1,3),(2,1)]
             +[ 6;4;(1,3),(2,1)]+[12;3;(1,1),(2,2)]+[ 6;3;(1,2),(3,1)]
                                +[ 3;3;(1,1),(2,2)]                   +[ 6;2;(2,1),(3,1)]
                                                   +[ 4;3;(1,2),(3,1)]+[ 4;2;(2,1),(3,1)]+[4;2;(1,1),(4,1)]
                                                                                         +[1;2;(1,1),(4,1)]+[1;1;(5,1)]
  ---------------------------------------------------------------
 =[1;5;(1,5)]+[10;4;(1,3),(2,1)]+[15;3;(1,1),(2,2)]+[10;3;(1,2),(3,1)]+[10;2;(2,1),(3,1)]+[5;2;(1,1),(4,1)]+[1;1;(5,1)]



  map< int, map< array<int, N>, int >
    */
