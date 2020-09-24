#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "patterns.h"
#include "omp.h"

#define DEFAULT_STENCIL_IMP codeAndCacheFusionStencil
#define DEFAULT_SCATTER_IMP lowOutputScatter
#define DEFAULT_PIPELINE_IMP pipelineBindToStage 

int numThreads = 0;

/**
 * Set the variable numThreads with the value inputed by the user
 */
void setNumberOfThreads () {

    #pragma omp parallel
    {
        #pragma omp single
        numThreads = omp_get_num_threads();
    }
}


/***********************Map*************************/

/**
 * Default Parallel Map Implementation
 */
void map (void *dest, void *src, size_t nJob, size_t sizeJob, void (*worker)(void *v1, const void *v2)) {    
    assert (dest != NULL);
    assert (src != NULL);
    assert (worker != NULL);

    #pragma omp parallel for
    for (int i=0; i < nJob; i++) {
        worker(dest + i * sizeJob, src + i * sizeJob);
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

    #pragma omp parallel for
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
        #pragma omp parallel for
        for (int i = 0; i < nJob; i++) 
            worker(dest + i * sizeJob, dest + i * sizeJob, src + ((i + inputSrc) % nJob) * sizeJob);
    }
}

void noFusionStencil (void *dest, void *src, size_t nJob, size_t sizeJob, size_t width, void (*worker)(void *v1, const void *v2, const void *v3)) {
    assert (dest != NULL);
    assert (src != NULL);
    assert (worker != NULL);

    #pragma omp parallel
    {
        int threadIndex = omp_get_thread_num();
        int numThreads = omp_get_num_threads();
        for (int inputSrc = 0; inputSrc < width; inputSrc++)
            for(int i = threadIndex; i < nJob; i += numThreads)
                worker(dest + i * sizeJob, dest + i * sizeJob, src + ((i + inputSrc) % nJob) * sizeJob);        
    } 
}

/**
 * Default Parallel Stencil Implementation
 */
void stencil (void *dest, void *src, size_t nJob, size_t sizeJob, size_t width, void (*worker)(void *v1, const void *v2, const void *v3)) {
    DEFAULT_STENCIL_IMP(dest, src, nJob, sizeJob, width, worker);
}


/***********************Reduce*************************/

void sequentialReduce (void *dest, void *src, size_t nJob, size_t sizeJob, void (*worker)(void *v1, const void *v2, const void *v3)) {
    memcpy (dest, src, sizeJob);
    for (int i = 1; i < nJob; i++) 
        worker(dest, dest, src + i * sizeJob);
}

/**
 * Default Parallel Reduce Implementation
 */
void reduce (void *dest, void *src, size_t nJob, size_t sizeJob, void (*worker)(void *v1, const void *v2, const void *v3)) {
    assert (dest != NULL);
    assert (src != NULL);
    assert (worker != NULL);
    
    if (!numThreads)
        setNumberOfThreads();

    void *destAux[numThreads];
    for (int i = 0; i < numThreads; i++)
        destAux[i] = malloc(sizeJob);

    #pragma omp parallel
    {
        int thread_index = omp_get_thread_num();
        size_t firstPos = (thread_index * nJob) / omp_get_num_threads();
        size_t lastPos = ((thread_index + 1) * nJob) / omp_get_num_threads();
        memcpy (destAux[thread_index], src + firstPos * sizeJob, sizeJob);
        for (int i = firstPos + 1;  i < lastPos;  i++) 
            worker(destAux[thread_index], destAux[thread_index], src + i * sizeJob);
    }

    memcpy (dest, destAux[0], sizeJob);
    free(destAux[0]);
    for (int i = 1; i < numThreads; i++) {
        worker(dest, dest, destAux[i]);
        free(destAux[i]);
    }
}


/***********************Scan*************************/

/**
 * Default Parallel Scan Implementation
 */
void scan (void *dest, void *src, size_t nJob, size_t sizeJob, void (*worker)(void *v1, const void *v2, const void *v3)) {
    
    assert (dest != NULL);
    assert (src != NULL);
    assert (worker != NULL);
    memcpy (dest, src, sizeJob);

    //if the number of jobs is inferior or equal to 1,
    //there is no need to execute the function, 
    //the result is already in the dest pointer
    if (nJob <= 1)
        return;

    //if the numThreads variable is not yet defined,
    //a.k.a still at zero
    if(!numThreads)
        setNumberOfThreads();

    //define a auxiliar pointer to each thread
    //and sets each pointer with value zero
    void *destAux[numThreads];
    for (int i = 0; i < numThreads; i++)
        destAux[i] = calloc(1, sizeJob);
        
    #pragma omp parallel
    {

        size_t threadIndex = omp_get_thread_num();

        //obtains the memory locations interval each thread will access
        size_t firstPos = (threadIndex * nJob) / numThreads;
        size_t lastPos = ((threadIndex + 1) * nJob) / numThreads;

        for(size_t i = firstPos; i < lastPos; i++)
            worker(destAux[threadIndex], destAux[threadIndex], src + i * sizeJob);
        
        #pragma omp single
        {
            //computes the scan using the values in the array destAux
            for(size_t i = 1; i < numThreads; i++)
                worker(destAux[i], destAux[i], destAux[i-1]);
        }

        //if the current thread is not the thread 0
        //it will set the value in dest at position firstPos,
        //the value stored in  destAux at position threadIndex - 1
        if(threadIndex)
            memcpy(dest + firstPos * sizeJob, destAux[threadIndex - 1], sizeJob);

        //computes the worker function and stores its result in dest at position i
        for(size_t i = firstPos + 1; i < lastPos; i++)
            worker(dest + i * sizeJob, dest + (i - 1) * sizeJob, src + i * sizeJob);

        free(destAux[threadIndex]);
    }
}


/***********************Pack*************************/

/**
 * Default Parallel Pack Implementation
 */
int pack (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    
    //if the numThreads variable is not yet defined,
    //a.k.a still at zero
    if(!numThreads)
        setNumberOfThreads();


    //creates two auxiliar variables to each thread,
    //where the countOnes will store the amount of values that
    //needs to be copied to the dest array
    //The inputVals will have the values that will be copies to the dest array
    //the size of inputVals will be number of positions each thread will have to go through
    size_t countOnes[numThreads];
    void *inputVals[numThreads];
    for(size_t i = 0; i < numThreads-1; i++) {
        inputVals[i] = malloc((nJob / numThreads) * sizeJob);
    }

    //the last thread, if the number of jobs divided by the number of threads is not an integer value,
    //will also need to go through the split rest
    inputVals[numThreads-1] = malloc((nJob / numThreads + nJob % numThreads) * sizeJob);

    #pragma omp parallel
    {
        
        int threadIndex = omp_get_thread_num();
        
        //obtains the memory locations interval each thread will access
        size_t firstIndex = (threadIndex * nJob) / numThreads;
        size_t lastIndex = ((threadIndex + 1) * nJob) / numThreads;
        size_t numOnes = 0;
        
        for (size_t i = firstIndex; i < lastIndex; i++)
            if (filter[i]) //if the value in the filter at position i is 1
                // copies the value in the src pointer at position i
                // to the inputVals array
                memcpy(inputVals[threadIndex] + (numOnes++) * sizeJob, src + i * sizeJob, sizeJob);
        
        //saves the number values stored in the inputVals
        countOnes[threadIndex] = numOnes;
    }

    //creates an auxiliar variable
    //that will say at which position each thread has to start
    //copying the values stored in inputVals to dest pointer
    size_t onesAccum[numThreads];
    onesAccum[0] = 0;
    for (int i = 1; i < numThreads; i++) 
        onesAccum[i] = onesAccum[i-1] + countOnes[i-1];

    #pragma omp parallel
    {
        int threadIndex = omp_get_thread_num();
        
        //obtains the start index of the array pointed by dest,
        //the current thread will have to copy the values stores in inputVals
        size_t indexDest = onesAccum[threadIndex];

        //obtains the number of values the current thread has to copy to the array dest
        size_t numValsToCopy = countOnes[threadIndex];
        for (size_t i = 0; i < numValsToCopy; i++)
            //copying the value in the inputVals[threadIndex] at i position 
            //to the dest pointer, at position indexDest
            memcpy(dest + (indexDest++) * sizeJob, inputVals[threadIndex] + i * sizeJob, sizeJob);
    }

    //computes the total amount of numbers copied to dest
    int pos = 0;
    for (int i = 0; i < numThreads; i++) {
        pos += countOnes[i];
        free(inputVals[i]);
    }

    return pos;
}


/***********************Gather*************************/

/**
 * Default Parallel Gather Implementation
 */
void gather (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter, int nFilter) {
    
    #pragma omp parallel for
    for (int i=0; i < nFilter; i++) {
        //copies the value in the array pointed by src at filter[i] position 
        //to dest in the position i
        memcpy (dest + i * sizeJob, src + filter[i] * sizeJob, sizeJob);
    }
}


/***********************Scatter*************************/

void workerMax (void* a, const void* b, const void* c) {
    // a = max (b, c)
    *(int *)a = *(int *)b;
    if (*(int *)c > *(int *)a)
        *(int *)a = *(int *)c;
}

/**
 *   Utils method used to compute the length of the destination output
 */
int computeDestinationLength (size_t nJob, const int *filter) {
    int *outputLengthPtr = malloc(sizeof(int));
    reduce((void *)outputLengthPtr, (void *)filter, nJob, sizeof(int), workerMax);
    int outputLength = *outputLengthPtr + 1;
    free(outputLengthPtr);
    return outputLength;
}

/**
 *   Permutation implementation of Scatter using fine-grain locking
 */
void lockScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(filter != NULL);

    int outputLength = computeDestinationLength(nJob, filter);
    //int hasOutput[outputLength];
    int *hasOutput = malloc(outputLength * sizeof(int));
    omp_lock_t locks[outputLength];

    #pragma omp parallel for
    for (int i = 0; i < outputLength; i++) {
        hasOutput[i] = -1;
        omp_init_lock(&locks[i]);
    }

    int outputIncomplete = outputLength;
    #pragma omp parallel
    {
        int threadIndex = omp_get_thread_num();
        int numThreads = omp_get_num_threads();
    
        //obtains the memory locations interval each thread will access
        size_t firstIndex = (threadIndex * nJob) / numThreads;
        size_t lastIndex = ((threadIndex + 1) * nJob) / numThreads;

        //iterates through the loop until reaching the end of assigned position of the thread 
        //or all the positions in the dest pointer become filled with some value
        for (int i = firstIndex; i < lastIndex && outputIncomplete; i++) {
            int indirection = filter[i];
            
            //if its the first time inserting in the given position, enter inside the filter
            //and attempts to lock the position it will access
            if (hasOutput[indirection] < threadIndex) {
                omp_set_lock(&locks[indirection]);
                if (hasOutput[indirection] < threadIndex) {
            
                    //marks the value to 1 to prevent entering again, at the same position
                    hasOutput[indirection] = threadIndex;
            
                    //copies the value to the dest pointer
                    memcpy (dest + indirection * sizeJob, src + i * sizeJob, sizeJob);
                    outputLength--;
                }
                omp_unset_lock(&locks[indirection]);
            }
        }
    }

    free(hasOutput);

    #pragma omp parallel for
    for (int i = 0; i < outputLength; i++)
        omp_destroy_lock(&locks[i]);
}

void lowOutputScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(filter != NULL);

    int outputLength = computeDestinationLength(nJob, filter);
    int *hasOutput = calloc(outputLength, sizeof(int));

    //iterates through the loop until reaching the end of src 
    //or all the positions in the dest pointer become filled with some value
    int outputIncomplete = outputLength;
    for (int i = 0; i < nJob && outputIncomplete; i++) {
        int indirection = filter[i];

        //if its the first time inserting in the given position, enter inside the filter
        if (!hasOutput[indirection]) {

            //marks the value to 1 to prevent entering again, at the same position
            hasOutput[indirection] = 1;

            //copies the value to the dest pointer
            memcpy (dest + indirection * sizeJob, src + i * sizeJob, sizeJob);
            outputIncomplete--;   
        }
    }

    free(hasOutput);
}

void multiScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(filter != NULL);

    int outputLength = computeDestinationLength(nJob, filter);

    //if the numThreads variable is not yet defined,
    //a.k.a still at zero
    if(!numThreads)
        setNumberOfThreads();
    
    int numberPartialOutputs = numThreads * outputLength;
    int hasOutput[numberPartialOutputs];
    #pragma omp parallel for
    for (int i = 0; i < numberPartialOutputs; i++) {
        hasOutput[i] = 0;
    } 

    //creates an auxiliar variable that will store the inserted values in each thread
    void *partialDest = calloc(numThreads, sizeJob * outputLength);
    #pragma omp parallel
    {
        int threadIndex = omp_get_thread_num();
        int outputIncomplete = outputLength;
        int numThreads = omp_get_num_threads();
        
        //obtains the memory locations interval each thread will access
        size_t firstIndex = (threadIndex * nJob) / numThreads;
        size_t lastIndex = ((threadIndex + 1) * nJob) / numThreads;

        //iterates through the loop until reaching the end of assigned position of the thread 
        //or all the positions in the dest pointer become filled with some value
        for (size_t i = firstIndex; i < lastIndex && outputIncomplete; i++) {
            int outputIndex = threadIndex * outputLength + filter[i];

            //if its the first time inserting in the given position, enter inside the filter
            if (!hasOutput[outputIncomplete]) {
                
                //copies the value to the auxiliar pointer
                memcpy (partialDest + (outputIndex) * sizeJob, src + i * sizeJob, sizeJob);
                
                //marks the value to 1 to prevent entering again, at the same position
                hasOutput[outputIndex] = 1;
                outputIncomplete--;
            }
        }
    }

    //goes through all values inserted in each partialDest assigned to each thread
    //and the first value to be found in a given position will be inserted in the dest pointer
    #pragma omp parallel for
    for (int i = 0; i < outputLength; i++)
        for (int t = 0; t < numThreads; t++) {
            if (hasOutput[t * outputLength + i]) {
                memcpy(dest + i * sizeJob, partialDest + (t * outputLength + i) * sizeJob, sizeJob);
                break;    
            }
        }
}

void sequentialScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(filter != NULL);
    
    for (int i = 0; i < nJob; i++)
        memcpy(dest + filter[i] * sizeJob, src + i * sizeJob, sizeJob);
}

void raceConditionsScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(filter != NULL);

    #pragma omp parallel for
    for (int i = 0; i < nJob; i++)
        memcpy(dest + filter[i] * sizeJob, src + i * sizeJob, sizeJob);
}

void noOpScatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {}

/**
 * Default Parallel Scatter Implementations
 */
void scatter (void *dest, void *src, size_t nJob, size_t sizeJob, const int *filter) {
    DEFAULT_SCATTER_IMP(dest, src, nJob, sizeJob, filter);
}


/***********************Pipeline*************************/

/**
 * Parallel Pipeline Implementation using Bind to Item strategy
 */
void pipelineBindToItem (void *dest, void *src, size_t nJob, size_t sizeJob, void (*workerList[])(void *v1, const void *v2), size_t nWorkers) {

     
    for (int i=0; i < nJob; i++) {

        //copies the value store in src at position i
        //to dest at position i
        memcpy (dest + i * sizeJob, src + i * sizeJob, sizeJob);

        //each worker will execute all the workerList functions 
        //and store its result in pointer dest at position i
        #pragma omp task firstprivate(i)    
        for (int j = 0;  j < nWorkers;  j++)
            workerList[j](dest + i * sizeJob, dest + i * sizeJob);
    }

}

/**
 * Parallel Pipeline Implementation using Bind to Stage strategy
 */
void pipelineBindToStage (void *dest, void *src, size_t nJob, size_t sizeJob, void (*workerList[])(void *v1, const void *v2), size_t nWorkers) {
   
    for (int i=0; i < nJob; i++) {

        //copies the value store in src at position i
        //to dest at position i
        memcpy (dest + i * sizeJob, src + i * sizeJob, sizeJob);
        

        for (int j = 0;  j < nWorkers;  j++)
            //creates a task where each worker will execute the assigned function
            //and store its value in pointer dest at position i
            //the first task is independent, but the rest will be dependent from the previous one
            #pragma omp task firstprivate(i, j) depend(out: i) 
            workerList[j](dest + i * sizeJob, dest + i * sizeJob);
    }
}

/**
 * Default Parallel Pipeline Implementation
 */
void pipeline (void *dest, void *src, size_t nJob, size_t sizeJob, void (*workerList[])(void *v1, const void *v2), size_t nWorkers) {

    DEFAULT_PIPELINE_IMP(dest, src, nJob, sizeJob, workerList, nWorkers);
}


/***********************Farm*************************/

/**
 * Default Parallel Farm Implementation
 */
void farm (void *dest, void *src, size_t nJob, size_t sizeJob, void (*worker)(void *v1, const void *v2), size_t nWorkers) {

    #pragma omp parallel
    {
        #pragma omp single
        {
            //defines the amount of initial and end positions of the array pointed
            //by the pointer dest each task will modify
            int jobs = nJob / nWorkers;
            int start = 0;
            int end = jobs;

            for(int i = 0; i < nWorkers; i++, start+= jobs, end+= jobs) {
                if(i == nWorkers-1)
                    end = nJob;

                //creates a task that will be handled by the available threads
                #pragma omp task firstprivate(start, end)
                {
                    for(int j = start;  j < end; j++)   
                        //executes the worker function and store its result in the position j
                        //of pointer dest
                        worker(dest + j * sizeJob, src + j * sizeJob);
                }
            }
        }
    }
}

