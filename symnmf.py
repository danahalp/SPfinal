import numpy as np
np.random.seed(0)
# np.random.uniform() for random selection later

import pandas as pd
import sys
import symnmfmodule as module

def print_error():
    print("An Error Has Occurred")


def print_matrix(H):
    for i in range(H.shape[0]):
        for j in range(H.shape[1]):
            print("{:.4f}".format(H[i][j]), end="")
            if j < H.shape[1] - 1:
                print(",", end="")
        print("")

# W is the input matrix
# k is the number of clusters
def initialize_H(W, k):
    m = np.mean(W)
    high = 2*np.sqrt(m / k)
    H = np.random.uniform(low=0, high=high, size=(W.shape[0], k))
    return H

def create_H(k, d, num_of_rows, num_of_columns):
    norm_result = [0.0] * (num_of_rows * num_of_rows)
    # run norm
    module.norm_wrap(d, num_of_rows, num_of_columns, norm_result)
    W = np.array(norm_result).reshape(num_of_rows, num_of_rows)

    # initialize H
    H = initialize_H(W, k)

    # prepare H and W for c python api and allocate result list of size num_of_rows*num_of_rows
    H_list = np.array(H).flatten().tolist()
    W_list = np.array(W).flatten().tolist()

    result = [0.0] * (num_of_rows * k)

    # call symnmf
    module.symnmf_wrap(W_list, num_of_rows, k, H_list, result)

    # res is the 0 matrix in num_of_rows*k size
    res = np.zeros((num_of_rows, k))

    # parse result list to res each k elements are a column including first row
    res = np.array(result).reshape(num_of_rows, k)
    return res

def main():
    # parse cmd input
    k = int(sys.argv[1])
    goal = sys.argv[2]
    file_name = sys.argv[3]

    # read data from file
    data = pd.read_csv(file_name, header=None)

    # convert data to numpy array
    data = data.to_numpy()

    d = np.array(data).flatten().tolist()

    num_of_rows = int(data.shape[0])
    num_of_columns = int(data.shape[1])
    
    # if goal== symnmf run symnmf
    if goal == "symnmf":
        #
        # norm_result = [0.0] * (num_of_rows * num_of_rows)
        #
        # # run norm
        # module.norm_wrap(d,num_of_rows, num_of_columns, norm_result)
        # W = np.array(norm_result).reshape(num_of_rows, num_of_rows)
        #
        # # initialize H
        # H = initialize_H(W, k)
        #
        # # prepare H and W for c python api and allocate result list of size num_of_rows*num_of_rows
        # H_list =  np.array(H).flatten().tolist()
        # W_list =  np.array(W).flatten().tolist()
        #
        # result = [0.0] * (num_of_rows * k)
        #
        # # call symnmf
        # module.symnmf_wrap(W_list, num_of_rows, k, H_list, result)
        #
        # # res is the 0 matrix in num_of_rows*k size
        # res = np.zeros((num_of_rows, k))
        #
        # # parse result list to res each k elements are a column including first row
        # res = np.array(result).reshape(num_of_rows, k)
        res = create_H(k, d, num_of_rows, num_of_columns)
        # print res
        print_matrix(res)

    #if goal== sym run sym from c python api    
    elif goal == "sym":
        # allocate result list of size num_of_rows*num_of_rows
        result = [0.0] * (num_of_rows * num_of_rows)
        # run sym
        module.sym_wrap(d,num_of_rows, num_of_columns, result)
        # read H from result list
        H = np.array(result).reshape(num_of_rows, num_of_rows)
        print_matrix(H)

    #if goal== ddg run ddg from c python api  
    elif goal == "ddg":
        # allocate result list of size num_of_rows*num_of_rows
        result = [0.0] * (num_of_rows * num_of_rows)
        # run ddg  
        module.ddg_wrap(d,num_of_rows, num_of_columns, result)
        H = np.array(result).reshape(num_of_rows, num_of_rows)
        print_matrix(H)
    
    # if goal== norm run norm from c python api
    elif goal == "norm":
        # allocate result list of size num_of_rows*num_of_rows
        result = [0.0] * (num_of_rows * num_of_rows)
        # run norm
        module.norm_wrap(d,num_of_rows, num_of_columns, result)
        H = np.array(result).reshape(num_of_rows, num_of_rows)
        print_matrix(H)
    else:
        print_error()
        return 0
        

if __name__ == "__main__":
    main()
