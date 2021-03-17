#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MASTER 0
#define REAL_PI 3.1415926535897

double  calcPi(int round) {
    double x;
    double y;
    double score;
    unsigned int cconst;

    score = 0;
    cconst = 2 << (31 - 1);
    for (int n = 1; n <= round; n++) {
        x = (2.0 * (double) random() / cconst) - 1.0;
        y = (2.0 * (double) random() / cconst) - 1.0;

        if ((x*x + y*y) <= 1.0) {
            score++;
        }
    }
    return 4.0 * (double) score/(double) round;
}

int main (int argc, char **argv) {
    int         numtasks;
    int         taskid;
    int         mType;

    int         darts = 5000;
    int         round = 100;

    double      resultPi = 0.0;
    double      rcvPi = 0.0;
    double      totalSum = 0.0;
    double      avePi = 0.0;
    MPI_Status  status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    srandom (taskid);

    avePi = 0;
    for (int i = 0; i < round; i++) {
        resultPi = calcPi(darts);

        mType = i;
        if (taskid != MASTER) {
            MPI_Send(&resultPi, 1, MPI_DOUBLE, MASTER, mType, MPI_COMM_WORLD);

        } else {
            rcvPi = 0;
            totalSum = 0.0;
            //attends toutes les réponses et les additionne
            for (int n = 1; n < numtasks; n++) {
                MPI_Recv(&rcvPi, 1, MPI_DOUBLE, MPI_ANY_SOURCE, mType, MPI_COMM_WORLD, &status);
                totalSum += rcvPi;
            }

            totalSum = (totalSum + resultPi) / numtasks;
            avePi = ((avePi * i) + totalSum)/(i + 1);
        }
    }

    if (taskid == MASTER) {
        printf("REAL PI = %f\n", REAL_PI);
        printf("PI = %f\n", avePi);
    }
    MPI_Finalize();
}
