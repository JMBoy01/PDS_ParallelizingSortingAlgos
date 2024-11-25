// g++ -g -Wall -fopenmp -o parallel_quicksort parallel_quicksort.cpp

#include <iostream>
#include <vector>
#include <omp.h>
#include "data_manager.h"

// Functie om de array te partitioneren rond de pivot
size_t partition(std::vector<int>& arr, size_t low, size_t high) {
    int pivot = arr[high]; // Het meest rechtse element is de pivot
    size_t i = low - 1; // i houdt de index van de laatste element dat kleiner is dan de pivot

    for (size_t j = low; j < high; ++j) {
        if (arr[j] < pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]); // Zet de pivot op de juiste plaats
    return i + 1;
}

// Quicksort functie die gebruik maakt van parallelisme
void quicksort(std::vector<int>& arr, size_t low, size_t high) {
    if (low < high) {
        size_t pi = partition(arr, low, high);

        #pragma omp parallel sections
        {
            #pragma omp section
            quicksort(arr, low, pi - 1);

            #pragma omp section
            quicksort(arr, pi + 1, high);
        }
    }
}

int main() {
    // Test array
    // std::vector<size_t> arr = {10, 80, 30, 90, 40, 50, 70};
    std::vector<int> arr;
    // Converteer door te itereren over de size_t vector
    for (size_t num : generate_random_dataset(1000)) {
        arr.push_back(static_cast<int>(num)); // Gebruik static_cast om size_t naar int te converteren
    }

    printf("array created");

    // Aantal threads instellen voor OpenMP
    // omp_set_num_threads(4); // Bijv. 4 threads gebruiken

    // Quicksort aanroepen
    quicksort(arr, 0, arr.size() - 1);

    // Resultaat printen
    std::cout << "Sorted array: ";
    for (const auto& num : arr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}
