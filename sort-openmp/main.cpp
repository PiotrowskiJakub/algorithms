#include <iostream>
#include <stdlib.h>
#include <random>
#include <omp.h>
#include <cstring>
#include <sys/time.h>

#define L 10000
using namespace std;

template<class T>
void swap(T *x, T *y) {
    T tmp = *x;
    *x = *y;
    *y = tmp;
}

template<class T>
void print(T *start, T *end) {
    if (*end > 100) {
        *end = 100;
    }
    for (T *x = start; x != end; x++) {
        cout << *x << " ";
    }
    cout << endl;
}

void sort(int *start, int *end) {
    for (int *x = start; x != end - 1; x++) {
        for (int *y = x + 1; y != end; y++) {
            if (*x > *y) {
                swap(x, y);
            }
        }
    }
}

int *calc_start(int *start, int *end, int thread_num, int num_threads) {
    return start + thread_num * ((end - start) / num_threads);
}

int *calc_end(int *start, int *end, int thread_num, int num_threads) {
    if (thread_num == num_threads - 1) return end;
    else {
        unsigned long end_offset = (thread_num + 1) * ((end - start) / num_threads);
        return start + end_offset;
    }
}

void sort_parallel(int *start, int *end) {
    int num_threads = 0;
#pragma omp parallel shared(num_threads)
    {
        num_threads = omp_get_num_threads();
    }

#pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        int *start_part = calc_start(start, end, thread_num, num_threads);
        int *end_part = calc_end(start, end, thread_num, num_threads);
        sort(start_part, end_part);
    }

    vector<int *> heads(num_threads);
    int *sorted = new int[end - start];
    for (int n = 0; n < num_threads; n++) heads[n] = calc_start(start, end, n, num_threads);
    unsigned long size = (end - start);
    for (unsigned long i = 0; i < size; i++) {
        int min_n = 0;
        int *min = NULL;
        for (int n = 0; n < num_threads; n++) {
            if (heads[n] == calc_end(start, end, n, num_threads)) continue;
            if (min == NULL || *min > *heads[n]) {
                min = heads[n];
                min_n = n;
            }
        }
        sorted[i] = *min;
        heads[min_n]++;
    }
    memcpy(start, sorted, sizeof(int) * (end - start));
    delete sorted;
}

int rand(int min, int max) {
    return rand() % (max - min) + min;
}

void randomize(int *start, int *end) {
    for (int *x = start; x != end; x++) {
        *x = rand(0, L);
    }
}

void print_calc(string name, void (*calc)(int *, int *), int *tab0, int *tabEnd) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    calc(tab0, tabEnd);
    gettimeofday(&end, NULL);
    double delta = ((end.tv_sec - start.tv_sec) * 1000000u +
                    end.tv_usec - start.tv_usec) / 1.e6;
    cout << endl << name << ": ";
    printf("sorted in %.2fs\n", delta);
}

int main() {
    srand(time(NULL));

    int *tab = new int[L];
    int *tab2 = new int[L];

    randomize(tab, tab + L);
    memcpy(tab2, tab, sizeof(int) * L);

    print(tab, tab + L);
    print(tab2, tab2 + L);

    print_calc("NOT PARALLEL", sort, tab, tab + L);
    print_calc("PARALLEL", sort_parallel, tab2, tab2 + L);

    print(tab, tab + L);
    print(tab2, tab2 + L);

    return 0;
}
































