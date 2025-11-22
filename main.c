#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#define SIZE (sizeof(int) * 10)

const int MOZOS = 5;
const int COCINEROS = 3;
const int REPOSTEROS = 1;

int main() {

    srand(time(NULL));

    //CREACIÓN DE SEMÁFOROS
    sem_t *semMes = sem_open("semMesada", O_CREAT, 0666, 1);
    sem_t *semHel = sem_open("semHeladera", O_CREAT, 0666, 1);

    if (semMes == SEM_FAILED || semHel == SEM_FAILED) {
        perror("Error creando semáforos");
        exit(1);
    }

    //CREACIÓN DE MEMORIA COMPARTIDA

    int shm_mes_fd = shm_open("mesada", O_CREAT | O_RDWR, 0666);
    int shm_hel_fd = shm_open("heladera", O_CREAT | O_RDWR, 0666);

    if (shm_mes_fd < 0 || shm_hel_fd < 0) {
        perror("Error creando SHM");
        exit(1);
    }

    // ajustar tamaño
    ftruncate(shm_mes_fd, SIZE);
    ftruncate(shm_hel_fd, SIZE);



    // ================================
    //   LANZAR PROCESOS HIJOS
    // ================================

    int p;

    // Lanzar repostero
    for (int i = 0; i < REPOSTEROS; i++) {
        p = fork();
        if (p == 0) {
            execl("./repostero", "./repostero", NULL);
            perror("Error al ejecutar repostero");
            exit(1);
        }
    }

    // Lanzar cocineros
    for (int i = 0; i < COCINEROS; i++) {
        p = fork();
        if (p == 0) {
            execl("./cocinero", "./cocinero", NULL);
            perror("Error al ejecutar cocinero");
            exit(1);
        }
    }

    // Lanzar mozos
    for (int i = 0; i < MOZOS; i++) {
        p = fork();
        if (p == 0) {
            execl("./mozo", "./mozo", NULL);
            perror("Error al ejecutar mozo");
            exit(1);
        }
    }

    //ESPERAMOS A TODOS LOS HIJOS
    for (int i = 0; i < (MOZOS + COCINEROS + REPOSTEROS); i++)
        wait(NULL);

    //CIERRE Y DESTRUCCIÓN DE SEMAFOROS Y MEMORIAS COMPARTIDAS
    sem_close(semMes);
    sem_close(semHel);

    sem_unlink("semMesada");
    sem_unlink("semHeladera");

    close(shm_mes_fd);
    close(shm_hel_fd);

    shm_unlink("mesada");
    shm_unlink("heladera");

    return 0;
}
