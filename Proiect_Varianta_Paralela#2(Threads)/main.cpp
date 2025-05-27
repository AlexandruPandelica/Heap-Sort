#include <iostream>
#include <fstream>
#include <thread>
#include <algorithm>
#include <random>
#include <chrono>
#include <vector>
#include <string>

using namespace std;

const int N = 300000;
const int NUM_THREADS = 4;
int inputData[N];

void sort_and_write(int* data, int start, int end, int thread_id) {
    sort(data + start, data + end);
    ofstream fout("output_thread_" + to_string(thread_id) + ".txt");
    for (int i = start; i < end; ++i) {
        fout << data[i] << "\n";
    }
    fout.close();
}

vector<int> read_thread_output(int thread_id) {
    ifstream fin("output_thread_" + to_string(thread_id) + ".txt");
    vector<int> result;
    int val;
    while (fin >> val) {
        result.push_back(val);
    }
    return result;
}

int main() {
    // Initializare aleatoare a datelor
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 99999);
    for (int i = 0; i < N; i++)
        inputData[i] = dis(gen);

    auto start_time = chrono::high_resolution_clock::now();

    int chunk_size = N / NUM_THREADS;
    thread threads[NUM_THREADS];

    // Sortare si scriere in fisiere
    for (int i = 0; i < NUM_THREADS; i++) {
        int start = i * chunk_size;
        int end = (i == NUM_THREADS - 1) ? N : start + chunk_size;
        threads[i] = thread(sort_and_write, inputData, start, end, i);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        threads[i].join();

    // Citirea si combinarea tuturor rezultatelor
    vector<int> all_data;
    for (int i = 0; i < NUM_THREADS; i++) {
        vector<int> partial = read_thread_output(i);
        all_data.insert(all_data.end(), partial.begin(), partial.end());
    }

    // Sortare globala
    sort(all_data.begin(), all_data.end());

    // Scriere in fisier final
    ofstream fglobal("output_final.txt");
    fglobal << "Vector sortat complet (" << all_data.size() << " elemente):\n";
    for (int x : all_data)
        fglobal << x << "\n";
    fglobal.close();

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;
    cout << "Timp executie (threads + IO): " << elapsed.count() << " secunde\n";

    return 0;
}
