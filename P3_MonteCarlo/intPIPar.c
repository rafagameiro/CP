#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Please specify the number of iterations and threads.\n");
        return 1;
    }

    int simulations = strtol(argv[1], NULL, 10);
    double step = 1 / (double) simulations;
    double pi, sum = 0.0;    
    
    int i;
    double x = 0.0;

    omp_set_num_threads(strtol(argv[2], NULL, 10));
    #pragma omp parallel for reduction (+:sum) 
    for(i = 0; i < simulations; i++)
    {
        x = (i + 0.5) * step;
        sum = sum + 4.0/(1.0 + x*x);
    }
    
    pi = step * sum;

    printf("Number of iterations: %d\n", strtol(argv[1], NULL, 10));
    printf("PI estimation: %f\n", pi);

    return 0;
}
