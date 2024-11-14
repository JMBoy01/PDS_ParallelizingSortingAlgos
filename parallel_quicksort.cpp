// How to compile: g++ -g -Wall -fopenmp -o parallel_quicksort parallel_quicksort.cpp

#include <stdio.h>
#include <vector>
#include <omp.h>

using namespace std;

void Sort(vector<int>& list, int left, int right) {
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

    // Only make extra threads if sub array is large enough in size
    int threshold = 25;
    if (left < R) {
        if (R - left > threshold) {
            printf("Making extra thread -> R - left > threshold: %d - %d = %d > %d\n", R, left, R-left, threshold);
        }

        #pragma omp task shared(list) if (R - left > threshold)  // Set threshold
        Sort(list, left, R);
    }
    if (L < right) {
        if (right - L > threshold) {
            printf("Making extra thread -> right - L > threshold: %d - %d = %d > %d\n", right, L, right-L, threshold);
        }

        #pragma omp task shared(list) if (right - L > threshold)
        Sort(list, L, right);
    }
}

void QuickSort(vector<int>& list) {
    #pragma omp parallel
    {
        #pragma omp single  // Start by making 1 thread
        Sort(list, 0, list.size() - 1);
    }
}

int main() {
    vector<int> list{
        56, 23, 78, 12, 45, 67, 89, 34, 92, 11, 38, 49, 26, 90, 72, 6, 83, 39, 18, 59,
        99, 3, 87, 65, 21, 43, 50, 29, 74, 94, 70, 10, 81, 55, 66, 48, 77, 53, 28, 7,
        96, 2, 60, 19, 35, 62, 85, 31, 100, 41, 37, 73, 1, 84, 5, 63, 17, 97, 25, 76,
        4, 61, 15, 69, 52, 91, 33, 40, 20, 75, 24, 64, 93, 47, 27, 68, 9, 95, 42, 8,
        82, 30, 13, 57, 44, 22, 80, 46, 14, 88, 51, 86, 58, 36, 16, 98, 71, 54, 79
    };

    QuickSort(list);

    printf("list: [");
    for (int val : list) {
        printf("%d, ", val);
    }
    printf("]");

    return 0;
}
