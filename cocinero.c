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
void prepara_plato();
void coloca_en_mostrador(sem_t *semMes, int *mesada);

int main() {

    srand(time(NULL));

    // ABRIR SEMAFORO DE LA MESADA
    sem_t *semMes = sem_open("semMesada", 0);
    if (semMes == SEM_FAILED) {
        perror("Error abriendo semaforo mesada");
        exit(1);
    }

    // ABRIR MEMORIA COMPARTIDA DE LA MESADA
    int shm_mes_fd = shm_open("mesada", O_RDWR, 0666);
    if (shm_mes_fd < 0) {
        perror("Error abriendo SHM mesada");
        exit(1);
    }

    int *mesada = (int*) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_mes_fd, 0);
    if (mesada == MAP_FAILED) {
        perror("Error al mapear mesada");
        exit(1);
    }

    printf("Cocinero iniciado. PID=%d\n", getpid());

    // BUCLE INFINITO DEL COCINERO
    while (1) {

        prepara_plato();  // cocina 1 plato

        coloca_en_mostrador(semMes, mesada);  // intenta colocarlo
    }

    // Nunca llega aquí, pero por prolijidad:
    munmap(mesada, SIZE);
    close(shm_mes_fd);
    sem_close(semMes);

    return 0;
}


//        IMPLEMENTACIÓN DE PROCEDIMIENTOS
void prepara_plato() {
    sleep(1);   // tiempo real de preparación
}

void coloca_en_mostrador(sem_t *semMes, int *mesada) {

    // Si está lleno, el cocinero se duerme
    if (mesada[0] >= 27) {
        printf("Cocinero %d: Mesada llena (27). Esperando...\n", getpid());
        sleep(1);
        return;
    }

    // Si hay espacio:
    sem_post(semMes);  // agrega 1 plato al contador del semáforo
    mesada[0]++;       // contador informativo de SHM

    printf("Cocinero %d colocó un plato. Total en mesada: %d\n",
           getpid(), mesada[0]);
}
