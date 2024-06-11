#ifndef PARAMS_H
#define PARAMS_H
#include "params.h"
#endif

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "functions.h"
#endif


void ij_j_i_main(T (&out)[OUT_DIM_c], 
            T (&w)[OUT_DIM_c][IN_DIM_c], 
            T (&in)[IN_DIM_c]){

		ij_j_i<PARTS_IN_c, PARTS_OUT_c, IN_DIM_c, OUT_DIM_c>(out, w, in);
}
