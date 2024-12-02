// How to compile local: g++ -g -Wall -fopenmp -o quicksort quicksort.cpp
// How to compile VSC: g++ -fopenmp -o quicksort quicksort.cpp

#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <omp.h>
#include "data_manager.h"

using namespace std;

// SERIAL
void serial_sort(vector<int>& list, int left, int right) {
    int L = left;
    int R = right;
    int middleElement = list[(left + right) / 2];

    do {
        while (list[L] < middleElement) {
            L++;
        }
        while (middleElement < list[R]) {
            R--;
        }

        if (L <= R) {
            int leftElement = list[L];
            list[L] = list[R];
            list[R] = leftElement;
            L++;
            R--;
        }
    } while (L < R);

    if (left < R) {
        serial_sort(list, left, R);
    }
    if (L < right) {
        serial_sort(list, L, right);
    }
}

void serial_quicksort(vector<int>& list) {
    serial_sort(list, 0, list.size() - 1);  // Array, left en right meegeven
}

// PARALLEL
void parallel_sort(vector<int>& vector, int left, int right, int& threshold) {
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
    if (left < R) {
        // if (R - left > threshold) {
        //     printf("Making extra thread -> R - left > threshold: %d - %d = %d > %d\n", R, left, R-left, threshold);
        // }

        if (R - left > threshold) {
            #pragma omp task shared(vector)
            {
                parallel_sort(vector, left, R, threshold);
            }
        }
        else {
            parallel_sort(vector, left, R, threshold);
        }
    }
    if (L < right) {
        // if (right - L > threshold) {
        //     printf("Making extra thread -> right - L > threshold: %d - %d = %d > %d\n", right, L, right-L, threshold);
        // }

        if (right - L > threshold) {
            #pragma omp task shared(vector)
            {
                parallel_sort(vector, L, right, threshold);
            }
        }
        else {
            parallel_sort(vector, L, right, threshold);
        }
    }

    #pragma omp taskwait
}

void parallel_quicksort(vector<int>& vector, int& threshold) {
    #pragma omp parallel
    {
        #pragma omp single  // Start by making 1 thread
        {
            parallel_sort(vector, 0, vector.size() - 1, threshold);
        }
    }
}

// OTHER
void write_results_to_file(std::vector<long long>& sizes, std::vector<double>& serial_times, std::vector<double>& parallel_times)
{
    string filename = "output.txt";
    std::ofstream outfile;

    outfile.open(filename);

    // Controleer of het bestand succesvol is geopend
    if (!outfile) {
        std::cerr << "Error: Unable to open file " << filename << "\n";
        return;
    }
    
    for (int i = 0; i < static_cast<int>(sizes.size()); i++) {
        outfile << sizes[i] << "\t" << serial_times[i] << "\t" << parallel_times[i] << "\n";
    }

    outfile.close();
}

bool is_sorted(vector<int>& vector)
{
    for (int i = 1; i < static_cast<int>(vector.size()); i++) {
        if (vector[i] < vector[i-1]) {
            return false;
        }
    }

    return true;
}

// MAIN
int main() {
    // std::vector<long long> sizes = {100000000, 500000000, 1000000000, 2000000000, 5000000000}; // 0.4 GB, 2GB, 4 GB, 8GB, 20GB
    std::vector<long long> sizes = {10000, 50000, 100000, 200000, 500000};
    std::vector<double> serial_times{};
    std::vector<double> parallel_times{};

    for (int size : sizes) {
        std::vector<int> dataset = generate_random_dataset(size);
        std::vector<int> parallel_dataset = dataset;
        // shuffle_dataset(dataset);

        // std::cout << "Array size: " << size << std::endl;

        // Serial quicksort
        auto start = std::chrono::high_resolution_clock::now();

        serial_quicksort(dataset);

        auto end = std::chrono::high_resolution_clock::now();
        double serial_time = std::chrono::duration<double>(end - start).count();

        serial_times.push_back(serial_time);
        
        if (!is_sorted(dataset)) {
            std::cerr << "Error: Sequential mergesort failed!" << std::endl;
            return -1;
        }

        // Parallel quicksort
        int threshold = 1000;
        // int threshold = size/omp_get_num_threads();

        start = std::chrono::high_resolution_clock::now();

        parallel_quicksort(parallel_dataset, threshold);

        end = std::chrono::high_resolution_clock::now();
        double parallel_time = std::chrono::duration<double>(end - start).count();
        
        parallel_times.push_back(parallel_time);

        if (!is_sorted(parallel_dataset)) {
            std::cerr << "Error: parallel mergesort (sequential merge) failed!" << std::endl;
            return -1;
        }
    }

    write_results_to_file(sizes, serial_times, parallel_times);

    return 0;
}