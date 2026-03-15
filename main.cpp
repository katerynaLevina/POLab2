#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;

const int DIVISOR = 17;


void runLinear(int* data, int N) {
    long long count = 0;
    int min_val = numeric_limits<int>::max();

    auto start_time = chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        if (data[i] % DIVISOR == 0) {
            count++;
            if (data[i] < min_val) min_val = data[i];
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_sec = end_time - start_time;

    cout << "--- Linear ---\n";
    cout << "Result: Count = " << count << " | Min = " << min_val << "\n";
    cout << "Execution time: "  << duration_sec.count() << " s\n\n";
}

void runMutex(int* data, int N, int numThreads) {
    long long global_count = 0;
    int global_min = numeric_limits<int>::max();
    mutex mtx;

    thread* threads = new thread[numThreads];
    int chunkSize = N / numThreads;

    auto start_time = chrono::high_resolution_clock::now();

    for (int t = 0; t < numThreads; ++t) {
        int start_idx = t * chunkSize;
        int end_idx = (t == numThreads - 1) ? N : (t + 1) * chunkSize;

        threads[t] = thread([&, start_idx, end_idx]() {
            for (int i = start_idx; i < end_idx; ++i) {

                if (data[i] % DIVISOR == 0) {
                    lock_guard<mutex> lock(mtx);
                    global_count++;
                    if (data[i] < global_min) global_min = data[i];
                }
            }
        });
    }

    for (int i = 0; i < numThreads; ++i) {
        threads[i].join();
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_sec = end_time - start_time;

    cout << "--- Mutex (" << numThreads << " threads) ---\n";
    cout << "Result: Count = " << global_count << " | Min = " << global_min << "\n";
    cout << "Execution time: "  << duration_sec.count() << " s\n\n";

    delete[] threads;
}

void runCAS(int* data, int N, int numThreads) {
    atomic<long long> atomic_count(0);
    atomic<int> atomic_min(numeric_limits<int>::max());

    thread* threads = new thread[numThreads];
    int chunkSize = N / numThreads;

    auto start_time = chrono::high_resolution_clock::now();

    for (int t = 0; t < numThreads; ++t) {
        int start_idx = t * chunkSize;
        int end_idx = (t == numThreads - 1) ? N : (t + 1) * chunkSize;

        threads[t] = thread([&, start_idx, end_idx]() {
            for (int i = start_idx; i < end_idx; ++i) {
                if (data[i] % DIVISOR == 0) {

                    long long current_count = atomic_count.load(memory_order_relaxed);
                    while (!atomic_count.compare_exchange_weak(current_count, current_count + 1, memory_order_relaxed)) {
                    }

                    int current_min = atomic_min.load(memory_order_relaxed);
                    while (data[i] < current_min &&
                           !atomic_min.compare_exchange_weak(current_min, data[i], memory_order_relaxed)) {
                    }
                }
            }
        });
    }

    for (int i = 0; i < numThreads; ++i) {
        threads[i].join();
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_sec = end_time - start_time;

    cout << "--- CAS (" << numThreads << " threads) ---\n";
    cout << "Result: Count = " << atomic_count << " | Min = " << atomic_min << "\n";
    cout << "Execution time: "  << duration_sec.count() << " s\n\n";

    delete[] threads;
}

int main() {
    int N = 1000000;
    int numThreads = 8;

    cout << "Initializing array of size " << N << "...\n\n";
    int* data = new int[N];
    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        data[i] = rand() % 10000;
    }


    runLinear(data, N);
    runMutex(data, N, numThreads);
    runCAS(data, N, numThreads);

    delete[] data;

    return 0;
}