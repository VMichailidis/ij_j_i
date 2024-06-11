#ifndef PARAMS_H
#define PARAMS_H
#include "params.h"
#endif

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "functions.h"
#endif

#define RAND_GEN 10
template <int N>
void print_vec(vec_t<T, N> &v){
	cout << "{";
	for(int i = 0; i < N; i++){cout << v[i]<< " ";}
	cout << "}" << endl;
}

template<int N, int M>
void print_mat(mat_t<T,N,M> &m){
	cout << "[";
	for(int i = 0; i<N; i++){
		cout << "[";
		for(int j = 0; j<M; j++){
			cout << m[i][j] << ", ";
		}
		cout<< "]," << endl;
	}
	cout << "]" << endl;
}

template <int PARTS, int MAX_SIZE ,int SIZE>
void get_parted_vec(vec_parted<T, PARTS, MAX_SIZE, SIZE> &v_hw, vec_t<T, SIZE> &v_sw){
	vec_t<T, MAX_SIZE> temp;
	vec_t<T, SIZE%MAX_SIZE> temp_r;
	for(int i = 0; i < PARTS; i++){
		for(int j = 0; j < MAX_SIZE; j++){
			v_sw[i*(MAX_SIZE) + j] = rand() % RAND_GEN;
			temp[j] = v_sw[i*(MAX_SIZE) + j];

		}
		v_hw.prt[i] = temp;
	}
	for(int i = 0; i < SIZE%MAX_SIZE; i++){
		v_sw[PARTS*MAX_SIZE + i] = rand() % RAND_GEN;
		temp_r[i] = v_sw[PARTS*MAX_SIZE + i];
 	}
	v_hw.rem = temp_r;
}

template<int PARTS, int MAX_SIZE, int ROWS, int COLS>
void get_weights(mat_parted<T, PARTS, MAX_SIZE, ROWS, COLS> &w_hw, mat_t<T, ROWS, COLS> &w_sw){
	for(int i = 0; i < ROWS; i++){
		get_parted_vec<PARTS, MAX_SIZE, COLS>(w_hw[i], w_sw[i]);
	}
}


template <int N>
void cdot(vec_t<T, N> &v1, vec_t<T, N> &v2, T &out){
	out = 0;
	for(int i=0; i<N; i++){out+=v1[i]*v2[i];}
}

template<int IN, int OUT>
void ij_j_i_model(mat_t<T, OUT, IN> &w, vec_t<T, IN> &in, vec_t<T, OUT> &out){
	for(int i = 0; i<OUT; i++){
		cdot<IN>(w[i], in, out[i]);
	}
}

int out_cmp(T pred_sw, T pred_hw){

	T temp = pred_sw;	
//	error when positive predicted software values differ from predicted hardware values
	int ret = (hls::abs(temp - pred_hw) > 1);
	return ret;
}
template<int PARTS, int MAX_LEN, int SIZE>
int vec_out_cmp(vec_parted<T, PARTS, MAX_LEN, SIZE> &pred_hw, vec_t <T, SIZE> &pred_sw){

	int ret = 0;
	for(int i = 0; i< PARTS; i++){
		for(int j = 0; j < MAX_LEN; SIZE){
			ret += (hls::abs( pred_hw.prt[i][j] - pred_sw[i*MAX_LEN + j] > 1));
		}
	}
	for(int i = 0; i < SIZE%MAX_LEN; i ++){
		ret += (hls::abs( pred_hw.rem[i] - pred_sw[PARTS*MAX_LEN + i] > 1));
	}
	return ret;
}

int test(int Num){
	mat_parted<T, PARTS_IN_c, MAX_LEN_c, OUT_DIM_c, IN_DIM_c> w_hw;
	vec_parted<T, PARTS_IN_c, MAX_LEN_c, IN_DIM_c> in_hw;
	vec_parted<T, PARTS_OUT_c, MAX_LEN_c, OUT_DIM_c> out_hw;


	mat_t<T, OUT_DIM_c, IN_DIM_c> w_sw;
	vec_t<T, IN_DIM_c> in_sw;
	vec_t<T, OUT_DIM_c> out_sw, out_sim;
	int err_count = 0;
	for(int i = 0; i<Num; i++){
		//setup
		get_parted_vec<PARTS_IN_c, MAX_LEN_c, IN_DIM_c>(in_hw, in_sw);

		get_weights<PARTS_IN_c, MAX_LEN_c, OUT_DIM_c, IN_DIM_c>(w_hw, w_sw);

		//genertate expected result
		ij_j_i_model<IN_DIM_c, OUT_DIM_c>(w_sw, in_sw, out_sw);


		//get design results
		ij_j_i_main(w_hw, in_hw, out_hw);

		copy_parted<PARTS_OUT_c, MAX_LEN_c, OUT_DIM_c>(out_hw, out_sim);

		cout << "Results: {";
		for (int i = 0; i<OUT_DIM_c; i++){
				cout << " " << out_sw[i];
			if(out_sim[i]!=out_sw[i]){err_count += 1;cout << "*";}
		}

		cout << "}" << endl;
		print_vec<OUT_DIM_c>(out_sim);
		if(err_count){cout<< "ERROR: " << err_count << " Mismatches detected" << endl;}
		else{cout << "Test Passed Successfully" << endl;}
	}
	return err_count;

}

int main(int argc, char **argv){
	return test(10);
}
