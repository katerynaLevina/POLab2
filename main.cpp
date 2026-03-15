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



int main() {
    int N = 10000000;


    cout << "Initializing array of size " << N << "...\n\n";
    int* data = new int[N];
    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        data[i] = rand() % 10000;
    }


    runLinear(data, N);



    delete[] data;

    return 0;
}