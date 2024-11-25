// How to compile: g++ -g -Wall -fopenmp -o parallel_quicksort parallel_quicksort.cpp
// How to run: ./parallel_quicksort.exe file_path/file.dat

#include <stdio.h>
#include <vector>
#include <omp.h>
#include "data_manager.h"
#include <string>

using namespace std;

void Sort(vector<int>& vector, int left, int right) {
    int L = left;
    int R = right;
    int middleElement = vector[(left + right) / 2];

    // Dit gaat ervoor zorgen dat left en right samen niet boven de bit limiet van een int gaat.
    // int middleElement = vector[left + (right - left) / 2];

    while (L < R) {

        while (vector[L] < middleElement) {
            L++;
        }
        while (middleElement < vector[R]) {
            R--;
        }

        if (L <= R) {
            std::swap(vector[L], vector[R]);
            L++;
            R--;
        }
    }

    // Only make extra threads if sub array is large enough in size
    int threshold = 25;
    if (left < R) {
        // if (R - left > threshold) {
        //     printf("Making extra thread -> R - left > threshold: %d - %d = %d > %d\n", R, left, R-left, threshold);
        // }

        if (R - left > threshold) {
            #pragma omp task shared(vector)
            {
                Sort(vector, left, R);
            }
        }
        else {
            Sort(vector, left, R);
        }
    }
    if (L < right) {
        // if (right - L > threshold) {
        //     printf("Making extra thread -> right - L > threshold: %d - %d = %d > %d\n", right, L, right-L, threshold);
        // }

        if (right - L > threshold) {
            #pragma omp task shared(vector)
            {
                Sort(vector, L, right);
            }
        }
        else {
            Sort(vector, L, right);
        }
    }
}

void QuickSort(vector<int>& vector) {
    #pragma omp parallel
    {
        #pragma omp single  // Start by making 1 thread
        {
            Sort(vector, 0, vector.size() - 1);
        }
    }
}

bool check_sorted(vector<int>& vector, int& error_amount, int start_value = 0)
{
    bool is_sorted = true;

    for (int i = 1; i < static_cast<int>(vector.size()); i++) {
        if (vector[i] < vector[i-1]) {
            is_sorted = false;

            error_amount++;
            // printf("error at index %d", i);
        }
    }

    return is_sorted;
}

void print_vector(vector<int>& vector)
{
    printf("[");
    for (auto val : vector) {
        printf("%d,", val);
    }
    printf("]\n");
}

int main(int argc, char* argv[]) {
    vector<int> test_vector;
    std::string file_path;

    if (argc > 1) {
        file_path = argv[1]; // Eerste argument (argv[0] is de programmanaam)
        // std::cout << file_path << std::endl;
    }

    if (!file_path.empty()) {
        test_vector = load_dataset(file_path);
    }
    else {
        // test_vector = generate_dataset(100000);
        // shuffle_dataset(test_vector);

        test_vector = generate_random_dataset(100000);

        save_dataset(test_vector, "generated_dataset.dat");
    }

    QuickSort(test_vector);

    int error_amount = 0;
    bool result = check_sorted(test_vector, error_amount);

    printf("\nIs array sorted: %d\nAmount of errors: %d\n", result, error_amount); // 1 = true -> correct gesorteerd, 0 = false -> niet correct gesorteerd

    // printf("test_vector: ");
    // print_vector(test_vector);

    return 0;
}
