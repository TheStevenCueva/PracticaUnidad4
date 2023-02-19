#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 10000
#define COLS 10000

// mpicc matrix.c -o matrix
// mpirun -np 4 ./matrix

int main(int argc, char** argv) {
    int rank, size;
    int num_search;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    entradas(rank, size, &num_search);

    int local_rows = ROWS / size;
    int local_cols = COLS;

    // Crear la matriz y llenarla con números aleatorios
    srand(time(NULL) + rank);
    int* matrix = (int*) malloc(local_rows * local_cols * sizeof(int));
    for (int i = 0; i < local_rows * local_cols; i++) {
        matrix[i] = rand() % 100;
    }

    
    // Realizar una búsqueda local para contar el número de veces que aparece un número en la matriz
    int local_count = 0;
    for (int i = 0; i < local_rows * local_cols; i++) {
        if (matrix[i] == num_search) {
            local_count++;
        }
    }
    // Utilizar una operación de reducción para sumar las contabilizaciones locales de todos los procesos
    int global_count;
    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    // Imprimir el resultado en el proceso raíz
    if (rank == 0) {
        printf("El número %d aparece %d veces en la matriz.\n", num_search, global_count);
    }

    // Liberar la memoria y finalizar MPI
    free(matrix);
    MPI_Finalize();
    return 0;
}

void entradas(int proceso, int nro_procesos, int *n) {
    if(proceso == 0) {
        //printf("Ingrese el número que desea buscar: ");
        scanf("%d", n);
    }
    MPI_Bcast(n, 1, MPI_INT , 0, MPI_COMM_WORLD);
}