#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
// TODO openmp task, niet voor mergesort wel te bekijken voor Quicksort; VSC -> beperken tot 4 nodes bij mpi (credits)
// openmp 72 cores
// In analys: speedup berekenen en hoever zitten we er vanaf, plotten  
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

void mergeSort(std::vector<int>& array) {
    if (array.size() <= 1) return;

    size_t mid = array.size() / 2;
    std::vector<int> left(array.begin(), array.begin() + mid);
    std::vector<int> right(array.begin() + mid, array.end());

    mergeSort(left);
    mergeSort(right);
    array = merge(left, right);
}


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<int> array{
        56, 23, 78, 12, 45, 67, 89, 34, 92, 11, 38, 49, 26, 90, 72, 6, 83, 39, 18, 59,
        99, 3, 87, 65, 21, 43, 50, 29, 74, 94, 70, 10, 81, 55, 66, 48, 77, 53, 28, 7,
        96, 2, 60, 19, 35, 62, 85, 31, 100, 41, 37, 73, 1, 84, 5, 63, 17, 97, 25, 76,
        4, 61, 15, 69, 52, 91, 33, 40, 20, 75, 24, 64, 93, 47, 27, 68, 9, 95, 42, 8,
        82, 30, 13, 57, 44, 22, 80, 46, 14, 88, 51, 86, 58, 36, 16, 98, 71, 54, 79
    };

    const int N = array.size();
    int chunk_size = N / size;
    std::vector<int> local_array(chunk_size);

    
    double start_time = MPI_Wtime();

    // scatter data to all processes
    MPI_Scatter(array.data(), chunk_size, MPI_INT, local_array.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // each process sorts its local array
    mergeSort(local_array);

    // gather sorted subarrays back at the root process
    std::vector<int> sorted_array;
    if (rank == 0) {
        sorted_array.resize(N);
    }
    MPI_Gather(local_array.data(), chunk_size, MPI_INT, sorted_array.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // the main thread merges all subarrays back together
    if (rank == 0) {
        size_t offset = chunk_size;
        while (offset < sorted_array.size()) {
            std::vector<int> chunk(sorted_array.begin(), sorted_array.begin() + offset);
            std::vector<int> next_chunk(sorted_array.begin() + offset, sorted_array.begin() + 2 * offset);
            sorted_array = merge(chunk, next_chunk);
            offset *= 2;
        }

        double end_time = MPI_Wtime();

        std::cout << "Time taken: " << end_time - start_time << " seconds" << std::endl;
        std::cout << "Sorted Array: ";
        for (size_t i = 0; i < sorted_array.size(); ++i) {
            std::cout << sorted_array[i] << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
    return 0;
}
