#include <stdio.h>
#include <time.h>

int main() {
    clock_t start, end;

    start = clock();

    long long result = 0;

    for (int i = 0; i < 1000000; i++)
        result += i;

    end = clock();

    double time_elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("The sum is: %lld\nCompiler Execution Time: %.4f seconds", result, time_elapsed);
    return 0;
}