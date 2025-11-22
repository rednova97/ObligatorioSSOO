#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

const int MOZOS = 3;
const int COCINEROS = 3;
const int REPOSTEROS = 3;

int main() {

    int p;
    
    for (int i = 0; i < REPOSTEROS; i++) {
        p = fork();
        if (p == 0) {
            char *args[] = {"repostero.exe", NULL};
            execv("C:\\Users\\awsba\\OneDrive\\Desktop\\OblSO\\repostero.exe", args);
        }
    }

    for (int i = 0; i < COCINEROS; i++) {
        p = fork();
        if (p == 0) {
            char *args[] = {"cocinero.exe", NULL};
            execv("C:\\Users\\awsba\\OneDrive\\Desktop\\OblSO\\cocinero.exe", args);

        }
    }

    for (int i = 0; i < MOZOS; i++) {
        p = fork();
        if (p == 0) {
            char *args[] = {"mozo.exe", NULL};
            execv("C:\\Users\\awsba\\OneDrive\\Desktop\\OblSO\\mozo.exe", args);

        }
    }

    for (int i = 0; i < (MOZOS + COCINEROS + REPOSTEROS); i++)
        wait(NULL);

    return 0;
}
 