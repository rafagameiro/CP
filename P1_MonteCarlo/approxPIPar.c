#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>


double doSimulation()
{

    double x = (double) rand() / RAND_MAX;
    double y = (double) rand() / RAND_MAX;
    double calc = (x*x) + (y*y);

    if(calc <= 1.0)
       return 1.0; 

    return 0.0;
}

void computePI(double points, int simulations, double *pi)
{
    
    *pi = (points / (double) simulations) * 4;
}

void * threadSimulation(void *simulations) 
{
    int i;
    double points = 0.0;
    for(i = 0; i < ((long)simulations); i++)
        points += doSimulation();

    printf("points %d\n", &points);

    return &points;
}

double * simulate(int simulations, int nThreads)
{
    static double results[2];
    pthread_t threads[nThreads];
    srand(time(NULL));
    long simulPerThread = simulations / nThreads;
    int i;

    for(i = 0; i < nThreads; i++)
       pthread_create(&threads[i], NULL, threadSimulation, (void *)simulPerThread);


    void *points;
    for(i = 0; i < nThreads; i++)
    {
        pthread_join(threads[i], points);
        printf("points received %p\n", points);
        results[0] += (long) points;
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
