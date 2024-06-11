#ifndef PARAMS_H
#define PARAMS_H
#include "params.h"
#endif

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "functions.h"
#endif


void ij_j_i_main(mat_parted<T, PARTS_IN_c, MAX_LEN_c, OUT_DIM_c, IN_DIM_c> &w,
		vec_parted<T, PARTS_IN_c, MAX_LEN_c, IN_DIM_c> &in,
		vec_parted<T, PARTS_OUT_c, MAX_LEN_c, OUT_DIM_c> &out){

		//wrapper circuit that takes data in memory and feeds them to streams.
		// Although simulations present no problems with reading empty streams, this circuit is the only way for cosimulations to avoid reading empty streams
		mat_parted_s<T, PARTS_IN_c, MAX_LEN_c, OUT_DIM_c, IN_DIM_c> w_temp;
		vec_parted_s<T, PARTS_IN_c, MAX_LEN_c, IN_DIM_c> in_temp;
		vec_parted_s<T, PARTS_OUT_c, MAX_LEN_c, OUT_DIM_c> out_temp;
#pragma HLS STREAM variable=in.prt depth=1
#pragma HLS STREAM variable=in.rem depth=1
#pragma HLS STREAM variable=out.prt depth=1
#pragma HLS STREAM variable=out.rem depth=1
#pragma HLS STREAM variable=w_temp depth=1
		write_parted_mat(w_temp, w);
		write_parted(in_temp, in);



		ij_j_i<PARTS_IN_c, PARTS_OUT_c, MAX_LEN_c, IN_DIM_c, OUT_DIM_c>(w_temp, in_temp, out_temp);

		read_parted(out, out_temp);

}
