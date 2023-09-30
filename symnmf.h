#ifndef UNTITLED_SYMNMF_H
#define UNTITLED_SYMNMF_H

#include <stdio.h>

void free_matrix(double** matrix, int n);

double ** mem_allocator(int n);

void print_non_square_matrix(double **matrix, int n, int m);

void print_matrix(double **matrix, int n);

double squared_euclidean_distance(double *a, double *b, int n);

void sym(double **data, int n, int m, double **similarity_matrix);

void ddg(double **data, int n, int m, double **diagonal_matrix);

void norm(double **data, int n, int m, double **W);

void update_H(double** W, double** H, double** H_new, int n, int k);

void connect_ptr_to_mem(double** ptr, double* mem, int n, int m);

int converged(double** H, double** H_new, int n, int k);

void symnmf(double **W_arr_ptr, int n, int k, double **H_arr_ptr, double **result_matrix);

double ** init_file_info(FILE* file_name, int n, int d);

void info_file(FILE* file_name, int* n, int* d);


#endif
