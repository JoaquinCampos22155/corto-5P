#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_THREADS 4
#define NUM_DOCTORES 3
#define NUM_PACIENTES 12

int main() {
    int n = NUM_PACIENTES;
    int data[NUM_PACIENTES];
    int total = 0;
    int max, min;
    double promedio;

    // Simular tiempos de espera aleatorios entre 1 y 20
    for (int i = 0; i < n; i++) {
        data[i] = (rand() % 20) + 1;
    }

    // Asignar número de threads de OpenMP
    omp_set_num_threads(NUM_THREADS);

    // Asignación de pacientes y suma de tiempos
    #pragma omp parallel for reduction(+:total) shared(data)
    for (int i = 0; i < n; i++) {
        int doctorId = i % NUM_DOCTORES;
        total += data[i];
    }

    // Calcular promedio
    promedio = (double) total / n;

    // Calcular máximo y mínimo en paralelo
    max = data[0];
    min = data[0];
    int max_result, min_result;

    #pragma omp parallel sections firstprivate(max, min) shared(data, max_result, min_result)
    {
        #pragma omp section
        {
            int local_max = max;
            for (int i = 1; i < n; i++) {
                if (data[i] > local_max) {
                    local_max = data[i];
                }
            }
            #pragma omp critical
            {
                max_result = local_max;
            }
        }

        #pragma omp section
        {
            int local_min = min;
            for (int i = 1; i < n; i++) {
                if (data[i] < local_min) {
                    local_min = data[i];
                }
            }
            #pragma omp critical
            {
                min_result = local_min; // guardar en variable compartida para el main
            }
        }
    }
    max = max_result;
    min = min_result;

    // Mostrar resultados
    printf("Tiempos de espera: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    printf("Promedio de los tiempos de espera: %.2f\n", promedio);
    printf("Valor máximo de espera: %d\n", max);
    printf("Valor mínimo de espera: %d\n", min);
    printf("Suma total de los tiempos de espera: %d\n", total);

    return 0;
}