#include <iostream>
#include <vector>
#include <chrono>
#include "data_manager.h"


bool is_sorted(const std::vector<int>& array) {
    for (size_t i = 1; i < array.size(); ++i) {
        if (array[i - 1] > array[i]) {
            return false;
        }
    }
    return true;
}

std::vector<int> mergeSequential(const std::vector<int>& left, const std::vector<int>& right) {
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

void mergeParallel(const std::vector<int>& left, const std::vector<int>& right, std::vector<int>& result) {
    size_t total_size = left.size() + right.size();
    result.resize(total_size);

    #pragma omp parallel
    {
        #pragma omp for schedule(dynamic)
        for (size_t k = 0; k < total_size; ++k) {
            size_t i = k < left.size() ? k : left.size();
            size_t j = k >= left.size() ? k - left.size() : 0;

            result[k] = (i < left.size() && (j >= right.size() || left[i] <= right[j]))
                            ? left[i++] : right[j++];
        }
    }
}

void sequentialMergeSort(std::vector<int>& array) {
    if (array.size() <= 1) return;

    size_t mid = array.size() / 2;
    std::vector<int> left(array.begin(), array.begin() + mid);
    std::vector<int> right(array.begin() + mid, array.end());

    sequentialMergeSort(left);
    sequentialMergeSort(right);

    array = mergeSequential(left, right);
}

void parallelMergeSortSequentialMerge(std::vector<int>& array, int depth, int max_depth) {
    if (array.size() <= 1) return;

    size_t mid = array.size() / 2;
    std::vector<int> left(array.begin(), array.begin() + mid);
    std::vector<int> right(array.begin() + mid, array.end());

    #pragma omp parallel sections if (depth < max_depth)
    {
        #pragma omp section
        parallelMergeSortSequentialMerge(left, depth + 1, max_depth);
        #pragma omp section
        parallelMergeSortSequentialMerge(right, depth + 1, max_depth);
    }

    array = mergeSequential(left, right);
}

void parallelMergeSortParallelMerge(std::vector<int>& array, int depth, int max_depth) {
    if (array.size() <= 1) return;

    size_t mid = array.size() / 2;
    std::vector<int> left(array.begin(), array.begin() + mid);
    std::vector<int> right(array.begin() + mid, array.end());

    #pragma omp parallel sections if (depth < max_depth)
    {
        #pragma omp section
        parallelMergeSortParallelMerge(left, depth + 1, max_depth);
        #pragma omp section
        parallelMergeSortParallelMerge(right, depth + 1, max_depth);
    }

    mergeParallel(left, right, array);
}

int main() {
    //omp_set_num_threads(72);
    //int max_depth = 6;
    int max_depth = 3;

    std::vector<int> sizes = {1000, 10000, 100000, 1000000};
 
    for (int size : sizes) {
        std::vector<int> dataset = generate_random_dataset(size);
        shuffle_dataset(dataset);

        std::cout << "Array size: " << size << std::endl;

        // Sequential mergesort
        std::vector<int> seq_array = dataset;
        auto start = std::chrono::high_resolution_clock::now();
        sequentialMergeSort(seq_array);
        auto end = std::chrono::high_resolution_clock::now();
        double seq_time = std::chrono::duration<double>(end - start).count();
        std::cout << "Sequential Merge Sort Time: " << seq_time << " seconds" << std::endl;
        if (!is_sorted(seq_array)) {
            std::cerr << "Error: Sequential Merge Sort failed!" << std::endl;
            return -1;
        }

        // Parallel mergesort (Sequential merge)
        std::vector<int> parallel_seq_array = dataset;
        start = std::chrono::high_resolution_clock::now();
        parallelMergeSortSequentialMerge(parallel_seq_array, 0, max_depth);
        end = std::chrono::high_resolution_clock::now();
        double parallel_seq_time = std::chrono::duration<double>(end - start).count();
        std::cout << "Parallel mergesort (sequential merge) time: " << parallel_seq_time << " seconds" << std::endl;
        if (!is_sorted(parallel_seq_array)) {
            std::cerr << "Error: parallel mergesort (sequential merge) failed!" << std::endl;
            return -1;
        }

        // Parallel mergesort (Parallel merge)
        std::vector<int> parallel_par_array = dataset;
        start = std::chrono::high_resolution_clock::now();
        parallelMergeSortParallelMerge(parallel_par_array, 0, max_depth);
        end = std::chrono::high_resolution_clock::now();
        double parallel_par_time = std::chrono::duration<double>(end - start).count();
        std::cout << "Parallel mergesort (parallel merge) time: " << parallel_par_time << " seconds" << std::endl;
        if (!is_sorted(parallel_par_array)) {
            std::cerr << "Error: parallel mergesort (parallel merge) failed!" << std::endl;
            return -1;
        }

        std::cout << "-----------------------------" << std::endl;
    }

    return 0;
}