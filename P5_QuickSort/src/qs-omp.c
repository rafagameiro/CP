#include "qs.h"
#include <omp.h>

int partition(void *base, size_t pos, size_t nel, size_t width, int (*compar)(const void *, const void *))
{
    void *x = base + (nel - 1) * width;
    int i = pos - 1;

    for(int j = pos; j < nel-1; j++)
    {
        if(compar(base + j * width, x) == -1 || compar(base + j * width, x) == 0)
        {
            i++;
            swap(base + i * width, base + j * width, width);
        }
    }
    i++;
    swap(base + i * width, base + (nel-1) * width, width);
    return i;
}

void qsort_par_impl(void *base, size_t pos, size_t nel, size_t width, int (*compar)(const void *, const void *))
{
    if( pos < nel ) {
        int q = partition(base, pos, nel, width, compar);
        #pragma omp task firstprivate(pos, q)
            qsort_par_impl(base, pos, q, width, compar);

        #pragma omp task firstprivate(pos, q)
            qsort_par_impl(base, q + 1, nel, width, compar);
    }
}


////////////////////////////////////////////////////////////////////////////////////////
/// Your own parallel implementation of Quick Sort using OpenMP
void QSORT(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *)) {
    // Your code goes here
    
    #pragma omp parallel
    #pragma omp single
    {
        qsort_par_impl(base, 0, nel, width, compar);
    }
}

