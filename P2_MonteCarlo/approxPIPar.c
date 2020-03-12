#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

void computePI(double points, int simulations, double *pi)
{
    
    *pi = (points / (double) simulations) * 4;
}

void * threadSimulation(void *simulations) 
{
    
    int i;
    double *points = malloc(sizeof(double));
    *points = 0.0;

    unsigned int seed = getpid() ^ pthread_self();

    double x;
    double y;
    for(i = 0; i < (*(int *)simulations); i++)
    {
        x = (double) rand_r(&seed) / RAND_MAX;
        y = (double) rand_r(&seed) / RAND_MAX;
        
        if((x*x) + (y*y) <= 1.0)
            *points += 1.0; 
    }
    
    return (void *) points;
}

double * simulate(int simulations, int nThreads)
{
    static double results[2];
    pthread_t threads[nThreads];
    int nsimulations = simulations / nThreads;
    int *simulPerThread = &nsimulations;
    int i;

    for(i = 0; i < nThreads-1; i++)
       pthread_create(&threads[i], NULL, threadSimulation, (void *)simulPerThread);

    *simulPerThread = *simulPerThread + *simulPerThread % nThreads;
    pthread_create(&threads[nThreads-1], NULL, threadSimulation, (void *)simulPerThread);


    void *points;
    for(i = 0; i < nThreads; i++)
    {
        pthread_join(threads[i], &points);
        results[0] += (*(double*) points);
        free((double *)points);
    }

    computePI(results[0], simulations, &results[1]);

    return results;
}

int main(int argc, char *argv[])
{
    
    if(argc != 3)
    {
        printf("Error: please specify the number of simulations and threads.\n");
        return 1;
    }

    double *results = simulate(strtol(argv[1], NULL, 10), strtol(argv[2], NULL, 10));

    printf("Total number of points: %s\n", argv[1]);
    printf("Points within circle: \t%d\n", (int)results[0]);
    printf("Pi estimation: \t\t%f\n", results[1]);

    return 0;
}
