#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>

#include <xmmintrin.h>

#define TEST_W 4096
#define TEST_H 4096

/* provide the implementations of naive_transpose,
 * sse_transpose, sse_prefetch_transpose
 */

#include "impl.c"

typedef struct matrix_impl Matrix;
struct matrix_impl {
    void (*transpose) (int *src, int *dst, int w, int h);
};

static long diff_in_us(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000.0 + diff.tv_nsec / 1000.0);
}

int main()
{

    struct timespec start, end;
    int *src  = (int *) malloc(sizeof(int) * TEST_W * TEST_H);

#ifdef NAIVE
    Matrix m = {
        .transpose = naive_transpose,
    };
#endif

#ifdef SSE
    Matrix m = {
        .transpose = sse_transpose,
    };
#endif

#ifdef SSE_PREFETCH
    Matrix m = {
        .transpose = sse_prefetch_transpose,
    };
#endif

    srand(time(NULL));
    for (int y = 0; y < TEST_H; y++)
        for (int x = 0; x < TEST_W; x++)
            *(src + y * TEST_W + x) = rand();

    int *out = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
    clock_gettime(CLOCK_REALTIME, &start);
    m.transpose(src, out, TEST_W, TEST_H);
    clock_gettime(CLOCK_REALTIME, &end);
    printf("Execution time: \t %ld us\n", diff_in_us(start, end));
    free(out);
    free(src);

    return 0;
}
