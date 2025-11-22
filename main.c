#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

const int MOZOS = 3;
const int COCINEROS = 3;
const int REPOSTEROS = 3;

int main() {

    int p;

    // Lanzar reposteros
    for (int i = 0; i < REPOSTEROS; i++) {
        p = fork();
        if (p == 0) {
            char *args[] = {"./repostero", NULL};
            execv("./repostero", args);
            perror("Error al ejecutar repostero");
            exit(1);
        }
    }

    // Lanzar cocineros
    for (int i = 0; i < COCINEROS; i++) {
        p = fork();
        if (p == 0) {
            char *args[] = {"./cocinero", NULL};
            execv("./cocinero", args);
            perror("Error al ejecutar cocinero");
            exit(1);
        }
    }

    // Lanzar mozos
    for (int i = 0; i < MOZOS; i++) {
        p = fork();
        if (p == 0) {
            char *args[] = {"./mozo", NULL};
            execv("./mozo", args);
            perror("Error al ejecutar mozo");
            exit(1);
        }
    }

    // Esperar a todos
    for (int i = 0; i < (MOZOS + COCINEROS + REPOSTEROS); i++)
        wait(NULL);

    return 0;
}
