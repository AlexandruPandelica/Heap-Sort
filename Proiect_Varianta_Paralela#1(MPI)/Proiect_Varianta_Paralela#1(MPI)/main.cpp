#define _CRT_SECURE_NO_WARNINGS


#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start_time = MPI_Wtime();

    int N = 1000000;
    int* data = NULL;

    if (rank == 0) {
        data = (int*)malloc(N * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < N; i++)
            data[i] = rand() % 100000;  // valori între 0 si 99999
    }


    int* sendcounts = (int*)malloc(size * sizeof(int));
    int* displs = (int*)malloc(size * sizeof(int));
    int rem = N % size, sum = 0;
    for (int i = 0; i < size; i++) {
        sendcounts[i] = N / size + (i < rem ? 1 : 0);
        displs[i] = sum;
        sum += sendcounts[i];
    }

    int local_n = sendcounts[rank];
    int* local_data = (int*)malloc(local_n * sizeof(int));

    MPI_Scatterv(data, sendcounts, displs, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    qsort(local_data, local_n, sizeof(int), compare);

    int num_samples = size - 1;
    int* samples = (int*)malloc(num_samples * sizeof(int));
    for (int i = 0; i < num_samples; i++) {
        int idx = (i + 1) * local_n / size;
        if (idx >= local_n) idx = local_n - 1;
        samples[i] = local_data[idx];
    }

    int* gathered_samples = NULL;
    if (rank == 0)
        gathered_samples = (int*)malloc(size * num_samples * sizeof(int));

    MPI_Gather(samples, num_samples, MPI_INT, gathered_samples, num_samples, MPI_INT, 0, MPI_COMM_WORLD);

    int* splitters = (int*)malloc((size - 1) * sizeof(int));
    if (rank == 0) {
        qsort(gathered_samples, size * num_samples, sizeof(int), compare);
        for (int i = 0; i < size - 1; i++)
            splitters[i] = gathered_samples[(i + 1) * size - 1];
    }

    MPI_Bcast(splitters, size - 1, MPI_INT, 0, MPI_COMM_WORLD);

    int* scounts = (int*)calloc(size, sizeof(int));
    for (int i = 0; i < local_n; i++) {
        int dest = 0;
        while (dest < size - 1 && local_data[i] > splitters[dest])
            dest++;
        scounts[dest]++;
    }

    int* sdispls = (int*)malloc(size * sizeof(int));
    sdispls[0] = 0;
    for (int i = 1; i < size; i++)
        sdispls[i] = sdispls[i - 1] + scounts[i - 1];

    int* sbuf = (int*)malloc(local_n * sizeof(int));
    int* scopy = (int*)calloc(size, sizeof(int));
    for (int i = 0; i < local_n; i++) {
        int dest = 0;
        while (dest < size - 1 && local_data[i] > splitters[dest])
            dest++;
        int pos = sdispls[dest] + scopy[dest]++;
        sbuf[pos] = local_data[i];
    }

    int* rcounts = (int*)malloc(size * sizeof(int));
    MPI_Alltoall(scounts, 1, MPI_INT, rcounts, 1, MPI_INT, MPI_COMM_WORLD);

    int* rdispls = (int*)malloc(size * sizeof(int));
    rdispls[0] = 0;
    for (int i = 1; i < size; i++)
        rdispls[i] = rdispls[i - 1] + rcounts[i - 1];

    int total_recv = rdispls[size - 1] + rcounts[size - 1];
    int* rbuf = (int*)malloc(total_recv * sizeof(int));

    MPI_Alltoallv(sbuf, scounts, sdispls, MPI_INT,
        rbuf, rcounts, rdispls, MPI_INT,
        MPI_COMM_WORLD);

    // Sortare final?
    qsort(rbuf, total_recv, sizeof(int), compare);

    // Scrie rezultatele locale într-un fi?ier individual
    char filename[64];
    sprintf_s(filename, sizeof(filename), "output_proces_%d.txt", rank);
    FILE* fout;
    fopen_s(&fout, filename, "w");



    if (fout != NULL) {
        fprintf(fout, "Proces %d - %d elemente:\n", rank, total_recv);
        for (int i = 0; i < total_recv; i++)
            fprintf(fout, "%d\n", rbuf[i]);
        fclose(fout);
    }
    else {
        fprintf(stderr, "Proces %d: Eroare la deschiderea fi?ierului %s\n", rank, filename);
    }

    // Procesul 0 adun? toate datele finale
    int* final_counts = (int*)malloc(size * sizeof(int));
    MPI_Gather(&total_recv, 1, MPI_INT, final_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int* final_displs = NULL;
    int total_final = 0;
    if (rank == 0) {
        final_displs = (int*)malloc(size * sizeof(int));
        final_displs[0] = 0;
        for (int i = 1; i < size; i++)
            final_displs[i] = final_displs[i - 1] + final_counts[i - 1];
        total_final = final_displs[size - 1] + final_counts[size - 1];
    }

    int* final_data = NULL;
    if (rank == 0)
        final_data = (int*)malloc(total_final * sizeof(int));

    MPI_Gatherv(rbuf, total_recv, MPI_INT, final_data, final_counts, final_displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        FILE* fglobal = fopen("output_final.txt", "w");
        if (fglobal != NULL) {
            fprintf(fglobal, "Vector sortat complet (%d elemente):\n", total_final);
            for (int i = 0; i < total_final; i++)
                fprintf(fglobal, "%d\n", final_data[i]);
            fclose(fglobal);
        }
        else {
            fprintf(stderr, "Eroare la scrierea în fisierul global.\n");
        }
    }

    // Cleanup
    free(local_data); free(samples); free(splitters); free(scounts); free(sdispls); free(sbuf);
    free(scopy); free(rcounts); free(rdispls); free(rbuf);
    if (rank == 0) {
        free(gathered_samples); free(sendcounts); free(displs);
        free(final_counts); free(final_displs); free(final_data);
    }

    double end_time = MPI_Wtime();
    if (rank == 0)
        printf("Timp executie (N = %d, %d procese): %.6f secunde\n", N, size, end_time - start_time);

    MPI_Finalize();
    return 0;
}
