#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h> 
#include "data_manager.h"


std::vector<int> merge(const std::vector<int>& left, const std::vector<int>& right) {
    std::vector<int> result;
    size_t i = 0, j = 0;
    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j]) {
            result.push_back(left[i++]);
        } else {
            result.push_back(right[j++]);
        }
    }
    while (i < left.size()) result.push_back(left[i++]);
    while (j < right.size()) result.push_back(right[j++]);
    return result;
}

void serialMergeSort(std::vector<int>& array) {
    if (array.size() <= 1) return;

    size_t mid = array.size() / 2;
    std::vector<int> left(array.begin(), array.begin() + mid);
    std::vector<int> right(array.begin() + mid, array.end());

    serialMergeSort(left);
    serialMergeSort(right);

    array = merge(left, right);
}


void parallelMergeSort(std::vector<int>& array, int depth = 0) {
    if (array.size() <= 1) return;

    size_t mid = array.size() / 2;
    std::vector<int> left(array.begin(), array.begin() + mid);
    std::vector<int> right(array.begin() + mid, array.end());

    #pragma omp parallel sections if(depth < 3) schedule(dynamic) // scheduling strategy for divide an conquer problem, helps balancing the load across threads
    {
        #pragma omp section
        {
            parallelMergeSort(left, depth + 1);
        }
        #pragma omp section
        {
            parallelMergeSort(right, depth + 1);
        }
    }

    array = merge(left, right);
}


int main() {
    
    std::vector<int> dataset_sizes = {1000, 10000, 100000, 500000, 1000000}; 
    for (int size : dataset_sizes) {
        std::cout << "dataset size: " << size << std::endl;

        std::vector<int> dataset = generate_random_dataset(size);
        // omp_set_num_threads(72); // VSC 72 cores
        // measure serial mergesort
        std::vector<int> serial_dataset = dataset;
        double start_serial = omp_get_wtime();
        serialMergeSort(serial_dataset);
        double end_serial = omp_get_wtime();
        std::cout << "serial mergesort time: " << end_serial - start_serial << " seconds" << std::endl;

        // measure parallel mergesort
        std::vector<int> parallel_dataset = dataset; 
        double start_parallel = omp_get_wtime();
        parallelMergeSort(parallel_dataset);
        double end_parallel = omp_get_wtime();
        std::cout << "parallel mergesort time: " << end_parallel - start_parallel << " seconds" << std::endl;

        if (serial_dataset == parallel_dataset) {
            std::cout << "correct!" << std::endl;
        } else {
            std::cerr << "Not correct!" << std::endl;
        }

        std::cout << "-------------------------------" << std::endl;
    }

    return 0;
}