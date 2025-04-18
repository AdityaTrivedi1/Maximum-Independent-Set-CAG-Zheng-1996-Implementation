#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#pragma clang diagnostic ignored "-Wc++11-extensions"
#define NUM_ARCS 5
#define NUM_POINTS 6
using namespace std;

int* get_random_arc();
int get_random();
void pass_time(int);
int main() {
    int* arc = NULL;
    cout << NUM_ARCS << endl;
    for (int z = 0; z < NUM_ARCS; z++) {
        arc = get_random_arc();
        cout << arc[0] << " " << arc[1] << endl;
        pass_time(arc[1]);
    }
    return 0;
}
int* get_random_arc() {
    int* result = (int*)calloc(2, sizeof(int));
    int milliseconds = get_random();
    result[0] = milliseconds;
    pass_time(milliseconds);
    milliseconds = get_random();
    result[1] = milliseconds;
    return result;
}
int get_random() {
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    int milliseconds = chrono::duration_cast<chrono::milliseconds>(duration).count() % (NUM_POINTS);
    return milliseconds;
}
void pass_time(int mod) {
    long long random = rand() % mod + 100;
    for (int y = 0; y < random; y++) rand();
    random = (rand() % 10000 + 100) * (rand() % 100000 + 1000);
    for (int y = 0; y < random; y++);
}

