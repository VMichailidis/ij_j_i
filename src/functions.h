#ifndef PARAMS_H
#define PARAMS_H
#include "params.h"
#endif

template<int PARTS, int N>
void partition_array(T (&out)[PARTS][N/PARTS], T (&in)[N]){
	#pragma HLS array_partition variable=in dim=1 complete
	#pragma HLS array_partition variable=out dim=0 complete
	for(int i = 0; i < PARTS; i++){
		for(int j = 0; j < N/PARTS; j++){
			#pragma HLS unroll
			out[i][j] = in[i*(N/PARTS) + j];
		}
	}
}

template<int PARTS, int N>
void aggregate_array(T (&out)[N], T (&in)[PARTS][N/PARTS]){

	#pragma HLS array_partition variable=in dim=0 complete
	#pragma HLS array_partition variable=out dim=1 complete
	for(int i = 0; i < PARTS; i++){
		for(int j = 0; j < N/PARTS; j++){
			#pragma HLS unroll
			out[i*(N/PARTS) + j] = in[i][j];
		}
	}
}

template <int N> 
void combine_reduce(T &out, T (&v1)[N], T (&v2)[N]){
	
	
	T result = 0;

	#pragma HLS array_partition variable=v1 dim=1 complete
	#pragma HLS array_partition variable=v2 dim=1 complete

	for(int i = 0; i<N; i++){
		#pragma HLS unroll
		result += v1[i] * v2[i];
	}
	out = result;
}

template<int PARTS, int N>
void parted_combine_reduce(T &out, T (&v1)[PARTS][N/PARTS],
                                    T (&v2)[PARTS][N/PARTS]){
	T acc = 0;
	T temp;
	for(int i = 0; i < PARTS; i++){
		#pragma HLS pipeline
		combine_reduce<N/PARTS>(temp, v1[i], v2[i]);
		acc += temp;
	}
	out = acc;

}

template<int IN_PARTS, int OUT_PARTS, int IN_DIM, int OUT_DIM>
void ij_j_i(T (&out)[OUT_DIM], 
 	        T (&w)[OUT_DIM][IN_DIM], 
 	        T (&in)[IN_DIM]){
	
	T in_temp[IN_PARTS][IN_DIM/IN_PARTS], w_temp[IN_PARTS][IN_DIM/IN_PARTS];
	T out_temp[OUT_PARTS][OUT_DIM/OUT_PARTS];
	
	partition_array(in_temp, in);

	PARTS: for(int i = 0; i <OUT_PARTS; i++){
		VALUES: for(int j = 0; j < OUT_DIM/OUT_PARTS; j++){
			#pragma HLS pipeline
			partition_array(w_temp, w[i*(OUT_DIM/OUT_PARTS) + j]);
			parted_combine_reduce<IN_PARTS, IN_DIM>(out_temp[i][j], w_temp, in_temp);
		}
	}
	aggregate_array(out, out_temp);
}
