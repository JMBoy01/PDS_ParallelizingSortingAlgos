#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>

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

void parallelMergeSort(std::vector<int>& array, int depth = 0) {
    if (array.size() <= 1) return;

    size_t mid = array.size() / 2;
    std::vector<int> left(array.begin(), array.begin() + mid);
    std::vector<int> right(array.begin() + mid, array.end());

    #pragma omp task shared(left) if (depth < 3)
    parallelMergeSort(left, depth + 1);

    #pragma omp task shared(right) if (depth < 3)
    parallelMergeSort(right, depth + 1);

    // wait for tasks to finish
    #pragma omp taskwait

    // merge the sorted halves
    array = merge(left, right);
}

int main() {
    std::vector<int> array{
        56, 23, 78, 12, 45, 67, 89, 34, 92, 11, 38, 49, 26, 90, 72, 6, 83, 39, 18, 59,
        99, 3, 87, 65, 21, 43, 50, 29, 74, 94, 70, 10, 81, 55, 66, 48, 77, 53, 28, 7,
        96, 2, 60, 19, 35, 62, 85, 31, 100, 41, 37, 73, 1, 84, 5, 63, 17, 97, 25, 76,
        4, 61, 15, 69, 52, 91, 33, 40, 20, 75, 24, 64, 93, 47, 27, 68, 9, 95, 42, 8,
        82, 30, 13, 57, 44, 22, 80, 46, 14, 88, 51, 86, 58, 36, 16, 98, 71, 54, 79
    };

    std::cout << "Original Array: ";
    for (int num : array) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    double start_time = omp_get_wtime(); 

    #pragma omp parallel
    {
        #pragma omp single
        {
            parallelMergeSort(array);
        }
    }

    double end_time = omp_get_wtime();

    std::cout << "Sorted Array: ";
    for (int num : array) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    std::cout << "Time taken: " << end_time - start_time << " seconds" << std::endl;

    return 0;
}
