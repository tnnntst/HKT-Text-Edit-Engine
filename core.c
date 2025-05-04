#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>

//.h
#include "texteditengine.h"

stackno *undost = NULL, *redost = NULL;
hisno *hishead = NULL;
treeno *header = NULL, *currentdraft = NULL;

//default draft
void defaultdraft() {
    header = malloc(sizeof(treeno));
    if (!header) {
        exit(1);
    }

    strcpy(header->data.text, ""); //empty
    struct timeval times;
    gettimeofday(&times, NULL);
    header->data.timesec = times.tv_sec;
    header->data.timemisec = times.tv_usec;
    strcpy(header->ver, "Draft 1");
    header->next = NULL;
    currentdraft = header;
}

//main menu
void ui() {
#ifdef _WIN32
    system("cls"); //cls window
#else
    system("clear"); //clear mac
#endif
    printf("--------------------------------------------------\n");
    printf("|               HKT TEXT EDIT ENGINE             |\n");
    printf("--------------------------------------------------\n");
    printf("--------------------------------------------------\n");
    printf("| NUM |              COMMAND                     |\n");
    printf("--------------------------------------------------\n");
    printf("|  1  | Edit                                     |\n");
    printf("|  2  | Undo                                     |\n");
    printf("|  3  | Redo                                     |\n");
    printf("|  4  | History                                  |\n");
    printf("|  5  | Drafts                                   |\n");
    printf("|  6  | Load .txt                                |\n");
    printf("|  7  | Save .txt                                |\n");
    printf("|  8  | About Us                                 |\n");
    printf("|  0  | Exit                                     |\n");
    printf("--------------------------------------------------\n");
}

//current text, draft
void textndraftnow() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("--------------------------------------------------\n");
    printf("|                  CURRENT TEXT                  |\n");
    printf("--------------------------------------------------\n");
    printf("%s\n", currentdraft->data.text);
    printf("--------------------------------------------------\n");
    printf("|                 CURRENT DRAFT                  |\n");
    printf("--------------------------------------------------\n");
    printf("Current Draft: %s\n", currentdraft->ver);
}

//group detail
void aboutus() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("--------------------------------------------------\n");
    printf("|                   ABOUT US                     |\n");
    printf("--------------------------------------------------\n");
    printf("C Programming Project : HKT Team\n");
    printf("Text Edit Engine: Undo, Redo, History and Drafts\n");
    printf("\n");
    printf("--------------------------------------------------\n");
    printf("| HKT Members:                                   |\n");
    printf("--------------------------------------------------\n");
    printf("| Name                      | ID                 |\n");
    printf("--------------------------------------------------\n");
    printf("| Warun Woraphoo            | 67070503438        |\n");
    printf("| Asamaporn Chayanuntasakul | 67070503448        |\n");
    printf("| Ivan Luwongsupakit        | 67070503449        |\n");
    printf("--------------------------------------------------\n");

    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

//free memory
void cleanup() {
    while (undost) {
        stackno* temp = undost;
        undost = undost->next;
        free(temp);
    }

    while (redost) {
        stackno* temp = redost;
        redost = redost->next;
        free(temp);
    }

    while (hishead) {
        hisno* temp = hishead;
        hishead = hishead->next;
        free(temp);
    }

    while (header) {
        treeno* temp = header;
        header = header->next;
        free(temp);
    }
}

//process user input
void process(char* com) {
    int choice;
    //check empty
    if (strlen(com) == 0) {
        printf("\n--------------------------------------------------\n");
        printf("⚠️ No option entered. Please select a menu option.\n");
        printf("--------------------------------------------------\n");
        return;
    }

    //num
    for (int i = 0; com[i] != '\0'; i++) {
        if (!isdigit(com[i])) {
            printf("\n--------------------------------------------------\n");
            printf("⚠️ Invalid option: '%s' is not a valid menu option.\n", com);
            printf("--------------------------------------------------\n");
            return;
        }
    }

    choice = atoi(com);
    char input[COM_SIZE];
    char message[100] = "";
    int showmessage = 0;

    //choices check
    if (choice < 0 || choice > 8) {
        printf("\n--------------------------------------------------\n");
        printf("⚠️ Invalid option: '%s' is not a valid menu choice.\n", com);
        printf("--------------------------------------------------\n");
        return;
    }

    //current text
    if (choice != 0 && choice != 2 && choice != 3 && choice != 4 && choice != 5) {
        textndraftnow();
    }

    switch(choice) {
        case 1: //edit
            edittext();
            textndraftnow();
            break;

        case 2: //undo
            if (!undost) {
                strcpy(message, "⚠️ Nothing to undo");
                showmessage = 1;
                break;
            }

            Content* undoresult = pop(&undost, &redost);
            if (!undoresult) {
                break;
            }

            strcpy(currentdraft->data.text, undoresult->text);
            currentdraft->data.timesec = undoresult->timesec;
            currentdraft->data.timemisec = undoresult->timemisec;
            addhis(currentdraft->data, "Undo");
            strcpy(message, "✅ Undo successful");
            showmessage = 1;
            textndraftnow();

            printf("--------------------------------------------------\n");
            printf("%s\n", message);
            printf("--------------------------------------------------\n");
            message[0] = '\0';
            showmessage = 0;
            break;

        case 3: //redo
            if (!redost) {
                strcpy(message, "⚠️ Nothing to redo");
                showmessage = 1;
                break;
            }

            Content* redoresult = pop(&redost, &undost);
            if (!redoresult) {
                break;
            }

            strcpy(currentdraft->data.text, redoresult->text);
            currentdraft->data.timesec = redoresult->timesec;
            currentdraft->data.timemisec = redoresult->timemisec;
            addhis(currentdraft->data, "Redo");
            strcpy(message, "✅ Redo successful");
            showmessage = 1;
            textndraftnow();

            printf("--------------------------------------------------\n");
            printf("%s\n", message);
            printf("--------------------------------------------------\n");
            message[0] = '\0';
            showmessage = 0;
            break;

        case 4: //history
            printf("\nEdit History:\n");
            showhis();
            break;

        case 5: //draft
            printf("\nAvailable Drafts:\n");
            showdraft();
            //show cuurent
            textndraftnow();
            break;

        case 6: //load txt
            printf("Enter filename to load (.txt) or 0 to cancel: ");
            scanf("%s", input);
            while (getchar() != '\n');

            if (strcmp(input, "0") == 0) {
                sprintf(message, "❌ Load cancelled");
                showmessage = 1;
                break;
            }

            if (strlen(input) < 4 || strcmp(input + strlen(input) - 4, ".txt") != 0) {
                strcat(input, ".txt");
            }

            if (loadfile(input)) {
                sprintf(message, "✅ File loaded successfully: %s", input);
                showmessage = 1;
                textndraftnow();

                printf("--------------------------------------------------\n");
                printf("%s\n", message);
                printf("--------------------------------------------------\n");
                message[0] = '\0';
                showmessage = 0;
            }

            else {
                sprintf(message, "❌ Failed to load file: %s", input);
                showmessage = 1;
            }
            break;

        case 7: //save txt
            printf("Enter filename to save (.txt) or 0 to cancel: ");
            scanf("%s", input);
            while (getchar() != '\n');

            if (strcmp(input, "0") == 0) {
                sprintf(message, "❌ Save cancelled");
                showmessage = 1;
                break;
            }

            if (strlen(input) < 4 || strcmp(input + strlen(input) - 4, ".txt") != 0) {
                strcat(input, ".txt");
            }

            savefile(input);
            sprintf(message, "File operation completed: %s", input);
            showmessage = 1;
            break;

        case 8: //about us
            aboutus();
            break;

        case 0: //end
            break;
    }

    if (showmessage && message[0] != '\0') {
        printf("\n--------------------------------------------------\n");
        printf("%s\n", message);
        printf("--------------------------------------------------\n");
    }

    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}