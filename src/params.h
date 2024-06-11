#include "hls_math.h"
#include "hls_stream.h"
#include "hls_vector.h"
#include <ap_fixed.h>
#include <iostream>
#include <stdlib.h>

#define IN_DIM_c 200
#define OUT_DIM_c 100
#define MAX_LEN_c 15 //largest synthesizable vector length
#define PARTS_IN_c (IN_DIM_c)/MAX_LEN_c
#define PARTS_OUT_c (OUT_DIM_c)/MAX_LEN_c


using namespace std;

 typedef ap_fixed<8,4, AP_RND> T;
//typedef int T;
// stream of vectors type
template <typename T, int N> using vec_s_t = hls::stream < hls::vector<T, N>>;

// vector type
template <typename T, int N> using vec_t = hls::vector<T, N>;

// matrix type
template <typename T, int N, int M>
using mat_s_t = hls::stream<hls::vector<hls::vector<T, M>, N>>;

// stream of matrices
template <typename T, int N, int M>
using mat_t = hls::vector<hls::vector<T, M>, N>;

// Partitioned vectors
template <typename T, int PARTS, int MAX_SIZE, int SIZE> struct vec_parted {
  vec_t<T, MAX_SIZE> prt[PARTS];
  vec_t<T, SIZE % MAX_SIZE> rem;

};

// Partitioned Stream of vectors
template <typename T, int PARTS, int MAX_SIZE, int SIZE> struct vec_parted_s {
  hls::stream<vec_t<T, MAX_SIZE>> prt[PARTS];
  hls::stream<vec_t<T, SIZE % MAX_SIZE>> rem;

};

template <typename T, int PARTS, int MAX_SIZE, int ROWS, int COLS>
using mat_parted = vec_parted<T, PARTS, MAX_SIZE, COLS>[ROWS];

template <typename T, int PARTS, int MAX_SIZE, int ROWS, int COLS>
using mat_parted_s = vec_parted_s<T, PARTS, MAX_SIZE, COLS>[ROWS];

template <int N> void combine_reduce(vec_t<T, N> &v1, vec_t<T, N> &v2, T &out);

template <int PARTS, int MAX_SIZE, int SIZE>
void parted_combine_reduce(vec_parted<T, PARTS, MAX_SIZE, SIZE> &v1,
                           vec_parted<T, PARTS, MAX_SIZE, SIZE> &v2, T &out);
// Main function

// void combine_reduce_main(vec_t<T, MAX_LEN> &v1, vec_t<T, MAX_LEN> &v2, T
// &out);

template <int N>
void combine_reduce(vec_s_t<T, N> &v1, vec_s_t<T, N> &v2, T &out);

template <int PARTS_IN, int PARTS_OUT, int MAX_LEN, int IN_DIM, int OUT_DIM>
void ij_j_i(mat_parted_s<T, PARTS_IN, MAX_LEN, OUT_DIM, IN_DIM> &w1,
		vec_parted_s<T, PARTS_IN, MAX_LEN, IN_DIM> &in,
		vec_parted_s<T, PARTS_OUT, MAX_LEN, OUT_DIM> &out);

// Main function

void ij_j_i_main(mat_parted<T, PARTS_IN_c, MAX_LEN_c, OUT_DIM_c, IN_DIM_c> &w,
		vec_parted<T, PARTS_IN_c, MAX_LEN_c, IN_DIM_c> &in,
		vec_parted<T, PARTS_OUT_c, MAX_LEN_c, OUT_DIM_c> &out);
