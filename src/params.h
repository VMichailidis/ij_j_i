#include "hls_math.h"
#include "hls_stream.h"
#include "hls_vector.h"
#include <ap_fixed.h>
#include <iostream>
#include <stdlib.h>

#define IN_DIM_c (28*28)
#define OUT_DIM_c 200
#define PARTS_IN_c 14
#define PARTS_OUT_c 5

using namespace std;

 // typedef ap_fixed<8,4, AP_RND> T;
typedef int T;

template <int N> 
void combine_reduce(T &out, T (&v)[N], T (&v2)[N]);

template <int PARTS, int N>
void parted_combine_reduce(T &out, T (&v1)[PARTS][N/PARTS],
                                    T (&v2)[PARTS][N/PARTS]);



template <int IN_PARTS, int OUT_PARTS, int IN_DIM, int OUT_DIM>
void ij_j_i(T (&out)[OUT_DIM], 
            T (&w)[OUT_DIM][IN_DIM], 
            T (&in)[IN_DIM]);
// Main function
void ij_j_i_main(T (&out)[OUT_DIM_c], 
            T (&w)[OUT_DIM_c][IN_DIM_c], 
            T (&in)[IN_DIM_c]);
