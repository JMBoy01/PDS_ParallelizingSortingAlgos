// How to compile: g++ -o quicksort quicksort.cpp
// How to run: ./parallel_quicksort.exe "file_path/file.dat"

#include <stdio.h>
#include <vector>
#include "data_manager.h"

using namespace std;

void sort(vector<int>& list, int left, int right) {
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
        sort(list, left, R);
    }
    if (L < right) {
        sort(list, L, right);
    }
}

void quicksort(vector<int>& list) {
    sort(list, 0, list.size() - 1);  // Array, left en right meegeven
}

bool check_sorted(vector<int>& vector, int& error_amount)
{
    bool is_sorted = true;

    #pragma omp parallel for shared(is_sorted, error_amount)
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
    vector<int> test_vector{};
    std::string file_path;

    if (argc > 1) {
        file_path = argv[1]; // Eerste argument (argv[0] is de exe)
        // std::cout << file_path << std::endl;
    } // Lees file in als er een file path gegeven is
    if (!file_path.empty()) {
        test_vector = load_dataset(file_path);
    } // Als de vector nog leeg is na het inlezen dan maak een random vector
    if (test_vector.empty()) {
        // test_vector = generate_dataset(100000);
        // shuffle_dataset(test_vector);

        test_vector = generate_random_dataset(100000);

        save_dataset(test_vector, "generated_dataset.dat");
    }

    quicksort(test_vector);

    int error_amount = 0;
    bool result = check_sorted(test_vector, error_amount);

    printf("\nIs array sorted: %d\nAmount of errors: %d\n", result, error_amount); // 1 = true -> correct gesorteerd, 0 = false -> niet correct gesorteerd

    // printf("test_vector: ");
    // print_vector(test_vector);

    return 0;
}