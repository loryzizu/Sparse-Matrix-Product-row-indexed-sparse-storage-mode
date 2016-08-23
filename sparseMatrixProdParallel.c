#include "/opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h"
#include "iindexx.c"
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define NOT_NULL_ELEMENTS 9

float ** initBookMat(){
    float ** inputMat=malloc(sizeof *inputMat * (5 +1) );
    inputMat[1]=malloc(sizeof *inputMat[1] * (5+1));
    inputMat[1][1]=3;
    inputMat[1][2]=0;
    inputMat[1][3]=1;
    inputMat[1][4]=0;
    inputMat[1][5]=0;
    inputMat[2]=malloc(sizeof *inputMat[2] * (5+1));
    inputMat[2][1]=0;
    inputMat[2][2]=4;
    inputMat[2][3]=0;
    inputMat[2][4]=0;
    inputMat[2][5]=0;
    inputMat[3]=malloc(sizeof *inputMat[3] * (5+1));
    inputMat[3][1]=0;
    inputMat[3][2]=7;
    inputMat[3][3]=5;
    inputMat[3][4]=9;
    inputMat[3][5]=0;
    inputMat[4]=malloc(sizeof *inputMat[4] * (5+1));
    inputMat[4][1]=0;
    inputMat[4][2]=0;
    inputMat[4][3]=0;
    inputMat[4][4]=0;
    inputMat[4][5]=2;
    inputMat[5]=malloc(sizeof *inputMat[5] * (5+1));
    inputMat[5][1]=0;
    inputMat[5][2]=0;
    inputMat[5][3]=0;
    inputMat[5][4]=6;
    inputMat[5][5]=5;

    /*
     * Print matrice libro
     * */
    printf("**********Matrice A***********\n");
    int i;
    for (i = 1; i <= 5; i++){
        int j;
        for ( j = 1; j <= 5; j++)
        {
            printf(" |%f| ",inputMat[i][j]);
        }
        printf("\n");
    }

    return inputMat;
}
float ** initRandMat(int rows, int cols){
    float **inputMat2=malloc(sizeof *inputMat2 * (rows +1) );
    int i;
    for (i = 1; i <= rows; i++)
    {
        /*
         * per ogni cella della prima riga alloco la colonna corrispondente,
         * in pratica espando lo spazio di memoria occupato dalla cella
         * lo spazio occupato dalla cella passa da sizeof float a sizeof float * col
         * */
        inputMat2[i] = malloc(sizeof *inputMat2[i] * (cols+1));
        if (inputMat2[i])
        {
            size_t j;
            for (j = 1; j <= cols; j++)
            {
                inputMat2[i][j] = 0;
                if(i==j){
                    inputMat2[i][j] = 1;
                }
            }
        }
    }

    /*
     * Set sparse values in random way
     * */
    srand(time(NULL));
    for(i=0; i< NOT_NULL_ELEMENTS;i++){

        int rows_index = rand()%(rows)+1;
        int col_index = rand()%(cols)+1;
        int rand_value = rand()%1000;
        inputMat2[rows_index][col_index]=rand_value;
    }

    /*
     * Print matrice 2
     * */
    printf("**********Matrice B***********\n");
    for (i = 1; i <= rows; i++){
        int j;
        for (j = 1; j <= cols; j++)
        {
            printf(" |%f| ",inputMat2[i][j]);
        }
        printf("\n");
    }

    return inputMat2;
}
/*
 * Converts matrix a, represented with classic structure, to row-indexed structure
 * */
void sprsin(float **a, int n, float thresh, unsigned long nmax, float sa[], unsigned long ija[]) {
    int i, j;
    unsigned long k;
    //Store diagonal elements.

    for (j = 1; j <= n; j++) {
        sa[j] = a[j][j];
    }

    //Index to 1st row off-diagonal element, if any.
    ija[1] = n + 2;

    k = n + 1;
    //Loop over rows.
    for (i = 1; i <= n; i++) {
        //Loop over columns.
        for (j = 1; j <= n; j++) {
            if (fabs(a[i][j]) >= thresh && i != j) {
                if (++k > nmax) nrerror("sprsin: nmax too small");
                //Store off-diagonal elements and their columns.
                sa[k] = a[i][j];
                ija[k] = j;
            }
        }
        //As each row is completed, store index to next.
        ija[i + 1] = k + 1;
    }
}
/*Construct the transpose of a sparse square matrix, from row-index sparse storage arrays sa and
        ija into arrays sb and ijb .*/
void sprstp(float sa[], unsigned long ija[], float sb[], unsigned long ijb[]) {
    unsigned long j, jl, jm, jp, ju, k, m, n2, noff, inc, iv;
    float v;
    n2 = ija[1];
    for (j = 1; j <= n2 - 2; j++){
        sb[j] = sa[j];
    }
    iindexx(ija[n2 - 1] - ija[1], (long *) &ija[n2 - 1], &ijb[n2 - 1]);
    jp = 0;
    for (k = ija[1]; k <= ija[n2 - 1] - 1; k++) {
        m = ijb[k] + n2 - 1;
        sb[k] = sa[m];
        for (j = jp + 1; j <= ija[m]; j++) ijb[j] = k;
        jp = ija[m];
        jl = 1;
        ju = n2 - 1;
        while (ju - jl > 1) {
            jm = (ju + jl) / 2;
            if (ija[jm] > m) ju = jm; else jl = jm;
        }
        ijb[k] = jl;


    }
    for (j = jp + 1; j < n2; j++) ijb[j] = ija[n2 - 1];
    for (j = 1; j <= n2 - 2; j++) {
        jl = ijb[j + 1] - ijb[j];
        noff = ijb[j] - 1;
        inc = 1;
        do {
            inc *= 3;
            inc++;
        } while (inc <= jl);
        do {
            inc /= 3;
            for (k = noff + inc + 1; k <= noff + jl; k++) {
                iv = ijb[k];
                v = sb[k];
                m = k;
                while (ijb[m - inc] > iv) {
                    ijb[m] = ijb[m - inc];
                    sb[m] = sb[m - inc];
                    m -= inc;
                    if (m - noff <= inc) break;
                }
                ijb[m] = iv;
                sb[m] = v;
            }
        } while (inc > 1);
    }
}


int main(int argc, char *argv[]) {
    int cntSend=0;
    int cntRcv=0;
    int size, rank, rc;
    MPI_Status status;
    rc = MPI_INIT (&argc, &argv);
    double start = MPI_Wtime();
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    if (rank == 0) {
        float **matA=initBookMat();
        float **matB=initRandMat(6,6);

        /*Convert matrices in row-indexed sparse storage mode*/
        float saA[12];
        long ijaA[12];
        sprsin(matA, 5, 0.1, 11, saA, ijaA);
        printf("Sprsin #1 executed\n");
        float saB[20];
        long ijaB[20];
        sprsin(matB, 5, 0.1, 19, saB, ijaB);
        printf("Sprsin #2 executed\n");

        /*Transpose matrix B*/
        float sbB[20];
        long ijbB[20];
        sprstp(saB,ijaB, sbB, ijbB);
        printf("Sprstp executed\n");

        /*Distribute the workload among the slaves*/
    }
    else {
        printf("I'm the slave #%d \n",rank);
    }
    double end = MPI_Wtime();
    MPI_FINALIZE();
    double elapsed = end - start;
    printf("Tempo trascorso: %f secondi\n", elapsed );
    printf("Send: %d \n", cntSend );
    printf("Rcv: %d \n", cntRcv );
}