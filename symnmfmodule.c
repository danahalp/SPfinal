#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.c" 
#include <stdio.h>


void parse_data_new(PyObject* data, int number_of_rows, int number_of_columns, double** datapoints_arr_ptr){
    int i, j;
    PyObject* float_obj;    

    for (i = 0; i < number_of_rows; i++) {
        for (j = 0; j < number_of_columns; j++) {
            float_obj = PyList_GetItem(data, i * number_of_columns + j); 
            datapoints_arr_ptr[i][j] = PyFloat_AsDouble(float_obj);
        }
    }
}

/* write matrix to PyObject* result in the same way as parse_data*/
void write_result(double** result_matrix, int number_of_rows, int number_of_columns, PyObject* result_list){
    int i, j;
    PyObject* float_obj;   

    for (i = 0 ; i < number_of_rows ; i++){
        for ( j=0 ; j < number_of_columns ; j++){
            /* convert double to python float */
            float_obj = PyFloat_FromDouble(result_matrix[i][j]);
            /* write python float to location j*number_of_rows + i */
            PyList_SetItem(result_list, (i*number_of_columns + j), float_obj);
        }
    }
}

static PyObject* sym_wrap(PyObject* self, PyObject* args){
    
    PyObject* data;
    int number_of_rows;
    int number_of_columns;
    double** datapoints_arr_ptr;
    PyObject* result;
    double** result_matrix;

    /* parse input arguments */
    if (!PyArg_ParseTuple(args, "OiiO",  &data, &number_of_rows, &number_of_columns, &result))
        return NULL;

    datapoints_arr_ptr = mem_allocator_non_square(number_of_rows, number_of_columns);
    parse_data_new(data, number_of_rows, number_of_columns, datapoints_arr_ptr);

    /* allocate memory for result_arr_ptr and result_arr_mem */
    result_matrix = mem_allocator(number_of_rows);

    /* calculate sym */
    sym(datapoints_arr_ptr, number_of_rows, number_of_columns, result_matrix);
    
    /* write result to PyObject* result */
    write_result(result_matrix, number_of_rows, number_of_rows, result);

    /* free memory */
    free_matrix(result_matrix, number_of_rows); 
    free_non_square_matrix(datapoints_arr_ptr);

    return Py_None;
}

static PyObject* ddg_wrap(PyObject* self, PyObject* args){
    
    PyObject* data;
    int number_of_rows;
    int number_of_columns;
    double** datapoints_arr_ptr;
    PyObject* result;
    double** result_matrix;

    /* parse input arguments */ 
    if (!PyArg_ParseTuple(args, "OiiO",  &data, &number_of_rows, &number_of_columns, &result))
        return NULL;

    datapoints_arr_ptr = mem_allocator_non_square(number_of_rows, number_of_columns);
    parse_data_new(data, number_of_rows, number_of_columns, datapoints_arr_ptr);

    /* allocate memory for result_arr_ptr and result_arr_mem */
    result_matrix = mem_allocator(number_of_rows);

    /* calculate ddg */
    ddg(datapoints_arr_ptr, number_of_rows, number_of_columns, result_matrix);
    
    /* write result to PyObject* result */
    write_result(result_matrix, number_of_rows, number_of_rows, result);

    /* free memory */
    free_matrix(result_matrix, number_of_rows);
    free_non_square_matrix(datapoints_arr_ptr);

    return Py_None;
}

static PyObject* norm_wrap(PyObject* self, PyObject* args){
    
    PyObject* data;
    int number_of_rows;
    int number_of_columns;
    double** datapoints_arr_ptr;
    PyObject* result;
    double** result_matrix;

    /* parse input arguments */
    if (!PyArg_ParseTuple(args, "OiiO",  &data, &number_of_rows, &number_of_columns, &result))
        return NULL;

    datapoints_arr_ptr = mem_allocator_non_square(number_of_rows, number_of_columns);
    parse_data_new(data, number_of_rows, number_of_columns, datapoints_arr_ptr);
    
    /* allocate memory for result_arr_ptr and result_arr_mem */
    result_matrix = mem_allocator(number_of_rows);

    /* calculate norm */
    norm(datapoints_arr_ptr, number_of_rows, number_of_columns, result_matrix);
    
    /* write result to PyObject* result */
    write_result(result_matrix, number_of_rows, number_of_rows, result);

    /* free memory */
    free_matrix(result_matrix, number_of_rows);
    free_non_square_matrix(datapoints_arr_ptr);

    return Py_None;
}

static PyObject* symnmf_wrap(PyObject* self, PyObject* args){
    
    PyObject* W;
    int number_of_rows;
    int k;
    PyObject* result;
    double** result_matrix;
    PyObject* H;    
    double** H_arr_ptr;
    double** W_arr_ptr;

    /* parse input arguments */
    if (!PyArg_ParseTuple(args, "OiiOO",  &W, &number_of_rows, &k, &H, &result))
        return NULL;

    /* allocate memory for H and W */
    W_arr_ptr = mem_allocator(number_of_rows);
    H_arr_ptr = mem_allocator_non_square(number_of_rows, k);
    
    /* write matrices into the allocated memory */
    parse_data_new(W, number_of_rows, number_of_rows, W_arr_ptr);
    parse_data_new(H, number_of_rows, k, H_arr_ptr);

    /* allocate memory for result */
    result_matrix = mem_allocator_non_square(number_of_rows, k);
   
    /* calculate symnmf */
    symnmf(W_arr_ptr, number_of_rows, k, H_arr_ptr, result_matrix);

    /* write result to PyObject* result */
    write_result(result_matrix, number_of_rows, k, result);

    /* free memory */
    free_non_square_matrix(result_matrix);
    free_matrix(W_arr_ptr, number_of_rows);
    free_non_square_matrix(H_arr_ptr);

    return Py_None;
}

static PyMethodDef methods[] = {
        {"sym_wrap", (PyCFunction)sym_wrap, METH_VARARGS, PyDoc_STR("data, number_of_rows, number_of_columns, result")},
        {"ddg_wrap", (PyCFunction)ddg_wrap, METH_VARARGS, PyDoc_STR("data, number_of_rows, number_of_columns, result")},
        {"norm_wrap", (PyCFunction)norm_wrap, METH_VARARGS, PyDoc_STR("data, number_of_rows, number_of_columns, result")},
        {"symnmf_wrap", (PyCFunction)symnmf_wrap, METH_VARARGS, PyDoc_STR("W, number_of_rows, k, H, result")},
        {NULL, NULL, 0, NULL}

};

static struct PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        "symnmfmodule",
        NULL,
        -1,
        methods
};

PyMODINIT_FUNC PyInit_symnmfmodule(void)
{
    return PyModule_Create(&module);
};


