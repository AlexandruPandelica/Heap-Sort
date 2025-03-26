#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

// Functia pentru a rearanja subarborele astfel incat sa respecte proprietatea de heap
void heapify(int arr[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

// Functia principala pentru Heap Sort
void heapSort(int arr[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

// Functia pentru afisarea unui array
void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
    cout << endl;
}

// Functie pentru masurarea timpului de rulare a Heap Sort
void measureExecutionTime(int arr[], int n) {
    auto start = high_resolution_clock::now();
    heapSort(arr, n);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Timpul de executie: " << duration.count() << " microsecunde" << endl;
}

int main() {
    int arr[3000];
    for (int i = 0; i < 3000; i++) {
        arr[i] = rand() % 100; // Generam valori random intre 0 si 99
    }
    int n = sizeof(arr) / sizeof(arr[0]);

    cout << "Vectorul initial: ";
    printArray(arr, n);
    cout << endl;
    measureExecutionTime(arr, n);
    cout << endl;
    cout << "Vectorul sortat: ";
    printArray(arr, n);

    return 0;
} 


/// Cele 30 de elemente:
/// 12, 11, 13, 5, 6, 7, 10, 2, 41, 82, 27, 22, 38, 55, 9, 18, 30, 1, 88, 60, 46, 33, 77, 50, 99, 20, 74, 89, 70, 64, 80
 
/// Cele 100 de elemente:
/// 12, 11, 13, 5, 6, 7, 10, 2, 41, 82, 27, 22, 38, 55, 9, 18, 30, 1, 88, 60, 46, 33, 77, 50, 99,
/// 14, 67, 23, 89, 45, 72, 16, 81, 92, 37, 3, 100, 56, 95, 25, 62, 78, 15, 35, 70, 40, 48, 66, 4, 21,
/// 80, 29, 57, 98, 17, 44, 31, 53, 26, 24, 34, 96, 43, 65, 91, 58, 32, 63, 86, 8, 54, 36, 85, 47, 94,
/// 71, 52, 75, 68, 39, 97, 79, 49, 59, 90, 19, 76, 20, 73, 64, 61, 74, 87, 28, 42, 93, 83, 84, 69, 51

/// Cele 150 de elemente:
/// 12, 11, 13, 5, 6, 7, 10, 2, 41, 82, 27, 22, 38, 55, 9, 18, 30, 1, 88, 60, 46, 33, 77, 50, 99,
/// 14, 67, 23, 89, 45, 72, 16, 81, 92, 37, 3, 100, 56, 95, 25, 62, 78, 15, 35, 70, 40, 48, 66, 4, 21,
/// 80, 29, 57, 98, 17, 44, 31, 53, 26, 24, 34, 96, 43, 65, 91, 58, 32, 63, 86, 8, 54, 36, 85, 47, 94,
/// 71, 52, 75, 68, 39, 97, 79, 49, 59, 90, 19, 76, 20, 73, 64, 61, 74, 87, 28, 42, 93, 83, 84, 69, 51,
/// 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
/// 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
/// 141, 142, 143, 144, 145, 146, 147, 148, 149, 150

/* int main() {
int arr[] = { 12, 11, 13, 5, 6, 7, 10, 2, 41, 82, 27, 22, 38, 55, 9, 18, 30, 1, 88, 60, 46, 33, 77, 50, 99,
              14, 67, 23, 89, 45, 72, 16, 81, 92, 37, 3, 100, 56, 95, 25, 62, 78, 15, 35, 70, 40, 48, 66, 4, 21,
              80, 29, 57, 98, 17, 44, 31, 53, 26, 24, 34, 96, 43, 65, 91, 58, 32, 63, 86, 8, 54, 36, 85, 47, 94,
              71, 52, 75, 68, 39, 97, 79, 49, 59, 90, 19, 76, 20, 73, 64, 61, 74, 87, 28, 42, 93, 83, 84, 69, 51,
              101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
              121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
              141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
              161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
              181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200 };
int n = sizeof(arr) / sizeof(arr[0]);

cout << "Vectorul initial: ";
printArray(arr, n);
cout << endl;
measureExecutionTime(arr, n);
cout << endl;
cout << "Vectorul sortat: ";
printArray(arr, n);

return 0;
} */


/*
int main() {
    int arr[1000];
    for (int i = 0; i < 1000; i++) {
        arr[i] = rand() % 10000; // Generam valori random intre 0 si 9999
    }
    int n = sizeof(arr) / sizeof(arr[0]);

    cout << "Vectorul initial: ";
    printArray(arr, n);
    cout << endl;
    measureExecutionTime(arr, n);
    cout << endl;
    cout << "Vectorul sortat: ";
    printArray(arr, n);

    return 0;
} */
