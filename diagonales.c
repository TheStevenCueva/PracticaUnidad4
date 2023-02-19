#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 100

// Inicializa una submatriz con valores aleatorios
void inicializar_submatriz(int *submatriz, int local_rows, int local_cols, int global_cols, int rank) {
    int i, j;
    srand(rank);
    for (i = 0; i < local_rows; i++) {
        for (j = 0; j < local_cols; j++) {
            submatriz[i*local_cols+j] = rand() % global_cols;
        }
    }
}

// Calcula la suma de los elementos en la diagonal de una submatriz
int suma_diagonal_submatriz(int *submatriz, int local_rows, int local_cols) {
    int i, sum = 0;
    for (i = 0; i < local_rows && i < local_cols; i++) {
        sum += submatriz[i*local_cols+i];
    }
    return sum;
}

// Calcula la suma de los elementos en la diagonal de una matriz distribuida
int suma_diagonal(int *submatriz, int local_rows, int local_cols, int global_cols, int rank, int num_procs) {
    int sum = 0;
    int i;
    // Calcular la suma de las diagonales de las submatrices locales
    sum = suma_diagonal_submatriz(submatriz, local_rows, local_cols);
    // Reducir la suma de todas las submatrices a través de todos los procesos
    for (i = 1; i < num_procs; i++) {
        if (rank == i) {
            MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        } else if (rank == 0) {
            int temp_sum;
            MPI_Recv(&temp_sum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += temp_sum;
        }
    }
    return sum;
}

int main(int argc, char **argv) {
    int rank, num_procs;
    int rows = N, cols = N;
    int local_rows, local_cols;
    int *matriz = NULL;
    int *submatriz = NULL;
    int global_cols;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Calcular el número de filas y columnas de la submatriz
    local_rows = rows / num_procs;
    local_cols = cols;

    // Inicializar la submatriz local
    submatriz = (int *) malloc(local_rows * local_cols * sizeof(int));
    //global_cols = cols * num_procs;
    global_cols = 100;
    inicializar_submatriz(submatriz, local_rows, local_cols, global_cols, rank);

    // Calcular la suma de los elementos en la diagonales
    int sum = suma_diagonal(submatriz, local_rows, local_cols, global_cols, rank, num_procs);
    // Calcular el valor más alto de los elementos en la diagonales
    int max = max_diagonal(submatriz, local_rows, local_cols, global_cols, rank, num_procs);

    int diagonal = diagonal_mas_grande(submatriz,local_rows,local_cols);
    
    if (rank == 0) {
        printf("La suma de los elementos en la diagonal es: %d\n", sum);
        printf("El valor más alto en las diagonales es: %d\n", max);
        if (diagonal == 1) {
            printf("La Diagonal pricipal es mayor\n");
        } else if (diagonal == 2) {
            printf("La Diagonal secundaria es mayor\n");
        } else {
            printf("La 2 Diagonales son iguales\n");
        }
    }
    free(matriz);
    free(submatriz);
    MPI_Finalize();
    return 0;
}

//--------------------------------------------------------------------------------------------

// Calcula el valor más alto de las diagonales de una submatriz
int max_diagonal_submatriz(int *submatriz, int local_rows, int local_cols) {
    int i, max = 0;
    for (i = 0; i < local_rows && i < local_cols; i++) {
        if (submatriz[i*local_cols+i] > max) {
            max = submatriz[i*local_cols+i];
        }
    }
    return max;
}

// Calcula el valor más alto de las diagonales de una matriz distribuida
int max_diagonal(int *submatriz, int local_rows, int local_cols, int global_cols, int rank, int num_procs) {
    int max = 0;
    int i;
    // Calcular el valor más alto de las diagonales de las submatrices locales
    max = max_diagonal_submatriz(submatriz, local_rows, local_cols);
    // Reducir el valor más alto de todas las submatrices a través de todos los procesos
    for (i = 1; i < num_procs; i++) {
        if (rank == i) {
            MPI_Send(&max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        } else if (rank == 0) {
            int temp_max;
            MPI_Recv(&temp_max, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (temp_max > max) {
                max = temp_max;
            }
        }
    }
    return max;
}

int diagonal_mas_grande(int *submatriz, int local_rows, int local_cols) {
    int i;
    int diagonal_principal = 0, diagonal_secundaria = 0;
    for (i = 0; i < local_rows && i < local_cols; i++) {
        diagonal_principal += submatriz[i*local_cols+i];
        diagonal_secundaria += submatriz[i*local_cols+local_cols-i-1];
    }
    if (diagonal_principal > diagonal_secundaria) {
        return 1;
    } else if (diagonal_principal < diagonal_secundaria) {
        return 2;
    } else {
        return 0;
    }
}