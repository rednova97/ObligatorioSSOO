#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>

#define SIZE (sizeof(int) * 10)

// PROTOTIPOS
bool heladera_vacia(int *heladera);
void rellenar_heladera(sem_t *semHel, int *heladera);

int main() {

    srand(time(NULL));

    // Abrir semáforo
    sem_t *semHel = sem_open("semHeladera", 0);
    if (semHel == SEM_FAILED) {
        perror("Error abriendo semaforo heladera");
        exit(1);
    }

    // Abrir memoria compartida
    int shm_hel_fd = shm_open("heladera", O_RDWR, 0666);
    int *heladera = (int*) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_hel_fd, 0);

    printf("Repostero iniciado. PID=%d\n", getpid());

    // Bucle principal
    while (1) {

        if (heladera_vacia(heladera)) {
            printf("Repostero %d: Heladera vacía, llenando...\n", getpid());
            rellenar_heladera(semHel, heladera);
        }

        sleep(1);
    }

    return 0;
}

// ======================================================
//                 PROCEDIMIENTOS
// ======================================================

bool heladera_vacia(int *heladera) {
    return heladera[0] <= 0;
}

void rellenar_heladera(sem_t *semHel, int *heladera) {

    // Producir 25 postres
    for (int i = 0; i < 25; i++)
        sem_post(semHel);

    heladera[0] = 25;

    printf("Repostero %d llenó la heladera con 25 postres.\n",
           getpid());
}
