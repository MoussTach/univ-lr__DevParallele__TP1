#include <mpi/mpi.h>
#include <math.h>
#include <stdio.h>

#define MASTER 0
#define REAL_PI 3.1415926535897

int main (int argc, char **argv) {
    int         numtasks;
    int         taskid;

    int         count = 0;
    int         start = 0;
    int         round = 0;

    double      totalSum = 0.0;
    double      pi = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    if (taskid == MASTER) {
        printf("Enter the number of intervals: (0 quits) \n");
        scanf("%d", &round);

    }
    MPI_Bcast(&round, 1, MPI_INT, 0, MPI_COMM_WORLD);

    count = round / numtasks;
    start = count * taskid;

    for (int i = start; i < (start + count); ++i) {
        totalSum += pow(-1, i) / (2 * i + 1);
    }
    MPI_Reduce(&totalSum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (taskid == MASTER) {
        pi = pi * 4;
        printf("REAL PI = %f\n", REAL_PI);
        printf("PI = %f\n", pi);
    }

    MPI_Finalize();
}
