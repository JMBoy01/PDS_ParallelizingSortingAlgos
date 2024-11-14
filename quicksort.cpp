#include <stdio.h>
#include <vector>

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

    if (left < R) {
        Sort(list, left, R);
    }
    if (L < right) {
        Sort(list, L, right);
    }
}

void QuickSort(vector<int>& list) {
    Sort(list, 0, list.size() - 1);  // Array, left en right meegeven
}

int main()
{
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