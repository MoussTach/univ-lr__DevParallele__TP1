#include <mpi/mpi.h>
#include <stdlib.h>
#include <time.h>
#include "stdio.h"

#define MASTER 0

int **createVector(int h, int v) {
    int **res;

    if (!(res = malloc(sizeof(int *) * v)))
        return NULL;

    for (int i = 0; i < v; i++) {
        if (!(res[i] = malloc((sizeof(int) * h))))
            return NULL;

        for (int j = 0; j < h; j++) {
            res[i][j] = rand() % 10;
        }
    }

    return res;
}


int **multMatrice(int **m1, int **m2, int h, int v) {
    int         numtasks;
    int         taskid;
    MPI_Status  status;

    int         dest;

    int         **res;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    if (taskid == MASTER) {
        if (!(res = malloc(sizeof(int *) * v)))
            return NULL;

        for (int i = 0; i < v; i++) {
            if (!(res[i] = malloc((sizeof(int) * h))))
                return NULL;

            for (int j = 0; j < h; j++) {
                res[i][j] = 0;
            }
        }

        printf("\n");
        dest = -1;
        for (int i = 0; i < v; i++) {
            for (int j = 0; j < h; j++) {
                dest = (dest + 1) % numtasks;
                MPI_Send(&m1[i][j], 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
                MPI_Send(&m2[i][j], 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            }
        }

        dest = -1;
        for (int i = 0; i < v; i++) {
            for (int j = 0; j < h; j++) {
                dest = (dest + 1) % numtasks;
                MPI_Recv(&res[i][j], 1, MPI_INT, dest, 1, MPI_COMM_WORLD, &status);
            }
        }
    } else {

        dest = -1;
        int val1 = 0;
        int val2 = 0;
        int resTab = 0;

        for (int i = 0; i < v; i++) {
            for (int j = 0; j < h; j++) {
                dest = (dest + 1) % numtasks;
                MPI_Recv(&val1, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&val2, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD, &status);

                resTab = val1 * val2;
                MPI_Send(&res, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD);
            }
        }
    }

    MPI_Finalize();
    return res;
}

int main (int argc, char **argv) {
    int h = 3;
    int v = 3;

    srand(time(NULL));
    int **m1 = createVector(h, v);
    int **m2 = createVector(h, v);

    printf("  m1\t\t*\t  m2\n");
    for (int i = 0; i < h; ++i) {
        //m1
        for (int j = 0; j < v; j++) {
            printf("%02d ", m1[i][j]);
        }

        printf("\t|\t");

        //m2
        for (int j = 0; j < v; j++) {
            printf("%02d ", m2[i][j]);
        }
        printf("\n");
    }

    int **m3 = multMatrice(m1, m2, h, v);
    printf("\n\t  m3\n");
    for (int i = 0; i < h; ++i) {
        printf("\t");

        //m3
        for (int j = 0; j < v; j++) {
            printf("%02d ", m3[i][j]);
        }
        printf("\n");
    }
}
