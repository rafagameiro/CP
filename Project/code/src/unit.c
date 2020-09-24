#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "patterns.h"
#include "debug.h"
#include "unit.h"

#define TYPE double
#define FMT "%lf"
#define MAX_TESTS 30
#define HEAVY_LENGTH 100
#define STENCIL_WIDTH 10

#define SMALL_OUTPUT 10
#define LARGE_OUTPUT 100000

//=======================================================
// Workers
//=======================================================

/*
static void workerMax(void* a, const void* b, const void* c) {
    // a = max (b, c)
    *(TYPE *)a = *(TYPE *)b;
    if (*(TYPE *)c > *(TYPE *)a)
        *(TYPE *)a = *(TYPE *)c;
}

static void workerMin(void* a, const void* b, const void* c) {
    // a = min (b, c)
    *(TYPE *)a = *(TYPE *)b;
    if (*(TYPE *)c < *(TYPE *)a)
        *(TYPE *)a = *(TYPE *)c;
}
*/

static void workerAdd(void* a, const void* b, const void* c) {
    // a = b + c
    *(TYPE *)a = *(TYPE *)b + *(TYPE *)c;
}

static void workerHeavyAdd(void* a, const void* b, const void* c) {
    for(size_t i = 0; i < HEAVY_LENGTH; i++);
    *(TYPE *)a = *(TYPE *)b + *(TYPE *)c;
}

/*
static void workerSubtract(void* a, const void* b, const void* c) {
    // a = n - c
    *(TYPE *)a = *(TYPE *)b - *(TYPE *)c;
}

static void workerMultiply(void* a, const void* b, const void* c) {
    // a = b * c
    *(TYPE *)a = *(TYPE *)b + *(TYPE *)c;
}
*/

static void workerDivide(void* a, const void* b, const void* c) {
    // a = b * c
    *(TYPE *)a = *(TYPE *)b / *(TYPE *)c;
}

static void workerAddOne(void* a, const void* b) {
    // a = b + 1
    *(TYPE *)a = *(TYPE *)b + 1;
}

static void workerAddTwo(void* a, const void* b) {
    // a = b + 2
    *(TYPE *)a = *(TYPE *)b + 2;
}

static void workerHeavyAddTwo(void* a, const void* b) {
    for(size_t i = 0; i < HEAVY_LENGTH; i++);
    *(TYPE *)a = *(TYPE *)b + 2;
}

static void workerMultTwo(void* a, const void* b) {
    // a = b * 2
    *(TYPE *)a = *(TYPE *)b * 2;
}

static void workerDivTwo(void* a, const void* b) {
    // a = b / 2
    *(TYPE *)a = *(TYPE *)b / 2;
}

//=======================================================
// Unit testing funtions
//=======================================================

int *filter;

void generateFilter(size_t n) {
    srand(time(NULL));
    srand48(time(NULL));
    filter = calloc(n,sizeof(*filter));
}

//=======================================================
// General Test
//=======================================================

void generalTrenaryOperandTest (void *src, size_t inputSize, size_t outputSize, size_t size, 
        void (*worker)(void *v1, const void *v2, const void *v3), 
        void (*pattern) (void *dest, void *src, size_t n, size_t size, void (*worker)(void *v1, const void *v2, const void *v3))) {
    TYPE *dest = malloc(outputSize * size);
    pattern((void *)dest, src, inputSize, size, worker);
    printDouble (dest, outputSize, __FUNCTION__);
    free(dest);
}

//=======================================================
// Map tests
//=======================================================

void testMapGeneral (void *src, size_t n, size_t size, void (*worker)(void *v1, const void *v2)) {
    TYPE *dest = malloc(n * size);
    map((void *)dest, src, n, size, worker);
    printDouble (dest, n, __FUNCTION__);
    free(dest);
}

void testMapSum (void *src, size_t n, size_t size) {
    testMapGeneral(src, n, size, workerAddTwo);
}

void testMapHeavy (void *src, size_t n, size_t size) {
    testMapGeneral(src, n, size, workerHeavyAddTwo);
}

//=======================================================
// Stencil tests
//=======================================================

void testStencil(void *src, size_t n, size_t size, 
        void (*pattern) (void *dest, void *src, size_t nJob, size_t sizeJob, size_t width, void (*worker)(void *v1, const void *v2, const void *v3))) {
    size_t width = STENCIL_WIDTH;
    TYPE *dest = calloc(n, size);
    pattern((void *)dest, src, n, size, width, workerAdd);
    printDouble (dest, n, __FUNCTION__);
    free(dest);
}

void defaultStencilTest(void *src, size_t n, size_t size) {
    testStencil(src, n, size, stencil);
}

void testCodeFusionStencil(void *src, size_t n, size_t size) {
    testStencil(src, n, size, codeFusionStencil);
}

void testCodeAndCacheFusionStencil(void *src, size_t n, size_t size) {
    testStencil(src, n, size, codeAndCacheFusionStencil);
}

void testNoFusionStencil(void *src, size_t n, size_t size) {
    testStencil(src, n, size, noFusionStencil);
}

//=======================================================
// Reduce tests
//=======================================================

void testReduceGeneral(void *src, size_t n, size_t size, void (*worker)(void *v1, const void *v2, const void *v3)) {
    generalTrenaryOperandTest(src, n, 1, size, worker, reduce);
}

void testReduceSum (void *src, size_t n, size_t size) {
    testReduceGeneral(src, n, size, workerAdd);
}

void testReduceDiv (void *src, size_t n, size_t size) {
    testReduceGeneral(src, n, size, workerDivide);
}

//=======================================================
// Scan tests
//=======================================================

void testScanGeneral(void *src, size_t n, size_t size, void (*worker)(void *v1, const void *v2, const void *v3)) {
    generalTrenaryOperandTest(src, n, n, size, worker, scan);    
}

void testScanSum (void *src, size_t n, size_t size) {
    testScanGeneral(src, n, size, workerAdd);
}

void testScanHeavy (void *src, size_t n, size_t size) {
    testScanGeneral(src, n, size, workerHeavyAdd);
}

//=======================================================
// Pack tests
//=======================================================

void testPackGeneral (void *src, size_t n, size_t size, int nFilter) {
    TYPE *dest = calloc (nFilter, sizeof(size));
    generateFilter(n);
    for (int i = 0;  i < n;  i++)
        filter[i] = (i == 0 || i == n/2 || i == n-1);
    pack (dest, src, n, size, filter); 
    printInt (filter, n, "filter");    
    printDouble (dest, nFilter, __FUNCTION__);
    free(filter);
    free (dest);
}

void testPackDest3 (void *src, size_t n, size_t size) {
    testPackGeneral(src, n, size, 3);
}

void testPackDest10 (void *src, size_t n, size_t size) {
    testPackGeneral(src, n, size, 10);
}

void testPackFilterBelow10 (void *src, size_t n, size_t size) {
    int nFilter = 0; 
    generateFilter(n);
    for (int i = 0;  i < n;  i++) {
        filter[i] = (drand48() < 0.1);
        nFilter = filter[i] ? nFilter +1 : nFilter;
    }
    TYPE *dest = calloc (nFilter, sizeof(size));
    pack (dest, src, n, size, filter); 
    printInt (filter, n, "filter");    
    printDouble (dest, nFilter, __FUNCTION__);
    free(filter);
    free (dest);
}

//=======================================================
// Gather tests
//=======================================================

void testGatherGeneral(void *src, size_t n, size_t size, int nFilter) {
    TYPE *dest = malloc (nFilter * size);
    generateFilter(nFilter);
    for(int i = 0; i < nFilter; i++)
        filter[i] = rand() % n;
    printInt (filter, nFilter, "filter");    
    gather (dest, src, n, size, filter, nFilter);    
    printDouble (dest, nFilter, __FUNCTION__);
    free(filter);
    free (dest);
}

void testGatherFilter3 (void *src, size_t n, size_t size) {
    testGatherGeneral(src, n, size, 3);
}

void testGatherFilter20 (void *src, size_t n, size_t size) {
    testGatherGeneral(src, n, size, 20);
}

void testGatherFilter100 (void *src, size_t n, size_t size) {
    testGatherGeneral(src, n, size, 100);
}

//=======================================================
// Scatter tests
//=======================================================

void testScatter(void *src, size_t n, size_t size, int nDest, int canCompleteFilter,
        void (*pattern) (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter)) {
      TYPE *dest = malloc (nDest * size);
    memset (dest, 0, nDest * size);
    generateFilter(n);
    int maxFilter = canCompleteFilter ? nDest : nDest - 1;
    for (int i = 0;  i < n;  i++)
        filter[i] = rand() % maxFilter;
    printInt (filter, n, "filter");    
    pattern (dest, src, n, size, filter);    
    printDouble (dest, nDest, __FUNCTION__);
    free(filter);
    free (dest);      
}

void defaultScatterTest(void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 1, scatter);
}

void testBaseScatterLikelyDestCache (void *src, size_t n, size_t size) {
    testScatter(src, n, size, SMALL_OUTPUT, 1, sequentialScatter);
}

void testBaseScatterUnlikelyDestCache (void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 1, sequentialScatter);
}

void testNoOpScatterSmallOutput (void *src, size_t n, size_t size) {
    testScatter(src, n, size, SMALL_OUTPUT, 1, noOpScatter);
}

void testNoOpScatterLargeOutput (void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 1, noOpScatter);
}

void testLowOutputScatterSmallOutput (void *src, size_t n, size_t size) {
    testScatter(src, n, size, SMALL_OUTPUT, 1, lowOutputScatter);
}

void testLowOutputScatterLargeOutputComplete (void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 1, lowOutputScatter);
}

void testLowOutputScatterLargeOutputIncomplete(void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 0, lowOutputScatter);
}

void testMultiscatterSmallOutput (void *src, size_t n, size_t size) {
    testScatter(src, n, size, SMALL_OUTPUT, 1, multiScatter);
}

void testMultiscatterLargeOutputComplete (void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 1, multiScatter);
}

void testMultiscatterLargeOutputIncomplete (void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 0, multiScatter);
}

void testLockScatterLotOfConcurrency (void *src, size_t n, size_t size) {
    testScatter(src, n, size, SMALL_OUTPUT, 0, lockScatter);
}

void testLockScatterSmallConcurrencyComplete (void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 1, lockScatter);
}

void testLockScatterSmallConcurrencyIncomplete (void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 0, lockScatter);
}

void testRaceConditionsScatterUnlikelyCache(void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 1, raceConditionsScatter);
}

void testRaceConditionsScatterLikelyCache(void *src, size_t n, size_t size) {
    testScatter(src, n, size, LARGE_OUTPUT, 1, raceConditionsScatter);
}

//=======================================================
// Pipeline tests
//=======================================================

void testPipelineGeneral (void *src, size_t n, size_t size, size_t nPipelineFunction, void (*pipelineFunction[])(void*, const void*)) {
    TYPE *dest = malloc (n * size);
    pipeline (dest, src, n, size, pipelineFunction, nPipelineFunction);
    printDouble (dest, n, __FUNCTION__);    
    free (dest);
}

void testPipelineNormal (void *src, size_t n, size_t size) {
    void (*pipelineFunction[])(void*, const void*) = {
        workerMultTwo,
        workerAddOne,
        workerDivTwo
    };

    size_t nPipelineFunction = sizeof (pipelineFunction)/sizeof(pipelineFunction[0]);
    testPipelineGeneral(src, n, size, nPipelineFunction, pipelineFunction);
}

void testPipelineBig (void *src, size_t n, size_t size) {
    void (*pipelineFunction[])(void*, const void*) = {
        workerMultTwo,
        workerAddOne,
        workerDivTwo,
        workerMultTwo,
        workerAddOne,
        workerDivTwo
    };
    int nPipelineFunction = sizeof (pipelineFunction)/sizeof(pipelineFunction[0]);
    testPipelineGeneral(src, n, size, nPipelineFunction, pipelineFunction);
    /*
    int nPipelineFunction = sizeof (pipelineFunction)/sizeof(pipelineFunction[0]);
    TYPE *dest = malloc (n * size);
    pipeline (dest, src, n, size, pipelineFunction, nPipelineFunction);
    printDouble (dest, n, __FUNCTION__);    
    free (dest);
    */
}

void testPipelineBigger (void *src, size_t n, size_t size) {
    void (*pipelineFunction[])(void*, const void*) = {
        workerMultTwo,
        workerAddOne,
        workerDivTwo,
        workerMultTwo,
        workerAddOne,
        workerDivTwo,
        workerMultTwo,
        workerAddOne,
        workerDivTwo
    };
    int nPipelineFunction = sizeof (pipelineFunction)/sizeof(pipelineFunction[0]);
    testPipelineGeneral(src, n, size, nPipelineFunction, pipelineFunction);
    /*
    int nPipelineFunction = sizeof (pipelineFunction)/sizeof(pipelineFunction[0]);
    TYPE *dest = malloc (n * size);
    pipeline (dest, src, n, size, pipelineFunction, nPipelineFunction);
    printDouble (dest, n, __FUNCTION__);    
    free (dest);
    */
}

//=======================================================
// Farm tests
//=======================================================

void testFarmGeneral (void *src, size_t n, size_t size, void (*worker)(void *v1, const void *v2), size_t nWorkers) {
    TYPE *dest = malloc (n * size);
    farm (dest, src, n, size, worker, nWorkers);
    printDouble (dest, n, __FUNCTION__);
    free (dest);
}

void testFarmSum (void *src, size_t n, size_t size) {
    testFarmGeneral(src, n, size, workerAddOne, 3);
}

void testFarmWorker5 (void *src, size_t n, size_t size) {
    testFarmGeneral(src, n, size, workerAddOne, 5);
}

void testFarmWorkerNThreads (void *src, size_t n, size_t size) {
    testFarmGeneral(src, n, size, workerAddOne, 10);
}

//=======================================================
// List of unit test functions
//=======================================================


typedef void (*TESTFUNCTION)(void *, size_t, size_t);

TESTFUNCTION testFunction[MAX_TESTS];

char *testNames[MAX_TESTS];

int nTestFunction = 0;

void setMapTests() {

    testFunction[nTestFunction] = testMapSum;
    testNames[nTestFunction++] = "testMapSum";
    
    testFunction[nTestFunction] = testMapHeavy;
    testNames[nTestFunction++] = "testMapHeavy";
    
}

void setStencilTests() {

    testFunction[nTestFunction] = testNoFusionStencil;
    testNames[nTestFunction++] = "testNoFusionStencil";

    testFunction[nTestFunction] = testCodeFusionStencil;
    testNames[nTestFunction++] = "testCodeFusionStencil";

    testFunction[nTestFunction] = testCodeAndCacheFusionStencil;
    testNames[nTestFunction++] = "testCodeAndCacheFusionStencil";

}

void setReduceTests() {

    testFunction[nTestFunction] = testReduceSum;
    testNames[nTestFunction++] = "testReduceSum";

    testFunction[nTestFunction] = testReduceDiv;
    testNames[nTestFunction++] = "testReduceDiv";

}

void setScanTests() {

    testFunction[nTestFunction] = testScanSum;
    testNames[nTestFunction++] = "testScanSum";

    testFunction[nTestFunction] = testScanHeavy;
    testNames[nTestFunction++] = "testScanHeavy";

}

void setPackTests() {

    testFunction[nTestFunction] = testPackDest3;
    testNames[nTestFunction++] = "testPackDest3";

    testFunction[nTestFunction] = testPackDest10;
    testNames[nTestFunction++] = "testPackDest10";

    testFunction[nTestFunction] = testPackFilterBelow10;
    testNames[nTestFunction++] = "testPackFilterBelow10";

}

void setGatherTests() {

    testFunction[nTestFunction] = testGatherFilter3;
    testNames[nTestFunction++] = "testGatherFilter3";

    testFunction[nTestFunction] = testGatherFilter20;
    testNames[nTestFunction++] = "testGatherFilter20";

    testFunction[nTestFunction] = testGatherFilter100;
    testNames[nTestFunction++] = "testGatherFilter100";
}

void setScatterTests() {

    testFunction[nTestFunction] = testBaseScatterLikelyDestCache;
    testNames[nTestFunction++] = "sequentialScatterCacheProneOutput";

    testFunction[nTestFunction] = testBaseScatterUnlikelyDestCache;
    testNames[nTestFunction++] = "sequentialScatterLowOutputCache";

    testFunction[nTestFunction] = testNoOpScatterSmallOutput;
    testNames[nTestFunction++] = "filterCreationSmallOutputs";

    testFunction[nTestFunction] = testNoOpScatterLargeOutput;
    testNames[nTestFunction++] = "filterCreationLargeOutputs";

    testFunction[nTestFunction] = testLowOutputScatterSmallOutput;
    testNames[nTestFunction++] = "scatterOptimizedLowOutputIdeal";

    testFunction[nTestFunction] = testLowOutputScatterLargeOutputComplete;
    testNames[nTestFunction++] = "scatterOptimizedLowOutputAverageCompl";

    testFunction[nTestFunction] = testLowOutputScatterLargeOutputIncomplete;
    testNames[nTestFunction++] = "scatterOptimizedLowOutputAverageImcompl";

    testFunction[nTestFunction] = testLockScatterLotOfConcurrency;
    testNames[nTestFunction++] = "scatterLocksHighConcurrency";

    testFunction[nTestFunction] = testLockScatterSmallConcurrencyComplete;
    testNames[nTestFunction++] = "scatterLocksBenefitingLeaveEarlyMechanism";

    testFunction[nTestFunction] = testLockScatterSmallConcurrencyIncomplete;
    testNames[nTestFunction++] = "scatterLocksNotBenefitingLeaveEarlyMechanism";

    testFunction[nTestFunction] = testRaceConditionsScatterUnlikelyCache;
    testNames[nTestFunction++] = "scatterRaceConditions";

    testFunction[nTestFunction] = testRaceConditionsScatterLikelyCache;
    testNames[nTestFunction++] = "scatterRaceConditionsThreadsSameWriteAccess";

}

void setPipelineTests() {

    testFunction[nTestFunction] = testPipelineNormal;
    testNames[nTestFunction++] = "testPipelineNormal";

    testFunction[nTestFunction] = testPipelineBig;
    testNames[nTestFunction++] = "testPipelineBig";

    testFunction[nTestFunction] = testPipelineBigger;
    testNames[nTestFunction++] = "testPipelineBigger";

}

void setFarmTests() {

    testFunction[nTestFunction] = testFarmSum;
    testNames[nTestFunction++] = "testFarmSum";

    testFunction[nTestFunction] = testFarmWorker5;
    testNames[nTestFunction++] = "testFarmWorker5";

    testFunction[nTestFunction] = testFarmWorkerNThreads;
    testNames[nTestFunction++] = "testFarmWorkerNThreads";

}

void setNormalTests() {

    testFunction[nTestFunction] = testMapSum;
    testNames[nTestFunction++] = "testMap";

    testFunction[nTestFunction] = defaultStencilTest;
    testNames[nTestFunction++] = "testStencil";

    testFunction[nTestFunction] = testReduceSum;
    testNames[nTestFunction++] = "testReduce";

    testFunction[nTestFunction] = testScanSum;
    testNames[nTestFunction++] = "testScan";

    testFunction[nTestFunction] = testPackDest3;
    testNames[nTestFunction++] = "testPack";

    testFunction[nTestFunction] = testGatherFilter3;
    testNames[nTestFunction++] = "testGather";

    testFunction[nTestFunction] = defaultScatterTest;
    testNames[nTestFunction++] = "testScatter";

    testFunction[nTestFunction] = testPipelineNormal;
    testNames[nTestFunction++] = "testPipeline";

    testFunction[nTestFunction] = testFarmSum;
    testNames[nTestFunction++] = "testFarm";

}

void setTestFunctions(void *pattern) {
    
    char *str = (char *) pattern;
    if(!strcmp("MAP", str)) {
        setMapTests();
    } else if(!strcmp("STENCIL", str)) {
        setStencilTests();
    } else if(!strcmp("REDUCE", str)) {
        setReduceTests();
    } else if(!strcmp("SCAN", str)) {
        setScanTests();
    } else if(!strcmp("PACK", str)) {
        setPackTests();
    } else if(!strcmp("GATHER", str)) {
        setGatherTests();
    } else if(!strcmp("SCATTER", str)) {
        setScatterTests();
    } else if(!strcmp("PIPELINE", str)) {
        setPipelineTests();
    } else if(!strcmp("FARM", str)) {
        setFarmTests();
    } else {
        setNormalTests();
    }
    
}
