#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void doSimulation(double *points)
{
    double x = (double) rand() / RAND_MAX;
    double y = (double) rand() / RAND_MAX;
    
    double calc = (x*x) + (y*y);
    
    if(calc <= 1.0)
       *points += 1.0; 
}

void computePI(double points, int simulations, double *pi)
{
    
    *pi = (points / (double) simulations) * 4;
}

double * simulate(int simulations)
{
    static double results[2];
    int i;

    srand(time(NULL));
    for(i = 0; i < simulations; i++)
       doSimulation(&results[0]); 

    computePI(results[0], simulations, &results[1]);

    return results;
}

int main(int argc, char *argv[])
{
    
    if(argc != 2)
    {
        printf("Error: please specify the number of simulations.\n");
        return 1;
    }

    double *results = simulate(strtol(argv[1], NULL, 10));

    printf("Total number of points: %s\n", argv[1]);
    printf("Points within circle: \t%d\n", (int)results[0]);
    printf("Pi estimation: \t\t%f\n", results[1]);

    return 0;
}
