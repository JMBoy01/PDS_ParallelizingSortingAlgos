// How to compile: g++ -g -Wall -fopenmp -o parallel_quicksort parallel_quicksort.cpp

#include <stdio.h>
#include <vector>
#include <omp.h>
#include "data_manager.h"

using namespace std;

void Sort(vector<size_t>& list, size_t left, size_t right) {
    size_t L = left;
    size_t R = right;
    // size_t middleElement = list[(left + right) / 2];
    size_t middleElement = list[left + (right - left) / 2];

    while (L < R) {

        while (list[L] < middleElement) {
            L++;
        }
        while (middleElement < list[R]) {
            R--;
        }

        if (L <= R) {
            // size_t leftElement = list[L];
            // list[L] = list[R];
            // list[R] = leftElement;
            std::swap(list[L], list[R]);
            L++;
            R--;
        }
    }

    // Only make extra threads if sub array is large enough in size
    size_t threshold = 25;
    if (left < R) {
        if (R - left > threshold) {
            printf("Making extra thread -> R - left > threshold: %lld - %lld = %lld > %lld\n", R, left, R-left, threshold);
        }

        #pragma omp task shared(list) if (R - left > threshold)  // Set threshold
        Sort(list, left, R);
    }
    if (L < right) {
        if (right - L > threshold) {
            printf("Making extra thread -> right - L > threshold: %lld - %lld = %lld > %lld\n", right, L, right-L, threshold);
        }

        #pragma omp task shared(list) firstprivate(left, right) if (right - L > threshold)
        Sort(list, L, right);
    }
    #pragma omp taskwait
}

void QuickSort(vector<size_t>& vector) {
    #pragma omp parallel
    {
        #pragma omp single  // Start by making 1 thread
        Sort(vector, 0, vector.size() - 1);
    }
}

bool check_sorted(vector<size_t>& test_vector, vector<size_t>& error_idxs, size_t start_value = 0)
{
    bool is_sorted = true;

    for (size_t i = 1; i < test_vector.size(); i++) {
        if (test_vector[i] < test_vector[i-1]) {
            error_idxs.push_back(i);
            is_sorted = false;
        }
    }

    return is_sorted;
}

int main() {
    // test_vector<int> test_vector{
    //     56, 23, 78, 12, 45, 67, 89, 34, 92, 11, 38, 49, 26, 90, 72, 6, 83, 39, 18, 59,
    //     99, 3, 87, 65, 21, 43, 50, 29, 74, 94, 70, 10, 81, 55, 66, 48, 77, 53, 28, 7,
    //     96, 2, 60, 19, 35, 62, 85, 31, 100, 41, 37, 73, 1, 84, 5, 63, 17, 97, 25, 76,
    //     4, 61, 15, 69, 52, 91, 33, 40, 20, 75, 24, 64, 93, 47, 27, 68, 9, 95, 42, 8,
    //     82, 30, 13, 57, 44, 22, 80, 46, 14, 88, 51, 86, 58, 36, 16, 98, 71, 54, 79
    // };

    vector<size_t> test_vector = generate_dataset(1000);
    shuffle_dataset(test_vector);

    // printf("test_vector: [");
    // for (int val : test_vector) {
    //     printf("%d, ", val);
    // }
    // printf("]\n\n");

    QuickSort(test_vector);

    // printf("test_vector: [");
    // for (int val : test_vector) {
    //     printf("%d, ", val);
    // }
    // printf("]\n\n");

    vector<size_t> error_idxs = {};
    bool result = check_sorted(test_vector, error_idxs);
    printf("\nIs array sorted: %d", result); // 1 = true -> correct gesorteerd, 0 = false -> niet correct gesorteerd
    
    printf("\nError idxs: [");
    for (size_t val : error_idxs) {
        printf("%lld, ", val);
    }
    printf("]\n\n");

    return 0;
}
