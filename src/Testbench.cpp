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
void print_array(T (&v)[N]){
	cout << "{";
	for(int i = 0; i < N; i++){cout << v[i]<< " ";}
	cout << "}" << endl;
}

template<int N, int M>
void print_mat(T (&m)[N][M]){
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


template<int N>
void get_rand_array(T (&arr)[N]){
	for(int i = 0; i<N; i++){
		arr[i] = rand()%RAND_GEN;
	}
}
template<int ROWS, int COLS>
void get_weights(T (&w)[ROWS][COLS]){
	for(int i = 0; i < ROWS; i++){
		get_rand_array(w[i]);
	}
}


template <int N>
void cdot(T &out, T (&v1)[N], T (&v2)[N]){
	out = 0;
	for(int i=0; i<N; i++){out+=v1[i]*v2[i];}
}

template<int IN, int OUT>
void ij_j_i_model(T (&out)[OUT], T (&w)[OUT][IN], T (&in)[IN]){
	for(int i = 0; i<OUT; i++){
		cdot(out[i], w[i], in);
	}
}

int out_cmp(T pred_sw, T pred_hw){
	int ret = (hls::abs(pred_sw - pred_hw) > 1);
	return ret;
}

int test(int Num){

	T w[OUT_DIM_c][IN_DIM_c], in[IN_DIM_c], out_sw[OUT_DIM_c], out_hw[OUT_DIM_c];
	int err_count = 0;
	int err_local = 0;
	for(int i = 0; i<Num; i++){
		err_local = 0;
		//setup
		get_rand_array(in);
		get_weights(w);
		//genertate expected result
		ij_j_i_model(out_sw, w, in);


		//get design results
		ij_j_i_main(out_hw, w, in);

		cout << "Results: {";
		for (int i = 0; i<OUT_DIM_c; i++){
				cout << " " << out_sw[i];
			if(out_hw[i]!=out_sw[i]){err_local += 1;cout << "*";}
		}

		cout << "}" << endl;
		if(err_count){cout<< "ERROR: " << err_local << " Mismatches detected" << endl;}
		else{cout << "Test Passed Successfully" << endl;}
		err_count += (err_local>0);
	}
	return err_count;
}

int main(int argc, char **argv){
	return test(10);
}
