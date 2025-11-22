#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define SIZE (sizeof(int) * 10)

// FIRMAS
void levanta_pedido(int &tipo, int &cantidad);
void retira_plato(int cantidad, sem_t *semMes, int *mesada);
void retira_postre(int cantidad, sem_t *semHel, int *heladera);
void entrega_pedido();

int main() {

    srand(time(NULL));

    // Abrir semáforos
    sem_t *semHel = sem_open("semHeladera", 0);
    sem_t *semMes = sem_open("semMesada", 0);

    if (semHel == SEM_FAILED || semMes == SEM_FAILED) {
        perror("Error abriendo semáforos");
        exit(1);
    }

    // Abrir memoria compartida
    int shm_hel_fd = shm_open("heladera", O_RDWR, 0666);
    int shm_mes_fd = shm_open("mesada",  O_RDWR, 0666);

    int *heladera = (int*) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_hel_fd, 0);
    int *mesada   = (int*) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_mes_fd, 0);

    printf("Mozo iniciado. PID=%d\n", getpid());

    while (1) {

        int tipo, cantidad;

        levanta_pedido(tipo, cantidad);

        if (tipo == 0)          // plato
            retira_plato(cantidad, semMes, mesada);
        else                    // postre
            retira_postre(cantidad, semHel, heladera);

        entrega_pedido();
    }

    // Nunca llega aquí
    munmap(heladera, SIZE);
    munmap(mesada, SIZE);
    sem_close(semHel);
    sem_close(semMes);

    return 0;
}



//        IMPLEMENTACION DE PROCEDIMIENTOS


void levanta_pedido(int &tipo, int &cantidad) {

    tipo = rand() % 2;      // 0 = plato, 1 = postre

    if (tipo == 0)
        cantidad = (rand() % 4) + 1;   // 1–4 platos
    else
        cantidad = (rand() % 6) + 1;   // 1–6 postres
}

void retira_plato(int cantidad, sem_t *semMes, int *mesada) {

    printf("Mozo %d quiere retirar %d platos.\n", getpid(), cantidad);

    // Si no hay platos, sem_wait duerme al mozo
    for (int i = 0; i < cantidad; i++)
        sem_wait(semMes);

    mesada[0] -= cantidad;
    if (mesada[0] < 0) mesada[0] = 0;

    printf("Mozo %d retiró %d platos. Mesada ahora: %d\n",
           getpid(), cantidad, mesada[0]);
}

void retira_postre(int cantidad, sem_t *semHel, int *heladera) {

    printf("Mozo %d quiere retirar %d postres.\n", getpid(), cantidad);

    for (int i = 0; i < cantidad; i++)
        sem_wait(semHel);

    heladera[0] -= cantidad;
    if (heladera[0] < 0) heladera[0] = 0;

    printf("Mozo %d retiró %d postres. Heladera ahora: %d\n",
           getpid(), cantidad, heladera[0]);
}

void entrega_pedido() {
    printf("Mozo %d está entregando pedido...\n", getpid());
    sleep(1);
}
