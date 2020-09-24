#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "patterns.h"

#define DEFAULT_STENCIL_IMP codeAndCacheFusionStencil
#define DEFAULT_SCATTER_IMP scatter

void map (void *dest, void *src, size_t nJob, size_t sizeJob, void (*worker)(void *v1, const void *v2)) {
    /* To be implemented */
    assert (dest != NULL);
    assert (src != NULL);
    assert (worker != NULL);
    char *d = dest;
    char *s = src;
    for (int i = 0;  i < nJob;  i++) {
        worker (&d[i * sizeJob], &s[i * sizeJob]);
    }
}

/***********************Stencil*************************/

/**
 * Stencil implementation that runs multiple maps
 */
void codeAndCacheFusionStencil (void *dest, void *src, size_t nJob, size_t sizeJob, size_t width, void (*worker)(void *v1, const void *v2, const void *v3)) {
    assert (dest != NULL);
    assert (src != NULL);
    assert (worker != NULL);

    for (int i = 0; i < nJob; i++)
        for (int inputSrc = 0; inputSrc < width; inputSrc++)
            worker(dest + i * sizeJob, dest + i * sizeJob, src + ((i + inputSrc) % nJob) * sizeJob);
}

/**
 * Stencil Implementation that applies a worker function to "width" adjacent cells (forward) using code fusion
 */
void codeFusionStencil (void *dest, void *src, size_t nJob, size_t sizeJob, size_t width, void (*worker)(void *v1, const void *v2, const void *v3)) {
    assert (dest != NULL);
    assert (src != NULL);
    assert (worker != NULL);

    for (int inputSrc = 0; inputSrc < width; inputSrc++) {
        for (int i = 0; i < nJob; i++) 
            worker(dest + i * sizeJob, dest + i * sizeJob, src + ((i + inputSrc) % nJob) * sizeJob);
    }
}

void noFusionStencil (void *dest, void *src, size_t nJob, size_t sizeJob, size_t width, void (*worker)(void *v1, const void *v2, const void *v3)) {
    assert (dest != NULL);
    assert (src != NULL);
    assert (worker != NULL);

        for (int inputSrc = 0; inputSrc < width; inputSrc++)
            for(int i = 0; i < nJob; i ++)
                worker(dest + i * sizeJob, dest + i * sizeJob, src + ((i + inputSrc) % nJob) * sizeJob);        
}

/**
 * Default Parallel Stencil Implementation
 */
void stencil (void *dest, void *src, size_t nJob, size_t sizeJob, size_t width, void (*worker)(void *v1, const void *v2, const void *v3)) {
    DEFAULT_STENCIL_IMP(dest, src, nJob, sizeJob, width, worker);
}

void reduce (void *dest, void *src, size_t nJob, size_t sizeJob, void (*worker)(void *v1, const void *v2, const void *v3)) {
    /* To be implemented */
    assert (dest != NULL);
    assert (src != NULL);
    assert (worker != NULL);
    char *d = dest;
    char *s = src;
    if (nJob > 0) {
        memcpy (&d[0], &s[0], sizeJob);
        for (int i = 1;  i < nJob;  i++)
            worker (&d[0], &d[0], &s[i * sizeJob]);
    }
}

void scan (void *dest, void *src, size_t nJob, size_t sizeJob, void (*worker)(void *v1, const void *v2, const void *v3)) {
    /* To be implemented */
    assert (dest != NULL);
    assert (src != NULL);
    assert (worker != NULL);
    char *d = dest;
    char *s = src;
    if (nJob > 1) {
        memcpy (&d[0], &s[0], sizeJob);
        for (int i = 1;  i < nJob;  i++)
            worker (&d[i * sizeJob], &d[(i-1) * sizeJob], &s[i * sizeJob]);
    }
}

int pack (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    /* To be implemented */
    assert (dest != NULL);
    assert (src != NULL);
    assert (filter != NULL);
    assert (nJob >= 0);
    assert (sizeJob > 0);
    char *d = dest;
    char *s = src;
    int pos = 0;
    for (int i=0; i < nJob; i++) {
        if (filter[i]) {
            memcpy (&d[pos * sizeJob], &s[i * sizeJob], sizeJob);
            pos++;
        }
    }
    return pos;
}

void gather (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter, int nFilter) {
    /* To be implemented */
    assert (dest != NULL);
    assert (src != NULL);
    assert (filter != NULL);
    assert (nJob >= 0);
    assert (sizeJob > 0);
    assert (nFilter >= 0);
    char *d = dest;
    char *s = src;
    for (int i=0; i < nFilter; i++) {
        assert (filter[i] < nJob);
        memcpy (&d[i * sizeJob], &s[filter[i] * sizeJob], sizeJob);
    }
}


/***********************Scatter*************************/

void lockScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    
    DEFAULT_SCATTER_IMP(dest, src, nJob, sizeJob, filter);
}

void lowOutputScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {

    DEFAULT_SCATTER_IMP(dest, src, nJob, sizeJob, filter);
}

void multiScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {

    DEFAULT_SCATTER_IMP(dest, src, nJob, sizeJob, filter);
}

void sequentialScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    
    DEFAULT_SCATTER_IMP(dest, src, nJob, sizeJob, filter);
}

void raceConditionsScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    DEFAULT_SCATTER_IMP(dest, src, nJob, sizeJob, filter);
}

void noOpScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {}

void scatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    /* To be implemented */
    assert (dest != NULL);
    assert (src != NULL);
    assert (filter != NULL);
    assert (nJob >= 0);
    assert (sizeJob > 0);
    char *d = dest;
    char *s = src;
    for (int i=0; i < nJob; i++) {
        assert (filter[i] < nJob);
        memcpy (&d[filter[i] * sizeJob], &s[i * sizeJob], sizeJob);
    }
}

void pipeline (void *dest, void *src, size_t nJob, size_t sizeJob, void (*workerList[])(void *v1, const void *v2), size_t nWorkers) {
    /* To be implemented */
    assert (dest != NULL);
    assert (src != NULL);
    assert (workerList != NULL);
    assert (nJob >= 0);
    assert (sizeJob > 0);
    char *d = dest;
    char *s = src;
    for (int i=0; i < nJob; i++) {
        memcpy (&d[i * sizeJob], &s[i * sizeJob], sizeJob);
        for (int j = 0;  j < nWorkers;  j++) {
            assert (workerList[j] != NULL);
            workerList[j] (&d[i * sizeJob], &d[i * sizeJob]);
        }
    }
}

void farm (void *dest, void *src, size_t nJob, size_t sizeJob, void (*worker)(void *v1, const void *v2), size_t nWorkers) {
    
    //map(dest, src, nJob, sizeJob, worker);
    
    int jobs = nJob / nWorkers;
    int start = 0;
    int end = jobs;
    for(int i = 0; i < nWorkers; i++, start+= jobs, end+= jobs) {
        if(i == nWorkers-1)
            end = nJob;
        for(int j = start;  j < end; j++)   
            worker(dest + j * sizeJob, src + j * sizeJob);
    }
}

