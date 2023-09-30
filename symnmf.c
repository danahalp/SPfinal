#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "symnmf.h"

double** Sym = NULL;
double** Ddg = NULL;
double** Norm = NULL;

/* free matrix */
void free_matrix(double** matrix, int n){
    int i;
    for ( i = 0 ; i < n; i++){
        free(matrix[i]);
    }
    free(matrix);
}

/* allocates memory for matrix  */
double ** mem_allocator(int n){
    int i;
    double **mat = (double **)malloc(n * sizeof(double *));
    if (mat == NULL) {
        printf("An Error Has Occurred");
        return NULL;
    }
    for ( i = 0; i < n; i++){
        mat[i] = (double *)malloc(n * sizeof(double));
        if (mat[i] == NULL){
            free_matrix(mat, i);
            printf("An Error Has Occurred");
            return NULL;
        }
    }
    return mat;
}
/* prints matrix for testing*/
void print_non_square_matrix(double **matrix, int n, int m) {
    int i, j;

    for ( i = 0; i < n; i++) {
        for ( j = 0; j < m; j++) {
            if (j != n-1){
                printf("%.4f,", matrix[i][j]);
            }
            else{
                printf("%.4f", matrix[i][j]);
            }
        }
        printf("\n");
    }
}
/* prints matrix */
void print_matrix(double **matrix, int n) {
    int i, j;
    for ( i = 0; i < n; i++) {
        for ( j = 0; j < n; j++) {
            if (j != n-1){
                printf("%.4f,", matrix[i][j]);
            }
            else{
                printf("%.4f", matrix[i][j]);
            }
        }
        printf("\n");
    }
}

/* calculate the distance */
double squared_euclidean_distance(double *a, double *b, int n) {
    int i;
    double distance = 0.0;
    for ( i = 0; i < n; i++) {
        distance += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return distance;
}

void sym(double **data, int n, int m, double **similarity_matrix) {
    int i, j;
    for ( i = 0; i < n; i++) {
        for ( j = 0; j < n; j++) {
            if (i != j) {
                double distance = squared_euclidean_distance(data[i], data[j], m);
                similarity_matrix[i][j] = exp(-0.5 * distance);
            }
            else {
                similarity_matrix[i][j] = 0.0;
            }
        }
    }
}

void ddg(double **data, int n, int m, double **diagonal_matrix) {
    int i, j;

    double **similarity_matrix = mem_allocator(n);
    sym(data, n, m, similarity_matrix);

    /* put 0.0 in all cells of diagonal_matrix*/
    for ( i = 0; i < n; i++) {
        for ( j = 0; j < n; j++) {
            diagonal_matrix[i][j] = 0.0;
        }
    }

    for ( i = 0; i < n; i++) {
        for ( j = 0; j < n; j++) {
            diagonal_matrix[i][i] += similarity_matrix[i][j];
        }
    }
}


/* Function to calculate the square root of a matrix D */
double** matrixSquareRoot(double** D, int n) {
    int i, j;
    double** sqrtD = mem_allocator(n);
    for ( i = 0; i < n; i++) {
        for ( j = 0; j < n; j++) {
            if (i == j) {
                sqrtD[i][j] = (1.0 / sqrt(D[i][j]));
            } else {
                sqrtD[i][j] = 0.0;
            }
        }
    }
    return sqrtD;
}

void norm(double **data, int n, int m, double **W) {
    int i,j,k;
    double ** sqrtD = NULL;

    double **A = mem_allocator(n);
    double **D = mem_allocator(n);

    sym(data, n, m, A);
    ddg(data, n, m, D);

    sqrtD = matrixSquareRoot(D, n);

    if (D == NULL || A == NULL || sqrtD == NULL || W == NULL){
        free_matrix(D, n);
        free_matrix(A, n);
        free_matrix(sqrtD, n);
        free_matrix(W, n);
        printf("An Error Has Occurred");
        exit(1);
    }

    /* (D^(-1/2)) * A * (D^(-1/2))*/

    /* Multiply sqrtD * A */
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            W[i][j] = 0.0; /* Initialize W[i][j] to zero*/
            for ( k = 0; k < n; k++) {
                W[i][j] += sqrtD[i][k] * A[k][j];
            }
        }
    }

    /* Multiply W * sqrtD */
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            double sum = 0.0;
            for ( k = 0; k < n; k++) {
                sum += W[i][k] * sqrtD[k][j];
            }
            W[i][j] = sum; /* Assign the result to W[i][j]*/
        }
    }

    /* Free allocated memory*/
    free_matrix(D, n);
    free_matrix(sqrtD, n);
    free_matrix(A, n);
}

/* multiply matrices */
void multiply_matrices(double** matrix1, double** matrix2, double** result, int rows1, int cols1, int cols2) {
    int i, j, k;
    for ( i = 0; i < rows1; i++) {
        for ( j = 0; j < cols2; j++) {
            result[i][j] = 0.0;
            for ( k = 0; k < cols1; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

/* non square matrices free */
void free_non_square_matrix(double** matrix_ptr){
    free(matrix_ptr[0]);
    free(matrix_ptr);
}

/* allocate memory for non square matrix */
double** mem_allocator_non_square(int n, int m){
    double** mat_ptr = calloc(sizeof(double*), n);
    double* mat_mem = calloc(sizeof(double), n * m);
    if ((mat_ptr == NULL)||(mat_mem == NULL)){
        printf("An Error Has Occurred");
        exit(1);
    }
    connect_ptr_to_mem(mat_ptr, mat_mem, n, m);
    return mat_ptr;
}

/* connect ptr to mem */
void connect_ptr_to_mem(double** ptr, double* mem, int n, int m){
    int i;
    for ( i = 0; i < n; i++){
        ptr[i] = mem + i*m;
    }
}

/* update H function fot symnmf*/
void update_H(double** W, double** H, double** H_new, int n, int k){
    double beta= 0.5;
    int i, j;
    double x, y;

    /* allocate memory for internal matrices */
    double** Ht_ptr = mem_allocator_non_square(k, n);
    double** WH_ptr = mem_allocator_non_square(n, k);
    double** HHt_ptr = mem_allocator_non_square(n, n);
    double** HHtH_ptr = mem_allocator_non_square(n, k);

    /* put in Ht the transpose of H */
    for ( i = 0; i < n; i++){
        for ( j = 0; j < k; j++){
            Ht_ptr[j][i] = H[i][j];
        }
    }

    /* WH = W * H */
    multiply_matrices(W, H, WH_ptr, n, n, k);

    /* HHt = H * Ht */
    multiply_matrices(H, Ht_ptr, HHt_ptr, n, k, n);

    /* HHtH = HHt * H */
    multiply_matrices(HHt_ptr, H, HHtH_ptr, n, n, k);

    /* H_new = H * ((WH / HHtH)^(beta)) */
    for (i = 0; i < n; i++) {
        for (j = 0; j < k; j++) {
            x = WH_ptr[i][j] / HHtH_ptr[i][j];
            y = (1 - beta) + (beta * x);
            H_new[i][j] = H[i][j] * y;
        }
    }

    /* free memory */
    free_non_square_matrix(HHt_ptr);
    free_non_square_matrix(Ht_ptr);
    free_non_square_matrix(WH_ptr);
    free_non_square_matrix(HHtH_ptr);
}

/* check convergence for symnmf */
int converged(double** H, double** H_new, int n, int k){

    int i, j;
    double epsilon = 0.0001;
    /* allocate new marix in the shape of H */
    double** H_new_minus_H_ptr = mem_allocator_non_square(n, k);
    double sum = 0.0;

    /* calculate the forbenius norm of (H_new-H) */
    /*///////////////////////////////////////////*/
    /* calculate H_new - H */
    for ( i = 0; i < n; i++){
        for ( j = 0; j < k; j++){
            H_new_minus_H_ptr[i][j] = H_new[i][j] - H[i][j];
        }
    }

    /* claculate sum of squares of each element of H_new_minus_H */
    for ( i = 0; i < n; i++){
        for ( j = 0; j < k; j++){
            sum += (H_new_minus_H_ptr[i][j] * H_new_minus_H_ptr[i][j]);
        }
    }

    free_non_square_matrix(H_new_minus_H_ptr);
    if (sum < epsilon)
        return 1;
    return 0;
}



void symnmf(double **W_arr_ptr, int n, int k, double **H_arr_ptr, double **result_matrix){

    int iter_num = 0;
    int i, j;
    double** H_new_ptr = mem_allocator_non_square(n, k);

    update_H(W_arr_ptr, H_arr_ptr, H_new_ptr, n, k);

    while ((iter_num<300) && (converged(H_arr_ptr, H_new_ptr, n, k)==0)){

        /* copy H_new to H */
        for ( i = 0; i < n; i++) {
            for ( j = 0; j < k; j++) {
                H_arr_ptr[i][j] = H_new_ptr[i][j];
            }
        }
        update_H(W_arr_ptr, H_arr_ptr, H_new_ptr, n, k);
        iter_num++;
    }


    /* after convergence, copy H_new to result */
    for ( i = 0; i < n; i++) {
        for ( j = 0; j < k; j++) {
            result_matrix[i][j] = H_new_ptr[i][j];
        }
    }

    /* free memory */
    free_non_square_matrix(H_new_ptr);



}

double ** init_file_info(FILE* file_name, int n, int d){
    char c;
    double curr;
    int i, j;
    double **data = mem_allocator_non_square(n, d);

    for ( i = 0; i < n; i++){
        for(  j = 0; j < d; j++){
            int val = fscanf(file_name, "%lf%c", &curr, &c);
            if (val == EOF && i != n - 1) {
                free_matrix(data, n);
                printf ("An Error Has Occurred");
                exit(1);
            }
            data[i][j] = curr;
        }
    }
    return data;
}

/* finds n = num of lines and d = dimensions */
void info_file(FILE* file_name, int* n, int* d){
    char c;
    c = fgetc(file_name);
    while (c != '\n'){
        if (c == ',')
            (*d)++;
        c = fgetc(file_name);
    }
    (*d)++;
    rewind(file_name);
    while (c != EOF){
        if (c == '\n')
            (*n)++;
        c = fgetc(file_name);
    }
}

int main(int argc, char* argv[]){
    char* goal;
    char* file_name;
    int n = 0;
    int d = 0;
    int res;
    FILE* file;
    double ** data;
    /* validate the 2 arguments in main */
    if (argc != 3){
        printf ("An Error Has Occurred");
        exit(1);
    }

    goal = argv[1];
    file_name = argv[2];

    file = fopen(file_name, "r");
    if (file == NULL){
        printf("An Error Has Occurred");
        fclose(file);
        exit(1);
    }
    info_file(file, &n, &d);
    fseek(file, 0, 0);
    n = n - 1;
    data = init_file_info(file, n, d);

    res = fclose(file);
    if(res != 0){
        printf("An Error Has Occurred");
        free_matrix(data, n);
        exit(1);
    }

    /* reading goal input */
    if (strcmp(goal,"sym") == 0){
        double **Sym = mem_allocator(n);
        sym(data, n, d, Sym);
        print_matrix(Sym, n);
        free_matrix(Sym, n);
    }
    else if(strcmp(goal, "ddg") == 0){
        double **Ddg = mem_allocator(n);
        ddg(data, n, d, Ddg);
        print_matrix(Ddg, n);
        free_matrix(Ddg, n);
    }
    else if(strcmp(goal, "norm") == 0){
        double **Norm = mem_allocator(n);
        norm(data, n, d, Norm);
        print_matrix(Norm, n);
        free_matrix(Norm, n);
    }
    else{
        printf("An Error Has Occurred");
        free_matrix(data, n);
        exit(1);
    }
    free_non_square_matrix(data);
    return 0;
}
