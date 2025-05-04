#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>

//.h
#include "texteditengine.h"

int main() {
    char com[COM_SIZE];

    defaultdraft();

    //main menu
    while (1) {
        ui();
        printf("Enter number: ");
        fgets(com, COM_SIZE, stdin);
        com[strcspn(com, "\n")] = 0;

        if (strcmp(com, "0") == 0) {
            break;
        }
        process(com);
    }

    cleanup();
    printf("\nProgram Ended. Press Enter to exit");
    getchar(); //enter

    return 0;
}