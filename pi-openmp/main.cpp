#include<omp.h>
#include<iostream>
#include <sys/time.h>

#define X 200000000

using namespace std;

double get_time() {
    return time(nullptr);
}

double f1(double x) {
    return 4 / (1 + x * x);
}

double calc_pi() {
    double dx = 0.00000001;
    long L = (long) (1 / dx);
    double sum = 0.0;
    for (long x = 0; x < L; x++) {
        sum += f1(x * dx) * dx;
    }
    return sum;
}

double calc_pi3() {
    double dx = 0.00000001;
    long L = (long) (1 / dx);
    double sum = 0.0;
#pragma omp parallel for reduction(+:sum)
    for (long x = 0; x < L; x++) {
        sum += f1(x * dx) * dx;
    }
    return sum;
}

double calc_pi4() {
    double dx;
    long L = (long) (1 / dx);
    double tmp_sum;
    double sum[] = {0, 0, 0, 0, 0, 0, 0, 0};
    int n, k;
    long x;
#pragma omp parallel default(none) private(n, k, L, x, dx, tmp_sum) shared(sum)
    {
        dx = 0.00000001;
        n = omp_get_thread_num();
        k = omp_get_num_threads();
        L = (long) (1 / dx);
        tmp_sum = 0;
        for (x = n; x < L - n; x += k) {
            tmp_sum += f1(x * dx) * dx;
        }
        sum[n] = tmp_sum;
    }

    double ret = 0.0;
    for (int i = 0; i < 8; i++) {
        ret += sum[i];
    }
    return ret;
}

inline double rand_0_1() {
    return (rand() / double(RAND_MAX));
}

inline double randr_0_1(unsigned int *seed) {
    return (rand_r(seed) / double(RAND_MAX));
}

double calc_pi_circle() {
    long in = 0;
    for (int i = 0; i < X; i++) {
        double x = rand_0_1();
        double y = rand_0_1();
        if (x * x + y * y < 1.0) in++;
    }

    return 4.0 * in / X;
}

double calc_pi_circle2() {
    long in = 0;
    long size = X;
    double x, y;
    unsigned int seed;


#pragma omp parallel private(seed) reduction(+:in)
    {
        seed = 17 * omp_get_thread_num();
#pragma omp for private(x, y)
        for (int i = 0; i < size; i++) {
            int n = omp_get_thread_num();
            x = randr_0_1(&seed);
            y = randr_0_1(&seed);
            if (x * x + y * y < 1.0) ++in;
        }
    }
    return 4.0 * in / X;
}


void print_calc(string name, double (*calc)(void)) {
    struct timeval start, end;
    double pi;
    gettimeofday(&start, NULL);
    pi = calc();
    gettimeofday(&end, NULL);
    double delta = ((end.tv_sec - start.tv_sec) * 1000000u +
                    end.tv_usec - start.tv_usec) / 1.e6;
    cout << name << endl;
    printf("pi=%f in %.2fs\n\n", pi, delta);
}

int main() {
    srand(time(nullptr));
    print_calc("NOT PARALLEL", calc_pi);
    print_calc("PARALLEL (openMP IMPLEMENTATION)", calc_pi3);
    print_calc("PARALLEL (OWN IMPLEMENTATION)", calc_pi4);
    print_calc("CIRCLE - NOT PARALLEL", calc_pi_circle);
    print_calc("CIRCLE - PARALLEL", calc_pi_circle2);
    return 0;
}













































