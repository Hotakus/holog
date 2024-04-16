/**
  ******************************************************************************
  * @file           : holog.c
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/4
  ******************************************************************************
  */

#include "holog.h"
#include <stdio.h>
int partition(int arr[], int low, int high);

// quickSort
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// partition
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return (i + 1);
}

int main() {
    // test quickSort
    int arr[] = { 10, 7, 8, 9, 1, 5 };
    int n = sizeof(arr) / sizeof(arr[0]);


    printf("Original Array:\n");
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);

    quickSort(arr, 0, n - 1);

    printf("\nSorted Array:\n");
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);

}
