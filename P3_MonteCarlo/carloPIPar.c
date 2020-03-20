#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Please specify the number of iterations and threads.\n");
        return 1;
    }

    int simulations = strtol(argv[1], NULL, 10);
    double pi;
    int points = 0.0;    
    
    omp_set_num_threads(strtol(argv[2], NULL, 10));

    #pragma omp parallel
    {
        int i;
        double x;
        double y;

        unsigned int seed = getpid();
        #pragma omp parallel for reduction (+:points) 
        for(i = 0; i < simulations; i++)
        {
            x = (double) rand_r(&seed) / RAND_MAX;
            y = (double) rand_r(&seed) / RAND_MAX;
        
            if((x*x) + (y*y) <= 1.0)
                points ++;
        }
    }

    pi = ((double) points / (double) simulations) * 4;

    printf("Total number of points: %d\n", simulations);
    printf("Points within circle: \t%d\n", points);
    printf("PI estimation: \t\t%f\n", pi);

    return 0;
}
