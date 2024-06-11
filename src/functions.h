#ifndef PARAMS_H
#define PARAMS_H
#include "params.h"
#endif

template <int PARTS, int MAX_SIZE, int SIZE>
void read_parted(vec_parted<T, PARTS, MAX_SIZE, SIZE> &v_t,
                 vec_parted_s<T, PARTS, MAX_SIZE, SIZE> &v_s) {
  READ_PARTS: for (int i = 0; i < PARTS; i++) {
	#pragma HLS unroll
    v_t.prt[i] = v_s.prt[i].read();
  }
  v_t.rem = v_s.rem.read();
}

template <int PARTS, int MAX_SIZE, int SIZE>
void copy_parted(vec_parted<T, PARTS, MAX_SIZE, SIZE> &v_t, vec_t<T, SIZE> &v){
	for(int i = 0; i < PARTS; i++){
		for(int j = 0; j < MAX_SIZE; j++){
			v[i*MAX_SIZE + j] = v_t.prt[i][j];
		}
	}
	for(int i = 0; i<SIZE%MAX_SIZE; i++){
		v[PARTS*MAX_SIZE + i] = v_t.rem[i];
	}
}

template <int PARTS, int MAX_SIZE, int SIZE>
void write_parted(vec_parted_s<T, PARTS, MAX_SIZE, SIZE> &v_s,
				  vec_parted<T, PARTS, MAX_SIZE, SIZE> &v_t){
	WRITE_PARTS: for (int i = 0; i < PARTS; i++) {
	#pragma HLS unroll
    v_s.prt[i] << v_t.prt[i];
  }
  v_s.rem << v_t.rem;
}

template <int PARTS, int MAX_SIZE, int ROWS, int COLS>
void write_parted_mat(mat_parted_s<T, PARTS, MAX_SIZE, ROWS, COLS> &w_s,
					  mat_parted<T, PARTS, MAX_SIZE, ROWS, COLS> &w_t){
	WRITE_VEC: for(int i = 0; i < ROWS; i++){
#pragma HLS pipeline
		write_parted<PARTS, MAX_SIZE, COLS>(w_s[i], w_t[i]);
	}
}

template<int N>
void combine_reduce(vec_t<T, N> v1, vec_t<T, N> v2, T &out){
	
	T v1_temp[N], v2_temp[N];
	
	#pragma HLS array_partition variable=v1_temp dim=1 complete
	#pragma HLS array_partition variable=v2_temp dim=1 complete

	T result = 0;
	for(int i=0; i<N; i++){
	#pragma HLS unroll
		v1_temp[i] = v1[i];

	}
	for(int i=0; i<N; i++){
	#pragma HLS unroll
		v2_temp[i] = v2[i];
	}

	for(int i = 0; i<N; i++){
		#pragma HLS pipeline
		result += v1_temp[i] * v2_temp[i];
	}
	out = result;
}

template<int PARTS, int MAX_SIZE, int SIZE>
void parted_combine_reduce(vec_parted<T, PARTS, MAX_SIZE, SIZE> &v1, vec_parted<T, PARTS, MAX_SIZE, SIZE> &v2, T &out){
	T acc = 0;
	T temp;
	for(int i = 0; i < PARTS; i++){
		#pragma HLS inline
		#pragma HLS pipeline
		combine_reduce<MAX_SIZE>(vec_t<T, MAX_SIZE>(v1.prt[i]), vec_t<T, MAX_SIZE>(v2.prt[i]), temp);
		acc += temp;
	}
#pragma HLS inline
	combine_reduce<SIZE%MAX_SIZE>(vec_t<T, SIZE%MAX_SIZE>(v1.rem), vec_t<T, SIZE%MAX_SIZE>(v2.rem), temp);
	acc += temp;

	out = acc;

}

template<int PARTS_IN, int PARTS_OUT, int MAX_LEN, int IN_DIM, int OUT_DIM>
void ij_j_i(mat_parted_s<T, PARTS_IN, MAX_LEN, OUT_DIM, IN_DIM> &w, vec_parted_s<T, PARTS_IN, MAX_LEN, IN_DIM> &in, vec_parted_s<T, PARTS_OUT, MAX_LEN, OUT_DIM> &out){

	vec_parted<T, PARTS_IN, MAX_LEN, IN_DIM> in_temp, w_temp;
	vec_parted<T, PARTS_OUT, MAX_LEN, OUT_DIM> out_temp;

	#pragma HLS STREAM variable=in.prt depth=1
	#pragma HLS STREAM variable=in.rem depth=1
	#pragma HLS STREAM variable=out.prt depth=1
	#pragma HLS STREAM variable=out.rem depth=1
	#pragma HLS array_partition variable=w_temp dim=1 complete

	read_parted<PARTS_IN, MAX_LEN, IN_DIM>(in_temp, in);
	PARTS: for(int i = 0; i < PARTS_OUT; i++){

		VALUES: for(int j = 0; j < MAX_LEN; j++){
			#pragma HLS pipeline
			read_parted(w_temp, w[i*MAX_LEN + j]);
			parted_combine_reduce<PARTS_IN, MAX_LEN, IN_DIM>(w_temp, in_temp, out_temp.prt[i][j]);
		}
	}
	REM: for(int i = 0; i <OUT_DIM%MAX_LEN; i++){
		#pragma HLS pipeline
		read_parted(w_temp, w[PARTS_OUT*MAX_LEN + i]);
		parted_combine_reduce<PARTS_IN, MAX_LEN, IN_DIM>(w_temp, in_temp, out_temp.rem[i]);
	}

	write_parted<PARTS_OUT, MAX_LEN, OUT_DIM>(out, out_temp );
}
